#include "RunWorldDirector.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "RunSessionSubsystem.h"
#include "AEnemyBase.h"
#include "LootChestActor.h"
#include "EchoPlayer.h"
#include "AttributeComponent.h"
#include "SkillComponent.h"
#include "Item/PlayerInventoryComponent.h"
#include "Item/ConsumableContainerComponent.h"
#include "Item/ItemContainerComponent.h"
#include "LobbyStashComponent.h"
#include "Puzzle/RoomEncounterTrigger.h"
#include "Item/EquipmentComponent.h"   


ARunWorldDirector::ARunWorldDirector()
{
	PrimaryActorTick.bCanEverTick = true;

	CaptureVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("CaptureVolume"));
	RootComponent = CaptureVolume;
	CaptureVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CaptureVolume->SetBoxExtent(FVector(5000.f, 5000.f, 2000.f));

	// ✅ 로비 스태시(20칸)
	LobbyStash = CreateDefaultSubobject<ULobbyStashComponent>(TEXT("LobbyStash"));
}

void ARunWorldDirector::BeginPlay()
{
	Super::BeginPlay();

	RunSS = GetGameInstance()->GetSubsystem<URunSessionSubsystem>();
	VolumeBox = CaptureVolume->Bounds.GetBox();

	if (bBuildSnapshotOnBeginPlay)
	{
		BuildSnapshot();
	}

	// 플레이어 사망 이벤트 바인딩
	if (AEchoPlayer* P = GetPlayer())
	{
		if (UAttributeComponent* Attr = P->FindComponentByClass<UAttributeComponent>())
		{
			Attr->OnDeath.AddDynamic(this, &ARunWorldDirector::OnPlayerDied);
		}
	}

	// 월드 준비 완료 알림
	if (RunSS.IsValid())
	{
		RunSS->NotifyWorldReady();
	}
}

void ARunWorldDirector::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (RunSS.IsValid())
	{
		RunSS->TickRun(DeltaSeconds);
	}
}

AEchoPlayer* ARunWorldDirector::GetPlayer() const
{
	if (UWorld* World = GetWorld())
	{
		return Cast<AEchoPlayer>(UGameplayStatics::GetPlayerPawn(World, 0));
	}
	return nullptr;
}

bool ARunWorldDirector::IsInsideCapture(const FVector& Location) const
{
	return VolumeBox.IsInsideOrOn(Location);
}

void ARunWorldDirector::CollectInside(TSubclassOf<AActor> FilterClass, TArray<AActor*>& Out) const
{
	Out.Reset();
	for (TActorIterator<AActor> It(GetWorld(), FilterClass); It; ++It)
	{
		AActor* A = *It;
		if (A && IsInsideCapture(A->GetActorLocation()))
		{
			Out.Add(A);
		}
	}
}

void ARunWorldDirector::BuildSnapshot()
{
	EnemySnapshot.Reset();
	ChestSnapshot.Reset();

	// Enemy
	{
		TArray<AActor*> Found;
		CollectInside(AEnemyBase::StaticClass(), Found);
		for (AActor* A : Found)
		{
			FSpawnSnapshotEntry E;
			E.Class = A->GetClass();
			E.Transform = A->GetActorTransform();
			EnemySnapshot.Add(E);
		}
	}

	// Chest
	{
		TArray<AActor*> Found;
		CollectInside(ALootChestActor::StaticClass(), Found);
		for (AActor* A : Found)
		{
			FSpawnSnapshotEntry E;
			E.Class = A->GetClass();
			E.Transform = A->GetActorTransform();
			ChestSnapshot.Add(E);
		}
	}

	if (bDebugLog)
	{
		UE_LOG(LogTemp, Log, TEXT("[Director] Snapshot built: Enemies=%d, Chests=%d"),
			EnemySnapshot.Num(), ChestSnapshot.Num());
	}
}

void ARunWorldDirector::DestroyInside(TSubclassOf<AActor> FilterClass)
{
	TArray<AActor*> Found;
	CollectInside(FilterClass, Found);
	for (AActor* A : Found)
	{
		A->Destroy();
	}
}

void ARunWorldDirector::SpawnFrom(const TArray<FSpawnSnapshotEntry>& Snapshot)
{
	for (const FSpawnSnapshotEntry& E : Snapshot)
	{
		if (!*E.Class) continue;

		FActorSpawnParameters P;
		P.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AActor* Spawned = GetWorld()->SpawnActor<AActor>(E.Class, E.Transform, P);

		// 적이면 AI 컨트롤러 보장
		if (AEnemyBase* Enemy = Cast<AEnemyBase>(Spawned))
		{
			Enemy->SpawnDefaultController();
		}
	}
}

void ARunWorldDirector::ResetMapFromSnapshot()
{
	DestroyInside(AEnemyBase::StaticClass());
	DestroyInside(ALootChestActor::StaticClass());

	SpawnFrom(EnemySnapshot);
	SpawnFrom(ChestSnapshot);

	if (bDebugLog)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Director] Map reset from snapshot. Enemies=%d, Chests=%d"),
			EnemySnapshot.Num(), ChestSnapshot.Num());
	}
	OnWorldRepopulated.Broadcast();
}

void ARunWorldDirector::ResetPlayerVitalsAndCooldowns( bool bResetXP /*=true*/)
{
	//if (!P) return;

	//if (UAttributeComponent* Attr = P->FindComponentByClass<UAttributeComponent>())
	//{
	//	if (bResetXP)
	//	{
	//		Attr->ResetLevelAndXP(Attr->Level); // 현재 레벨 유지 + XP 0
	//	}
	//	Attr->Health = Attr->MaxHealth;
	//	Attr->Mana = Attr->MaxMana;
	//	Attr->BroadcastHealth();
	//	Attr->BroadcastMana();
	//}

	//if (USkillComponent* SK = P->FindComponentByClass<USkillComponent>())
	//{
	//	SK->ResetAllCooldowns();
	//}

	// 1) 플레이어 상태/인벤토리 초기화
	if (AEchoPlayer* P = GetPlayer())
	{
		ResetPlayerState(P);
		//ClearPlayerSessionInventory(P);
		TeleportPlayerToLobby();
	}

	// 2) 월드 초기화
	ResetWorld();

	// 3) Subsystem 타이머/상태 초기화
	if (RunSS.IsValid())
	{
		RunSS->ResetRunTimerAndPhase();
	}
}

// ===== 런 리셋 =====

void ARunWorldDirector::OnPlayerDied()
{
	ResetRunSession();
}

void ARunWorldDirector::ResetRunSession()
{
	// 1) 플레이어 상태/인벤토리 초기화
	if (AEchoPlayer* P = GetPlayer())
	{
		ResetPlayerState(P);
		ClearPlayerSessionInventory(P);
		TeleportPlayerToLobby();
	}

	// 2) 월드 초기화
	ResetWorld();

	// 3) Subsystem 타이머/상태 초기화
	if (RunSS.IsValid())
	{
		RunSS->ResetRunTimerAndPhase();
	}
}

void ARunWorldDirector::ResetPlayerState(AEchoPlayer* P)
{
	if (!P) return;

	if (UAttributeComponent* Attr = P->FindComponentByClass<UAttributeComponent>())
	{
		UEquipmentComponent* Equip = P->FindComponentByClass<UEquipmentComponent>();
		Attr->ResetAllToDefaults(Equip);   // 장비 스탯을 포함해서 초기화
	}

	if (USkillComponent* SK = P->FindComponentByClass<USkillComponent>())
	{
		SK->ResetAllCooldowns();
	}
}

void ARunWorldDirector::ClearPlayerSessionInventory(AEchoPlayer* P)
{
	if (!P) return;

	if (UPlayerInventoryComponent* Inv = P->FindComponentByClass<UPlayerInventoryComponent>())
	{
		Inv->ResetSessionInventory();
	}
}

void ARunWorldDirector::ResetWorld()
{
	ResetMapFromSnapshot();
}

void ARunWorldDirector::TeleportToAnchor(AActor* Anchor) const
{
	if (!Anchor) return;
	if (AEchoPlayer* P = GetPlayer())
	{
		P->SetActorLocation(Anchor->GetActorLocation());
		P->SetActorRotation(Anchor->GetActorRotation());
	}
}

void ARunWorldDirector::TeleportPlayerToLobby()
{
	TeleportToAnchor(LobbySpawn);
}

void ARunWorldDirector::TeleportPlayerToDungeon()
{
	TeleportToAnchor(DungeonSpawn);

	// ✅ 포탈 재진입 시 문/트리거 상태만 전체 초기화
	ResetAllEncounterTriggers();
}

void ARunWorldDirector::TeleportPlayerToBoss()
{
	TeleportToAnchor(BossSpawn);
}

void ARunWorldDirector::ResetAllEncounterTriggers() const
{
	for (TActorIterator<ARoomEncounterTrigger> It(GetWorld()); It; ++It)
	{
		if (IsValid(*It))
		{
			It->ResetDoorState();
		}
	}
}
