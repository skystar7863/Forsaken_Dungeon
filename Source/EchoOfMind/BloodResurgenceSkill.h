#pragma once

#include "CoreMinimal.h"
#include "Skill.h"
#include "BloodResurgenceSkill.generated.h"

class AEchoPlayer;
class UAttributeComponent;

/**
 * Blood Resurgence
 * - 발동 시 현재 체력의 5% 감소
 * - 이후 12초 동안 최대체력의 40% 회복
 */
UCLASS(Blueprintable)
class ECHOOFMIND_API UBloodResurgenceSkill : public USkill
{
    GENERATED_BODY()

public:
    UBloodResurgenceSkill();

    /** 치료 지속 시간(초) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BloodResurgence")
    float Duration = 12.f;

    /** 총 회복 비율(%), 0.4 = 최대체력의 40% */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BloodResurgence")
    float TotalHealRatio = 0.4f;

private:
    /** 내부 타이머 핸들 */
    FTimerHandle TimerHandle_Heal;

    /** 남은 Tick 횟수 */
    int32 TicksDone;

    /** 총 Tick 횟수 */
    int32 TotalTicks;

    /** 한 Tick 당 회복량 */
    float HealPerTick;

    /** 플레이어 & 컴포넌트 캐시 */
    TWeakObjectPtr<AEchoPlayer> OwnerPlayer;
    TWeakObjectPtr<UAttributeComponent> AttrComp;

    /** 매 Tick 호출 */
    void DoHealTick();

    // Execute_Implementation 선언
    virtual void Execute_Implementation(AEchoPlayer* Player) override;
};
