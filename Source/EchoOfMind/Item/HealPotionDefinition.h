#pragma once
#include "CoreMinimal.h"
#include "ConsumableDefinition.h"
#include "HealPotionDefinition.generated.h"

/** 체력 회복 포션 정의 */
UCLASS(BlueprintType)
class ECHOOFMIND_API UHealPotionDefinition : public UConsumableDefinition
{
    GENERATED_BODY()
public:
    /** 총 회복량 (예: 50) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealPotion")
    float HealTotal = 50.f;

    /** 회복 지속시간 (예: 5초) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealPotion")
    float HealDuration = 5.f;

    /** 포션 사용 */
    virtual bool Consume(class AEchoPlayer* Player) const override;
};
