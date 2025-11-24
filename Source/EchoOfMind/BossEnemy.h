#pragma once
#include "CoreMinimal.h"
#include "AEnemyBase.h"
#include "NiagaraSystem.h"
#include "BossEnemy.generated.h"

UCLASS()
class ECHOOFMIND_API ABossEnemy : public AEnemyBase
{
	GENERATED_BODY()

public:
	ABossEnemy();

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	UAnimMontage* MeleeMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	UAnimMontage* RangeMontage;

	UPROPERTY(EditAnywhere, Category = "Boss|VFX")
	UNiagaraSystem* WarningCircleFX;

	UPROPERTY(EditAnywhere, Category = "Boss|VFX")
	UParticleSystem* StormFX;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	float StormRadius = 300.f;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	float StormDamagePerTick = 10.f;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	float StormTickInterval = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	float StormDuration = 2.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|Cooldown")
	float LastMeleeAttackTime = -FLT_MAX;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|Cooldown")
	float LastRangeAttackTime = -FLT_MAX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Cooldown")
	float MeleeCooldown = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Cooldown")
	float RangeCooldown_Phase1 = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Cooldown")
	float RangeCooldown_Phase2 = 3.0f;

	bool bIsAttacking = false;

	UFUNCTION()
	void ApplyStormDamage(FVector Center, float Radius, float DamagePerTick);

	int32 Phase = 1;

	UFUNCTION(BlueprintPure, Category = "Boss|Phase")
	int32 GetPhase() const { return Phase; }
protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHealthChanged(float NewHealth);
};
