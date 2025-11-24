#pragma once
#include "BehaviorTree/BTService.h"
#include "BTService_FindTarget.generated.h"

UCLASS()
class ECHOOFMIND_API UBTService_FindTarget : public UBTService
{
    GENERATED_BODY()
public:
    UBTService_FindTarget();

protected:
    virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override; // ¡Ú Ãß°¡
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
