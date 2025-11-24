#pragma once
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTDecorator_CanAttack.generated.h"

UCLASS()
class ECHOOFMIND_API UBTDecorator_CanAttack : public UBTDecorator_BlackboardBase
{
    GENERATED_BODY()
public:
    UBTDecorator_CanAttack();

protected:
    // 조건 검사: 현재 공격 패턴을 사용할 수 있는가?
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
