#include "LootChestActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Item/EnemyLootComponent.h"
#include "Item/ItemDefinition.h"
#include "EchoPlayer.h"
#include "EchoPlayerController.h"
#include "Item/InventoryRootWidget.h"
#include "Kismet/GameplayStatics.h" // ✅ 추가
#include "Kismet/KismetMathLibrary.h"

ALootChestActor::ALootChestActor()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);

	// Root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// Base
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(Root);
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BaseMesh->SetCollisionResponseToAllChannels(ECR_Block);
	BaseMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // 라인트레이스 적중

	// LidPivot (필요 시 위치만 조정, 회전은 사용하지 않음)
	LidPivot = CreateDefaultSubobject<USceneComponent>(TEXT("LidPivot"));
	LidPivot->SetupAttachment(BaseMesh);

	// Lid (뚜껑 – 실제 회전 대상)
	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidMesh"));
	LidMesh->SetupAttachment(LidPivot);
	LidMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	LidMesh->SetCollisionResponseToAllChannels(ECR_Block);

	// Loot
	Loot = CreateDefaultSubobject<UEnemyLootComponent>(TEXT("Loot"));

	// 닫힘/열림 회전(로컬 Pitch)
	LidRot_Closed = FRotator::ZeroRotator;
	LidRot_Open = FRotator(0.f, 0.f, OpenAngle); // X축(Pitch)으로 OpenAngle만큼
}

void ALootChestActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bPlayingAnim) return;

	AnimElapsed += DeltaSeconds;
	const float Alpha = FMath::Clamp(AnimElapsed / FMath::Max(0.01f, OpenDuration), 0.f, 1.f);
	UpdateLid(Alpha);

	if (Alpha >= 1.f)
	{
		bPlayingAnim = false;
		SetActorTickEnabled(false);
	}
}

void ALootChestActor::UpdateLid(float Alpha)
{
	// LidMesh 로컬 회전 보간
	const FRotator Target = UKismetMathLibrary::RLerp(LidRot_Closed, LidRot_Open, Alpha, true);
	LidMesh->SetRelativeRotation(Target);
}

void ALootChestActor::PlayOpen()
{
	if (bLidOpen) return;          // 이미 열린 경우 무시(한번 열리면 계속 오픈 상태 유지)
	bLidOpen = true;
	bPlayingAnim = true;
	AnimElapsed = 0.f;
	SetActorTickEnabled(true);

	// ✅ 열리는 사운드 재생
	PlayOpenSound();
}

bool ALootChestActor::IsEquipmentType(EItemType Type) const
{
	return Type == EItemType::Weapon_Sword ||
		Type == EItemType::Weapon_Staff ||
		Type == EItemType::Helmet ||
		Type == EItemType::Chest ||
		Type == EItemType::Leg ||
		Type == EItemType::Boots;
}

int32 ALootChestActor::GetDropCount() const
{
	if (DropsOverride > 0) return DropsOverride;
	switch (ChestSize)
	{
	case EChestSize::Small:  return 2;
	case EChestSize::Medium: return 3;
	case EChestSize::Large:  return 4;
	default:                 return 2;
	}
}

EItemRarity ALootChestActor::RollRarity() const
{
	const float r = FMath::FRandRange(0.f, 100.f);

	if (ChestSize == EChestSize::Small)
	{
		// Common 40, Rare 40, Superior 15, Heroic 5, Legendary 0
		if (r < 40) return EItemRarity::Common;
		if (r < 80) return EItemRarity::Rare;
		if (r < 95) return EItemRarity::Superior;
		if (r < 100) return EItemRarity::Heroic;
		return EItemRarity::Legendary;
	}
	else if (ChestSize == EChestSize::Medium)
	{
		// Rare 45, Superior 35, Heroic 18, Legendary 2
		if (r < 45) return EItemRarity::Rare;
		if (r < 80) return EItemRarity::Superior;
		if (r < 98) return EItemRarity::Heroic;
		return EItemRarity::Legendary;
	}
	else
	{
		// Superior 45, Heroic 45, Legendary 10
		if (r < 45) return EItemRarity::Superior;
		if (r < 90) return EItemRarity::Heroic;
		return EItemRarity::Legendary;
	}
}

void ALootChestActor::RollExtraPerks(const FItemInstance& Base, TArray<EExtraPerk>& OutPerks) const
{
	OutPerks.Reset();

	const int32 Needed =
		(Base.Rarity == EItemRarity::Heroic) ? 1 :
		(Base.Rarity == EItemRarity::Legendary) ? 2 : 0;

	if (Needed <= 0) return;

	TArray<EExtraPerk> Candidates = {
		EExtraPerk::Plus5Pct_Str,
		EExtraPerk::Plus5Pct_Agi,
		EExtraPerk::Plus5Pct_Int,
		EExtraPerk::Plus5Pct_Vit,
		EExtraPerk::Plus3Pct_MoveSpeed,
		EExtraPerk::Plus5_Defense,
		EExtraPerk::Bleed5for5s_NonWeapon
	};

	// 무기에는 출혈 제외
	if (Base.Def && (Base.Def->ItemType == EItemType::Weapon_Sword || Base.Def->ItemType == EItemType::Weapon_Staff))
	{
		Candidates.Remove(EExtraPerk::Bleed5for5s_NonWeapon);
	}

	for (int32 i = 0; i < Needed && Candidates.Num() > 0; ++i)
	{
		const int32 idx = FMath::RandRange(0, Candidates.Num() - 1);
		OutPerks.Add(Candidates[idx]);
		Candidates.RemoveAt(idx);
	}
}

void ALootChestActor::GenerateLoot()
{
	if (!Loot) return;

	Loot->Clear();

	// 풀에서 장비류만 필터
	TArray<UItemDefinition*> EquipPool;
	for (UItemDefinition* Def : ItemPool)
	{
		if (Def && IsEquipmentType(Def->ItemType))
			EquipPool.Add(Def);
	}

	const int32 Drops = GetDropCount();

	for (int32 i = 0; i < Drops && EquipPool.Num() > 0; ++i)
	{
		const int32 idx = FMath::RandRange(0, EquipPool.Num() - 1);
		UItemDefinition* Def = EquipPool[idx];

		FItemInstance Inst;
		Inst.Def = Def;
		Inst.Rarity = RollRarity();
		Inst.StackCount = 1;

		RollExtraPerks(Inst, Inst.ExtraPerks);
		Loot->AddItem(Inst);
	}
}

void ALootChestActor::Interact(AEchoPlayer* Player)
{
	if (!Player || !Loot) return;

	if (!bOpenedOnce)
	{
		GenerateLoot();
		bOpenedOnce = true;
	}

	// 뚜껑 열기(한 번 열리면 계속 열림)
	PlayOpen();

	// UI 열기
	if (AEchoPlayerController* PC = Player->GetPlayerController())
	{
		PC->ShowInventory(true);
		if (UInventoryRootWidget* RootW = PC->GetInventoryRoot())
		{
			RootW->OpenLoot(Loot, this); // 포인터 전달해도 OnLootWidgetClosed는 아무 것도 안 함
		}
	}
}

void ALootChestActor::OnLootWidgetClosed()
{
	// 더 이상 닫지 않음(상자는 열린 채 유지)
}


void ALootChestActor::PlayOpenSound()
{
	if (ChestOpenSound && LidMesh)
	{
		UGameplayStatics::SpawnSoundAttached(
			ChestOpenSound,
			LidMesh,
			NAME_None,
			FVector::ZeroVector,
			EAttachLocation::KeepRelativeOffset,
			true);
	}
}