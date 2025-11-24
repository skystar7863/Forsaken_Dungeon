#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_BossMoveToPlayer.generated.h"

/**
 * 보스가 플레이어 쪽으로 이동하는 간단한 태스크
 * - 공격 중엔 이동 안 함
 * - 근접 사거리까지 접근
 */
UCLASS()
class ECHOOFMIND_API UBTTask_BossMoveToPlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_BossMoveToPlayer();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Boss|Move")
	float AcceptableRadius = 200.f;

	UPROPERTY(EditAnywhere, Category = "Boss|Move")
	float MoveSpeed = 250.f;
};
