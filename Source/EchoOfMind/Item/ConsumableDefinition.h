#pragma once
#include "CoreMinimal.h"
#include "Item/ItemDefinition.h"
#include "ConsumableDefinition.generated.h"

/** 모든 소모품의 공통 베이스 */
UCLASS(Abstract, BlueprintType)
class ECHOOFMIND_API UConsumableDefinition : public UItemDefinition
{
    GENERATED_BODY()
public:
    /** 퀵바/컨테이너에서 소모품으로 분류 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable")
    bool bIsConsumable = true;

    /** 기본 스택 상한(파생에서 재정의 가능) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable")
    int32 MaxStack = 5;

    /** 실제 사용(소모) – 파생에서 구현 */
    UFUNCTION(BlueprintCallable, Category = "Consumable")
    virtual bool Consume(class AEchoPlayer* Player) const { return false; }
};
