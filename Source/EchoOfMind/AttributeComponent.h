#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"

// ── Delegates ──
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnManaChanged, float, NewMana);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnXPChanged, int32, Level, float, CurrentXP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelUp, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTakeDamage, float, DamageAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackPowerChanged, float, NewAttackPower);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ECHOOFMIND_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAttributeComponent();

	// ─── 레벨/경험치 ───
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	int32 Level;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	float CurrentXP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level")
	float XPPerLevel;

	UPROPERTY(BlueprintAssignable, Category = "Level")
	FOnXPChanged OnXPChanged;

	UPROPERTY(EditAnywhere, Category = "Level")
	FOnLevelUp OnLevelUp;

	// ─── 기본 스탯 ───
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	int32 Strength = 25;

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	int32 Agility = 15;*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	int32 Vitality = 25;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	int32 Intelligence = 25;

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	int32 Dexterity = 15;*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	float MaxMana = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	float Mana;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	float AttackPower = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	float Defense = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	float MoveSpeedMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	float OutgoingDamageMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	float IncomingDamageMultiplier = 1.0f;

	// ─── 이벤트 ───
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTakeDamage OnTakeDamage;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnManaChanged OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAttackPowerChanged OnAttackPowerChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeath OnDeath;

	// 포션 HOT 타이머(사용 중이면 존재)
	FTimerHandle PotionHoTHandle;
	int32 PotionHoT_TicksLeft = 0;

	// ─── API ───
	UFUNCTION(BlueprintCallable, Category = "Level")
	void AddXP(float Amount);

	/** 레벨/경험치만 초기화(스탯은 유지) */
	UFUNCTION(BlueprintCallable, Category = "Level")
	void ResetLevelAndXP(int32 NewLevel = 1);

	/** ✅ 레벨/경험치 + 모든 스탯/배수까지 “초기값”으로 완전 리셋 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void ResetAllToDefaults(class UEquipmentComponent* Equipment = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float TakeDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float AddHealth(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float Heal(float HealAmount);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool UseMana(float ManaCost);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float RestoreMana(float ManaAmount);

	// 스탯 수정 헬퍼
	void ModifyStrength(int32 Delta);
	/*void ModifyAgility(int32 Delta) { Agility += Delta; }*/
	void ModifyVitality(int32 Delta) { Vitality += Delta; }
	void ModifyIntelligence(int32 Delta) { Intelligence += Delta; }
	/*void ModifyDexterity(int32 Delta) { Dexterity += Delta; }*/

	/*UFUNCTION(BlueprintCallable, Category = "Attributes")
	void ModifyAttackPower(float Delta);*/

	/*UFUNCTION(BlueprintCallable, Category = "Attributes")
	void ModifyMaxHP(float Delta);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void ModifyDefense(float Delta);*/
public:
	virtual void BeginPlay() override;

	// 내부 처리/브로드캐스트
	void BroadcastXP();
	void BroadcastHealth();
	void BroadcastMana();
	void BroadcastAttackPower();
	void HandleDeath();
	void HandleLevelUp();

	/** 파생값 재계산(비율 보존 옵션) */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void RecalculateFromPrimary(bool bPreserveRatios = true);

private:
	// BeginPlay 시점 기준 “원래 값” 스냅샷 (리셋 시 복원)
	UPROPERTY(Transient) int32 InitLevel = 1;
	UPROPERTY(Transient) int32 InitStrength = 25;
	UPROPERTY(Transient) int32 InitVitality = 25;
	UPROPERTY(Transient) int32 InitIntelligence = 25;
	UPROPERTY(Transient) float InitDefense = 5.f;
	UPROPERTY(Transient) float InitMoveSpeedMultiplier = 1.0f;
	UPROPERTY(Transient) float InitOutgoingDamageMultiplier = 1.0f;
	UPROPERTY(Transient) float InitIncomingDamageMultiplier = 1.0f;
};
