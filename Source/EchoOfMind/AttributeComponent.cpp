#include "AttributeComponent.h"
#include "Item/EquipmentComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	Level = 1;
	CurrentXP = 0.f;
	XPPerLevel = 100.f;

	MaxHealth = Vitality * 10.f;
	Health = MaxHealth;

	Mana = MaxMana;
}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	// 시작 시 실값 정리
	Health = FMath::Clamp(Health, 0.f, MaxHealth);
	BroadcastHealth();
	BroadcastMana();
	BroadcastXP();
	AttackPower += Strength;
	BroadcastAttackPower();

	// ✅ 초기 스냅샷(“원래 값”) 저장
	InitLevel = Level;
	InitStrength = Strength;
	InitVitality = Vitality;
	InitIntelligence = Intelligence;
	InitDefense = Defense;
	InitMoveSpeedMultiplier = MoveSpeedMultiplier;
	InitOutgoingDamageMultiplier = OutgoingDamageMultiplier;
	InitIncomingDamageMultiplier = IncomingDamageMultiplier;
}

void UAttributeComponent::AddXP(float Amount)
{
	CurrentXP += Amount;
	BroadcastXP();

	while (CurrentXP >= XPPerLevel)
	{
		CurrentXP -= XPPerLevel;
		HandleLevelUp();
	}
}

void UAttributeComponent::ResetLevelAndXP(int32 NewLevel)
{
	Level = FMath::Max(1, NewLevel);
	CurrentXP = 0.f;
	BroadcastXP();
}

void UAttributeComponent::ResetAllToDefaults(UEquipmentComponent* Equipment)
{
	// 1️⃣ 기본값으로 복원
	Level = FMath::Max(1, InitLevel);
	CurrentXP = 0.f;

	Strength = InitStrength;
	Vitality = InitVitality;
	Intelligence = InitIntelligence;
	Defense = InitDefense;
	MoveSpeedMultiplier = InitMoveSpeedMultiplier;
	OutgoingDamageMultiplier = InitOutgoingDamageMultiplier;
	IncomingDamageMultiplier = InitIncomingDamageMultiplier;

	// 2️⃣ 기본 능력치 기반 파생값 재계산
	RecalculateFromPrimary(false);

	// 3️⃣ 장비 효과 다시 적용
	if (Equipment)
	{
		const FItemStats EquippedStats = Equipment->GetTotalEquippedStats(); // ✅ 모든 장비 스탯 합산 함수
		Strength += EquippedStats.Strength;
		Vitality += EquippedStats.Vitality;
		Intelligence += EquippedStats.Intelligence;
		Defense += EquippedStats.Defense;

		// Vitality 기반 MaxHealth 다시 계산
		MaxHealth = Vitality * 10.f;

		// 공격력 재계산 (기본 공격력 + Strength)
		AttackPower = FMath::Max(0.f, AttackPower + Strength);
	}

	// 4️⃣ 체력, 마나 풀로 복구 및 브로드캐스트
	Health = MaxHealth;
	Mana = MaxMana;
	BroadcastHealth();
	BroadcastMana();
	BroadcastAttackPower();
	BroadcastXP();
}

float UAttributeComponent::TakeDamage(float DamageAmount)
{
	const float AdjustedDamage = FMath::Max(0.f, DamageAmount - Defense) * IncomingDamageMultiplier;
	OnTakeDamage.Broadcast(AdjustedDamage);

	Health = FMath::Clamp(Health - AdjustedDamage, 0.f, MaxHealth);
	BroadcastHealth();

	if (Health <= 0.f)
	{
		HandleDeath();
	}
	return AdjustedDamage;
}

float UAttributeComponent::AddHealth(float Amount)
{
	if (Amount <= 0.f) return Health;
	Health = FMath::Clamp(Health + Amount, 0.f, MaxHealth);
	BroadcastHealth();
	return Health;
}

float UAttributeComponent::Heal(float HealAmount)
{
	return AddHealth(HealAmount);
}

bool UAttributeComponent::UseMana(float ManaCost)
{
	if (ManaCost > Mana) return false;
	Mana = FMath::Clamp(Mana - ManaCost, 0.f, MaxMana);
	BroadcastMana();
	return true;
}

void UAttributeComponent::BroadcastAttackPower()
{
	OnAttackPowerChanged.Broadcast(AttackPower);
}


float UAttributeComponent::RestoreMana(float ManaAmount)
{
	Mana = FMath::Clamp(Mana + ManaAmount, 0.f, MaxMana);
	BroadcastMana();
	return Mana;
}

void UAttributeComponent::ModifyStrength(int32 Delta)
{
	Strength += Delta;
	AttackPower = FMath::Max(0.f, AttackPower + Delta);
	BroadcastAttackPower();
}


void UAttributeComponent::BroadcastXP()
{
	OnXPChanged.Broadcast(Level, CurrentXP);
}

void UAttributeComponent::BroadcastHealth()
{
	OnHealthChanged.Broadcast(Health);
}

void UAttributeComponent::BroadcastMana()
{
	OnManaChanged.Broadcast(Mana);
}

void UAttributeComponent::HandleDeath()
{
	OnDeath.Broadcast();
}

void UAttributeComponent::HandleLevelUp()
{
	Level++;

	// 레벨 업 스탯 증가 규칙
	ModifyStrength(1);
	/*ModifyAgility(1);*/
	ModifyVitality(1);
	ModifyIntelligence(1);
	/*ModifyDexterity(1);*/

	RecalculateFromPrimary(true);

	BroadcastHealth();
	BroadcastMana();
	OnLevelUp.Broadcast(Level);
	BroadcastXP();
}

void UAttributeComponent::RecalculateFromPrimary(bool bPreserveRatios)
{
	const float OldMaxH = MaxHealth;
	const float OldMaxM = MaxMana;
	const float OldH = Health;
	const float OldM = Mana;

	MaxHealth = FMath::Max(1.f, Vitality * 10.f);
	MaxMana = FMath::Max(0.f, Intelligence * 5.f);

	if (bPreserveRatios)
	{
		const float HRatio = (OldMaxH > 0.f) ? (OldH / OldMaxH) : 1.f;
		const float MRatio = (OldMaxM > 0.f) ? (OldM / OldMaxM) : 1.f;

		Health = FMath::Clamp(MaxHealth * HRatio, 0.f, MaxHealth);
		Mana = FMath::Clamp(MaxMana * MRatio, 0.f, MaxMana);
	}
	else
	{
		Health = FMath::Clamp(Health, 0.f, MaxHealth);
		Mana = FMath::Clamp(Mana, 0.f, MaxMana);
	}

	BroadcastHealth();
	BroadcastMana();
}

//void UAttributeComponent::ModifyAttackPower(float Delta)
//{
//	AttackPower = FMath::Max(0.0f, AttackPower + Delta);
//}

//void UAttributeComponent::ModifyMaxHP(float Delta)
//{
//	MaxHealth = FMath::Max(1.0f, MaxHealth + Delta);
//
//	// 현재 HP가 초과되지 않도록
//	if (Health > MaxHealth)
//		Health = MaxHealth;
//
//	OnHealthChanged.Broadcast(Health);
//}
//
//void UAttributeComponent::ModifyDefense(float Delta)
//{
//	Defense = FMath::Max(0.0f, Defense + Delta);
//}