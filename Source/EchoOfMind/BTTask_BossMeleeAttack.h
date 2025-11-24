#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_BossMeleeAttack.generated.h"

/**
 *  보스 근접 공격 태스크
 *  - 가까운 거리에서 몽타주 실행
 *  - Phase2에서는 넉백 추가
 *  - 쿨타임 중엔 실행 안 됨
 */
UCLASS()
class ECHOOFMIND_API UBTTask_BossMeleeAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	//UBTTask_BossMeleeAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	/** 공격 쿨타임 */
	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackCooldown = 3.0f;

	/** 넉백 세기 (Phase2 전용) */
	UPROPERTY(EditAnywhere, Category = "Attack")
	float KnockbackForce = 800.0f;
};
