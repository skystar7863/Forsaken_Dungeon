#pragma once
#include "BehaviorTree/BTService.h"
#include "BTService_ChooseAttack.generated.h"

UCLASS()
class ECHOOFMIND_API UBTService_ChooseAttack : public UBTService
{
    GENERATED_BODY()
public:
    UBTService_ChooseAttack();
protected:
    // 서비스는 일정 주기마다 실행됨
    // 여기서 매번 "어떤 공격 패턴을 쓸지" 결정해서 블랙보드에 기록
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
