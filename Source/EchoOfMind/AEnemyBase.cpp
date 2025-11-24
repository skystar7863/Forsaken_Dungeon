#include "AEnemyBase.h"
#include "AttributeComponent.h"
#include "DrawDebugHelpers.h"

// 전리품/아이템
#include "Item/EnemyLootComponent.h"
#include "Item/ItemDefinition.h"
#include "Item/ItemEnums.h"

// 컴포넌트/AI/애님
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "AEnemyAIController.h"                 // ⬅ 네 프로젝트의 AI 컨트롤러 헤더

// 물리/충돌
#include "PhysicsEngine/PhysicsAsset.h"
#include "Engine/CollisionProfile.h"

// 상호작용/인벤토리 UI
#include "EchoPlayer.h"
#include "EchoPlayerController.h"
#include "Item/InventoryRootWidget.h"

// 데미지 & Perception 보고
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Damage.h"

#include "Engine/World.h"
#include "TimerManager.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bIsAttacking = false; // ✅ 기본 비활성화

	// ✅ 스폰/배치 시 자동으로 AIController 생성/소유 보장
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AEnemyAIController::StaticClass();

	// ---- 컴포넌트 ----
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	Loot = CreateDefaultSubobject<UEnemyLootComponent>(TEXT("Loot"));

	// 무기 메시 (StaticMesh). 메인 스켈레탈 메쉬의 손 소켓에 붙일 예정
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetMesh(), TEXT("RightHandSocket"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetGenerateOverlapEvents(false);

	// 무기 히트박스 (WeaponMesh 자식)
	WeaponHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponHitBox"));
	WeaponHitBox->SetupAttachment(WeaponMesh);
	WeaponHitBox->SetBoxExtent(WeaponHitBoxExtent);
	WeaponHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 히트 윈도우에만 켬
	WeaponHitBox->SetCollisionObjectType(ECC_WorldDynamic);
	WeaponHitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponHitBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 플레이어 캡슐과 겹치면 히트
	WeaponHitBox->SetGenerateOverlapEvents(true);

	// 이동
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 540, 0);

	// 살아있을 때 Mesh 충돌 최소
	if (USkeletalMeshComponent* SkelMesh = GetMesh())
	{
		SkelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SkelMesh->SetGenerateOverlapEvents(false);
	}
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	InitializeFromArchetype();

	if (Attributes)
	{
		Attributes->OnDeath.AddDynamic(this, &AEnemyBase::OnDeath);
	}

	// 무기 메시를 손 소켓에 스냅 (StaticMeshComponent도 소켓에 붙을 수 있음)
	if (WeaponMesh && !WeaponAttachSocket.IsNone())
	{
		WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocket);
	}

	// 무기 히트박스 바인딩
	if (WeaponHitBox)
	{
		WeaponHitBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::OnWeaponBeginOverlap);
		WeaponHitBox->IgnoreActorWhenMoving(this, true);
	}
}

void AEnemyBase::InitializeFromArchetype()
{
	if (!Archetype || !Attributes) return;

	// 스탯
	Tier = Archetype->Tier;
	BaseAttackPower = Archetype->AttackPower;
	MoveSpeed = Archetype->MoveSpeed;

	Attributes->MaxHealth = Archetype->MaxHealth;
	Attributes->Health = Attributes->MaxHealth;
	Attributes->AttackPower = BaseAttackPower;

	// 이동속도
	if (auto* Move = GetCharacterMovement())
	{
		Move->MaxWalkSpeed = MoveSpeed;
	}

	// 공격 패턴/쿨타임
	AttackPatterns = Archetype->AttackPatterns;
	AttackLastUseTime.SetNum(AttackPatterns.Num());
	for (int32 i = 0; i < AttackLastUseTime.Num(); ++i)
	{
		AttackLastUseTime[i] = -10000.f;
	}
}

// ===== 유틸 =====
bool AEnemyBase::IsUsingWeapon() const
{
	if (!WeaponMesh) return false;
	return WeaponMesh->GetStaticMesh() != nullptr;
}

bool AEnemyBase::CanUseAttackIndex(int32 Index) const
{
	if (!GetWorld() || !AttackPatterns.IsValidIndex(Index)) return false;
	const float Now = GetWorld()->GetTimeSeconds();
	return (Now - AttackLastUseTime[Index]) >= AttackPatterns[Index].Cooldown;
}

void AEnemyBase::MarkAttackUsed(int32 Index)
{
	if (!GetWorld() || !AttackPatterns.IsValidIndex(Index)) return;
	AttackLastUseTime[Index] = GetWorld()->GetTimeSeconds();
}

// ===== 무기 히트 윈도우 =====
void AEnemyBase::EnableWeaponHitWindow()
{
	if (!IsUsingWeapon() || !WeaponHitBox) return;

	bWeaponHitWindowActive = true;
	HitActorsThisWindow.Empty();
	WeaponHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemyBase::DisableWeaponHitWindow()
{
	if (!WeaponHitBox) return;

	bWeaponHitWindowActive = false;
	WeaponHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitActorsThisWindow.Empty();
}

void AEnemyBase::AnimNotify_EnemyHitStart()
{
	if (IsUsingWeapon())
	{
		EnableWeaponHitWindow();
	}
	else if (bDoMeleeOnNotifyWhenNoWeapon)
	{
		// 맨손이면 노티파이 타이밍에 1회 스윕 공격 수행
		const float Base = Attributes ? Attributes->AttackPower : 20.f;
		const float Dmg = Attributes ? (Base * Attributes->OutgoingDamageMultiplier) : Base;
		ApplyMeleeHit(MeleeRange, MeleeRadius, Dmg);
	}
}

void AEnemyBase::AnimNotify_EnemyHitEnd()
{
	if (IsUsingWeapon())
	{
		DisableWeaponHitWindow();
	}
}

void AEnemyBase::OnWeaponBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!bWeaponHitWindowActive) return;
	if (!OtherActor || OtherActor == this) return;

	// 플레이어만 타격
	AEchoPlayer* Player = Cast<AEchoPlayer>(OtherActor);
	if (!Player) return;

	// 중복 타격 방지
	if (HitActorsThisWindow.Contains(Player)) return;

	// 데미지
	const float Base = Attributes ? Attributes->AttackPower : 20.f;
	const float Dmg = Attributes ? (Base * Attributes->OutgoingDamageMultiplier) : Base;

	UGameplayStatics::ApplyDamage(
		/* DamagedActor    */ Player,
		/* BaseDamage      */ Dmg,
		/* EventInstigator */ GetController(),
		/* DamageCauser    */ this,
		/* DamageType      */ nullptr
	);

	HitActorsThisWindow.Add(Player);
}

// ===== 맨손 스윕 =====
void AEnemyBase::ApplyMeleeHit(float Range, float Radius, float Damage)
{
	// 무기 있는 적은 이 함수 대신 히트박스를 통해 처리
	if (IsUsingWeapon())
	{
		return;
	}

	const FVector Start = GetActorLocation();
	const FVector End = Start + GetActorForwardVector() * Range;
	FCollisionShape Shape = FCollisionShape::MakeSphere(Radius);

	TArray<FHitResult> Hits;
	const bool bHit = GetWorld()->SweepMultiByChannel(Hits, Start, End, FQuat::Identity, ECC_Pawn, Shape);

#if WITH_EDITOR
	//DrawDebugSphere(GetWorld(), End, Radius, 12, bHit ? FColor::Green : FColor::Red, false, 0.25f);
#endif

	for (auto& H : Hits)
	{
		if (AActor* Other = H.GetActor())
		{
			if (Other == this) continue;

			// 플레이어만 타격
			if (!Cast<AEchoPlayer>(Other)) continue;

			UGameplayStatics::ApplyDamage(
				/* DamagedActor   */ Other,
				/* BaseDamage     */ Damage,
				/* EventInstigator*/ GetController(),
				/* DamageCauser   */ this,
				/* DamageType     */ nullptr
			);
		}
	}
}

// ===== 데미지 라우팅 =====
float AEnemyBase::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	if (!Attributes) return 0.f;

	AActor* InstigatorActor = DamageCauser;
	if (!InstigatorActor && EventInstigator) { InstigatorActor = EventInstigator->GetPawn(); }

	// 마지막 가해자 저장 → 처치 시 XP 지급
	LastDamageInstigator = InstigatorActor;

	UAISense_Damage::ReportDamageEvent(
		GetWorld(), this, InstigatorActor, DamageAmount,
		InstigatorActor ? InstigatorActor->GetActorLocation() : GetActorLocation(),
		GetActorLocation()
	);

	return Attributes->TakeDamage(DamageAmount);
}

// ===== 사망/전리품/상호작용 =====
void AEnemyBase::OnDeath()
{
	if (bIsDead) return;
	bIsDead = true;

	// AI 정지 + 캡슐 충돌 비활성
	DetachFromControllerPendingDestroy();
	if (auto* Move = GetCharacterMovement()) { Move->DisableMovement(); }
	if (auto* Cap = GetCapsuleComponent()) { Cap->SetCollisionEnabled(ECollisionEnabled::NoCollision); }

	// 죽음 몽타주(있으면, 라그돌 전 잠깐 재생 가능)
	if (DeathMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(DeathMontage, 1.f);
	}

	// === Mesh를 상호작용 + 물리(라그돌) 대상으로 전환 ===
	if (USkeletalMeshComponent* SkelMesh = GetMesh())
	{
		SkelMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SkelMesh->SetCollisionProfileName(FName(TEXT("Ragdoll")));
		SkelMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		SkelMesh->SetGenerateOverlapEvents(false);

		SkelMesh->SetAllBodiesSimulatePhysics(true);
		SkelMesh->SetSimulatePhysics(true);
		SkelMesh->WakeAllRigidBodies();
	}

	// 전리품 채우기/XP 지급
	AwardXPToKiller();
	GenerateLoot();

	// 루트가 비면 자동 삭제(1초마다 체크)
	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().SetTimer(
			LootCheckTimer, this, &AEnemyBase::CheckLootAndAutoDestroy, 1.0f, true, 1.0f);
	}
}

void AEnemyBase::AwardXPToKiller()
{
	AEchoPlayer* Killer = LastDamageInstigator.IsValid()
		? Cast<AEchoPlayer>(LastDamageInstigator.Get())
		: nullptr;

	if (!Killer) return;

	if (UAttributeComponent* Attr = Killer->FindComponentByClass<UAttributeComponent>())
	{
		float BaseXP = 25.f;
		if (Archetype && Archetype->XPReward > 0.f) BaseXP = Archetype->XPReward;

		float Mult = 1.f;
		if (bScaleXPByTier)
		{
			switch (Tier)
			{
			case EEnemyTier::Small:  Mult = 1.0f; break;
			case EEnemyTier::Medium: Mult = 1.5f; break;
			case EEnemyTier::Boss:   Mult = 3.0f; break;
			default:                 Mult = 1.0f; break;
			}
		}
		Attr->AddXP(BaseXP * Mult);
	}
}

void AEnemyBase::CheckLootAndAutoDestroy()
{
	if (!Loot) return;

	const bool bEmpty = Loot->IsEmpty();
	if (bEmpty)
	{
		if (UWorld* W = GetWorld())
		{
			W->GetTimerManager().ClearTimer(LootCheckTimer);
		}
		Destroy();
	}
}

void AEnemyBase::Interact(AEchoPlayer* Player)
{
	if (!bIsDead || !Player || !Loot) return;

	if (AEchoPlayerController* PC = Player->GetPlayerController())
	{
		PC->ShowInventory(true);
		if (UInventoryRootWidget* Root = PC->GetInventoryRoot())
		{
			Root->OpenLoot(Loot);
		}
	}
}

void AEnemyBase::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor) return;

	// 플레이어만 허용
	if (!Cast<AEchoPlayer>(Actor)) return;

	if (Stimulus.WasSuccessfullySensed())
	{
		CurrentTarget = Actor;

		if (AAIController* AC = Cast<AAIController>(GetController()))
		{
			if (UBlackboardComponent* BB = AC->GetBlackboardComponent())
			{
				static const FName BBKEY_TargetActor(TEXT("TargetActor"));
				BB->SetValueAsObject(BBKEY_TargetActor, Actor);
			}
		}
	}
}

// ===== 전리품 생성 =====
EItemRarity AEnemyBase::RollRarityFromTable(const ULootRarityTable* Table) const
{
	if (!Table || Table->Weights.Num() == 0) return EItemRarity::Common;

	float Sum = 0.f;
	for (const auto& W : Table->Weights) Sum += W.Weight;
	if (Sum <= 0.f) return EItemRarity::Common;

	const float R = FMath::FRandRange(0.f, Sum);
	float Acc = 0.f;
	for (const auto& W : Table->Weights)
	{
		Acc += W.Weight;
		if (R <= Acc) return W.Rarity;
	}
	return Table->Weights.Last().Rarity;
}

UItemDefinition* AEnemyBase::PickFromPool(const TArray<UItemDefinition*>& Pool) const
{
	if (Pool.Num() == 0) return nullptr;
	const int32 Idx = FMath::RandRange(0, Pool.Num() - 1);
	return Pool[Idx];
}

void AEnemyBase::GenerateLoot()
{
	if (!Loot || !Archetype) return;

	Loot->Clear();

	const int32 Count = FMath::Max(0, Archetype->DropCount);
	for (int32 i = 0; i < Count; ++i)
	{
		const EItemRarity R = RollRarityFromTable(Archetype->RarityTable);

		FItemInstance Inst;
		Inst.Rarity = R;
		Inst.StackCount = 1;

		if (R == EItemRarity::Consumable)
		{
			Inst.Def = PickFromPool(Archetype->ConsumablePool);
		}
		else
		{
			Inst.Def = PickFromPool(Archetype->EquipmentPool);

			// 영웅/전설 추가 Perk
			if (R == EItemRarity::Heroic || R == EItemRarity::Legendary)
			{
				const int32 Needed = (R == EItemRarity::Heroic) ? 1 : 2;
				TArray<EExtraPerk> Candidates = {
					EExtraPerk::Plus5Pct_Str, EExtraPerk::Plus5Pct_Agi, EExtraPerk::Plus5Pct_Int,
					EExtraPerk::Plus5Pct_Vit, EExtraPerk::Plus3Pct_MoveSpeed, EExtraPerk::Plus5_Defense,
					EExtraPerk::Bleed5for5s_NonWeapon
				};
				// 무기면 출혈 제외
				if (Inst.Def && (Inst.Def->ItemType == EItemType::Weapon_Sword || Inst.Def->ItemType == EItemType::Weapon_Staff))
				{
					Candidates.Remove(EExtraPerk::Bleed5for5s_NonWeapon);
				}
				for (int32 k = 0; k < Needed && Candidates.Num()>0; ++k)
				{
					const int32 Pick = FMath::RandRange(0, Candidates.Num() - 1);
					Inst.ExtraPerks.Add(Candidates[Pick]);
					Candidates.RemoveAt(Pick);
				}
			}
		}

		if (Inst.Def)
		{
			Loot->AddItem(Inst);
		}
	}
}

void AEnemyBase::StartAttackCooldown(float CooldownTime)
{
	bCanAttack = false;

	// 1초 후 다시 공격 가능
	if (UWorld* World = GetWorld())
	{
		FTimerHandle CooldownHandle;
		World->GetTimerManager().SetTimer(CooldownHandle, [this]()
			{
				bCanAttack = true;
			}, CooldownTime, false);
	}
}
