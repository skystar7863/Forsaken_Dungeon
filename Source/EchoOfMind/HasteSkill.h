#pragma once

#include "CoreMinimal.h"
#include "Skill.h"
#include "HasteSkill.generated.h"

class AEchoPlayer;

/**
 * Haste 스킬
 * - 발동 시 이동 속도 20% 증가
 * - 지속 시간 20초
 * - 쿨타임 240초
 */
UCLASS(Blueprintable)
class ECHOOFMIND_API UHasteSkill : public USkill
{
    GENERATED_BODY()

public:
    UHasteSkill();

    /** 스킬 지속 시간(초) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Haste")
    float Duration = 20.f;

    /** 속도 배수 (1.2 = +20%) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Haste")
    float SpeedMultiplier = 1.2f;

protected:
    /** 스킬 실행 */
    virtual void Execute_Implementation(AEchoPlayer* Player) override;

private:
    /** 타이머 핸들 */
    FTimerHandle TimerHandle_EndHaste;

    /** 원래 이동 속도 저장 */
    float OriginalSpeed = 0.f;

    /** 스킬 소유자 캐시 */
    TWeakObjectPtr<AEchoPlayer> OwnerPlayer;

    /** 버프 종료 시 호출 */
    UFUNCTION()
    void EndHaste();
};
