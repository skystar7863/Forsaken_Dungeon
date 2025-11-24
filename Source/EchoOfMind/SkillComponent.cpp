#include "SkillComponent.h"
#include "Skill.h"
#include "EchoPlayer.h"
#include "Engine/World.h"

USkillComponent::USkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	ActiveSkills.SetNum(2); // 슬롯 0, 1 초기화
}

void USkillComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USkillComponent::EquipSkill(int32 SlotIndex, TSubclassOf<USkill> SkillClass)
{
	if (SlotIndex < 0 || SlotIndex >= ActiveSkills.Num()) return;
	if (!AvailableSkillClasses.Contains(SkillClass)) return;

	if (ActiveSkills[SlotIndex])
	{
		ActiveSkills[SlotIndex]->ConditionalBeginDestroy();
		ActiveSkills[SlotIndex] = nullptr;
	}

	if (SkillClass)
	{
		ActiveSkills[SlotIndex] = NewObject<USkill>(this, SkillClass);
	}
}

void USkillComponent::UseSkill(int32 SlotIndex)
{
	if (!ActiveSkills.IsValidIndex(SlotIndex)) return;

	USkill* Skill = ActiveSkills[SlotIndex];
	if (!Skill) return;

	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;

	// 쿨다운 체크
	if (Now - Skill->LastUseTime < Skill->Cooldown)
		return;

	Skill->LastUseTime = Now;

	if (AEchoPlayer* Player = Cast<AEchoPlayer>(GetOwner()))
	{
		Skill->Execute(Player);
	}
}

void USkillComponent::ResetAllCooldowns()
{
	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;

	for (USkill* S : ActiveSkills)
	{
		if (!S) continue;
		// 최근 사용 시간을 "현재 - 쿨다운"으로 되돌려서 즉시 사용 가능 상태로
		S->LastUseTime = Now - S->Cooldown;
	}
}
