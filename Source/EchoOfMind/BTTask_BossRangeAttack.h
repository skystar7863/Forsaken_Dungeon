#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_BossRangeAttack.generated.h"

/**
 *  보스 원거리 공격 태스크
 *  - 플레이어가 일정 거리 이상일 때 실행
 *  - 0.5초 후 발 밑에 경고 이펙트 (나이아가라)
 *  - 0.5초 뒤 폭풍 이펙트 (케스케이드)
 *  - 폭풍 2초 유지, 0.5초마다 데미지 틱
 *  - Phase2에서는 쿨타임 단축
 */
UCLASS()
class ECHOOFMIND_API UBTTask_BossRangeAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	//UBTTask_BossRangeAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	/** 페이즈별 쿨타임 (초 단위) */
	UPROPERTY(EditAnywhere, Category = "Attack")
	float Cooldown_Phase1 = 4.0f;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float Cooldown_Phase2 = 3.0f;

	/** 시전 준비 시간 */
	UPROPERTY(EditAnywhere, Category = "Attack")
	float CastDelay = 0.5f;
};
