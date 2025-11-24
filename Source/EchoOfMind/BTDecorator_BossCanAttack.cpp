#include "BTDecorator_BossCanAttack.h"
#include "AIController.h"
#include "BossEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"

UBTDecorator_BossCanAttack::UBTDecorator_BossCanAttack()
{
	NodeName = TEXT("Boss Can Attack");
}

bool UBTDecorator_BossCanAttack::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return false;

	ABossEnemy* Boss = Cast<ABossEnemy>(AICon->GetPawn());
	if (!Boss || Boss->bIsAttacking) return false;

	AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")));
	if (!Target) return false;

	const float Distance = FVector::Dist(Boss->GetActorLocation(), Target->GetActorLocation());

	if (bIsMeleeCheck)
	{
		// 근접공격은 일정 거리 이내
		return Distance <= MeleeRange;
	}
	else
	{
		// 원거리 공격은 일정 거리 밖
		return Distance >= RangeMinDistance;
	}
}
