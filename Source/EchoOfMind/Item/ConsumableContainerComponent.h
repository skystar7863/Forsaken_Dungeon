#pragma once
#include "CoreMinimal.h"
#include "ItemContainerComponent.h"   // 기본 컨테이너
#include "ConsumableContainerComponent.generated.h"

class UConsumableDefinition;

UCLASS(ClassGroup = (Inventory), meta = (BlueprintSpawnableComponent))
class ECHOOFMIND_API UConsumableContainerComponent : public UItemContainerComponent
{
    GENERATED_BODY()
public:
    // 오직 소모품만 허용
    virtual bool CanAcceptItem(const FItemInstance& Item, int32 Index) const override;

    // 스택 제한 (아이템별 MaxStack 사용)
    virtual int32 GetMaxStackForItem(const FItemInstance& Item) const override;

    // 사용(소비). 성공 시 1개 감소/삭제 및 변경 브로드캐스트
    UFUNCTION(BlueprintCallable, Category = "Consumable")
    bool ConsumeAt(int32 Index, class AEchoPlayer* Player);

protected:
    bool IsConsumable(const FItemInstance& Item) const;
};
