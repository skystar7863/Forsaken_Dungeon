#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTDecorator_BossCanAttack.generated.h"

/**
 * 보스가 공격 가능한 상태인지 판정하는 데코레이터
 * - bIsAttacking == false
 * - 대상과의 거리 조건 만족 시 true 반환
 */
UCLASS()
class ECHOOFMIND_API UBTDecorator_BossCanAttack : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTDecorator_BossCanAttack();

protected:
	/** true일 때 근접공격 거리, false일 때 원거리 공격 거리 */
	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	bool bIsMeleeCheck = true;

	/** 근접 사정거리 */
	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	float MeleeRange = 200.f;

	/** 원거리 최소 사정거리 */
	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	float RangeMinDistance = 200.f;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
