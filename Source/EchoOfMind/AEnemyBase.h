#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyTier.h"
#include "EnemyAttackPattern.h"
#include "EnemyArchetype.h"
#include "Perception/AIPerceptionTypes.h"
#include "AEnemyBase.generated.h"

class UAttributeComponent;
class UEnemyLootComponent;
class ULootRarityTable;
class UItemDefinition;
class UBehaviorTree;
class UBlackboardData;
class AEchoPlayer;

class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class ECHOOFMIND_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyBase();

	/** 아키타입(DataAsset) : 스탯/패턴/드랍 등 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
	UEnemyArchetype* Archetype = nullptr;

	/** 능력치(HP/공격력 등) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAttributeComponent* Attributes;

	/** 전리품 컨테이너(시체에 담김) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UEnemyLootComponent* Loot;

	/** 죽음 몽타주(선택) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	class UAnimMontage* DeathMontage = nullptr;

	/** 플레이어가 시체를 상호작용할 때 호출 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Interact(class AEchoPlayer* Player);

	/** 컨트롤러 Perception에서 콜백으로 호출 */
	UFUNCTION() void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	/** AnimNotify 래퍼(애님에서 바로 호출하기 편하게) */
	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	void AnimNotify_EnemyHitStart();
	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	void AnimNotify_EnemyHitEnd();

	bool bIsDead = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat|State")
	bool bIsAttacking = false;
protected:
	/** BT/BB 자산(컨트롤러에서 Run) */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTreeAsset;
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBlackboardData* BlackboardAsset;

	// ---- 런타임 캐시 ----
	UPROPERTY(VisibleInstanceOnly, Category = "Enemy")
	EEnemyTier Tier = EEnemyTier::Small;

	UPROPERTY(VisibleInstanceOnly, Category = "Enemy")
	float BaseAttackPower = 20.f;

	UPROPERTY(VisibleInstanceOnly, Category = "Enemy")
	float MoveSpeed = 200.f;

	/** 공격 패턴 & 쿨다운 타임 */
	UPROPERTY(VisibleInstanceOnly, Category = "Enemy")
	TArray<FEnemyAttackPattern> AttackPatterns;

	UPROPERTY(Transient)
	TArray<float> AttackLastUseTime;

	/** 현재 타깃(서비스/Perception에서 갱신) */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	AActor* CurrentTarget = nullptr;


	FTimerHandle LootCheckTimer;

	// ──────────────── 전투 (StaticMesh 무기 / 맨손 자동 전환) ────────────────
	/** 무기 메시(Static). 지정되어 있으면 무기 콜리전 방식, 아니면 맨손 스윕 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Weapon")
	UStaticMeshComponent* WeaponMesh;

	/** 무기 히트박스(WeaponMesh의 자식). 히트 윈도우에서만 Query */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Weapon")
	UBoxComponent* WeaponHitBox;

	/** 무기 부착 소켓 (메인 스켈레탈 메시에 존재하는 손 소켓 이름) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Weapon")
	FName WeaponAttachSocket = TEXT("RightHandSocket");

	/** 무기 히트박스 기본 크기 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Weapon")
	FVector WeaponHitBoxExtent = FVector(6.f, 40.f, 4.f);

	/** 현재 프레임이 히트 윈도우인지 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Weapon")
	bool bWeaponHitWindowActive = false;

	/** 같은 히트 윈도우 내 중복 타격 방지 */
	TSet<TWeakObjectPtr<AActor>> HitActorsThisWindow;

	/** (맨손시) 노티파이에서 원형 스윕을 자동 수행할지 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Melee")
	bool bDoMeleeOnNotifyWhenNoWeapon = true;

	/** 맨손 스윕 기본값 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Melee")
	float MeleeRange = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Melee")
	float MeleeRadius = 60.f;

protected:
	/** 현재 이 적이 무기를 사용하는가? (WeaponMesh에 StaticMesh가 세팅되어 있으면 true) */
	bool IsUsingWeapon() const;

	/** 무기 히트 윈도우 ON/OFF (AnimNotify에서 호출 추천) */
	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	void EnableWeaponHitWindow();
	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	void DisableWeaponHitWindow();

	/** 무기 히트박스 오버랩 */
	UFUNCTION()
	void OnWeaponBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	// ──────────────── XP 보상 ────────────────
	UPROPERTY(EditAnywhere, Category = "Reward")
	bool bScaleXPByTier = true;

	UPROPERTY(VisibleInstanceOnly, Category = "Reward")
	TWeakObjectPtr<AActor> LastDamageInstigator;

	void AwardXPToKiller();

public:
	// ===== Getter/전투 유틸 =====
	UFUNCTION(BlueprintPure, Category = "AI")
	FORCEINLINE int32 GetAttackPatternNum() const { return AttackPatterns.Num(); }

	UFUNCTION(BlueprintPure, Category = "AI")
	bool GetAttackPatternAt(int32 Index, FEnemyAttackPattern& Out) const
	{
		if (!AttackPatterns.IsValidIndex(Index)) return false;
		Out = AttackPatterns[Index];
		return true;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bCanAttack = true;

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool CanUseAttackIndex(int32 Index) const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void MarkAttackUsed(int32 Index);

	/** (무기 없는 적) 근접 판정: 전방 원형 스윕 */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ApplyMeleeHit(float Range, float Radius, float Damage);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartAttackCooldown(float CooldownTime);

	UFUNCTION(BlueprintPure, Category = "AI")
	UBehaviorTree* GetBehaviorTreeAsset() const { return BehaviorTreeAsset; }
	UFUNCTION(BlueprintPure, Category = "AI")
	UBlackboardData* GetBlackboardAsset()  const { return BlackboardAsset; }

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;
	void InitializeFromArchetype();

	UFUNCTION() void OnDeath();

	void GenerateLoot();
	void CheckLootAndAutoDestroy();

	EItemRarity      RollRarityFromTable(const ULootRarityTable* Table) const;
	UItemDefinition* PickFromPool(const TArray<UItemDefinition*>& Pool) const;
};
