#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillComponent.generated.h"

class USkill;

/**
 * 플레이어가 “장착”한 스킬(2개)을 관리합니다.
 * 슬롯 0,1 에 EquipSkill 으로 원하는 스킬을 세팅하고
 * UseSkill(0/1) 으로 실행하세요.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ECHOOFMIND_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USkillComponent();

	/** 선택 가능한 스킬 풀 */
	UPROPERTY(EditAnywhere, Category = "Skills")
	TArray<TSubclassOf<USkill>> AvailableSkillClasses;

	/** 슬롯별 장착된 스킬 인스턴스(0~1) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skills")
	TArray<USkill*> ActiveSkills;

	UFUNCTION(BlueprintCallable, Category = "Skills")
	void EquipSkill(int32 SlotIndex, TSubclassOf<USkill> SkillClass);

	/** 지정한 슬롯의 스킬 사용 */
	void UseSkill(int32 SlotIndex);

	/** ✅ 모든 스킬 쿨다운 초기화 */
	UFUNCTION(BlueprintCallable, Category = "Skills")
	void ResetAllCooldowns();

protected:
	virtual void BeginPlay() override;
};
