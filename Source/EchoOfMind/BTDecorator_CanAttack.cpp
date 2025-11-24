#include "BTDecorator_CanAttack.h"
#include "EnemyBBKeys.h"
#include "AEnemyBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTDecorator_CanAttack::UBTDecorator_CanAttack()
{
    // 에디터에서 노드 이름으로 보임
    NodeName = TEXT("Can Attack");
}

bool UBTDecorator_CanAttack::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    auto* BB = OwnerComp.GetBlackboardComponent();
    auto* C = OwnerComp.GetAIOwner();
    if (!BB || !C) return false;

    AEnemyBase* E = Cast<AEnemyBase>(C->GetPawn());
    AActor* Target = Cast<AActor>(BB->GetValueAsObject(BBKEY_TargetActor));
    const int32 Idx = BB->GetValueAsInt(BBKEY_AttackIndex);

    // 필수 요소가 없으면 실패
    if (!E || !Target) return false;
    if (!E->CanUseAttackIndex(Idx)) return false; // 쿨다운 체크

    // 공격 패턴 가져오기
    FEnemyAttackPattern P;
    if (!E->GetAttackPatternAt(Idx, P)) return false;

    // 거리 조건 충족 여부 검사
    const float Dist = FVector::Dist(E->GetActorLocation(), Target->GetActorLocation());
    const float Range = P.Range;
    return Dist <= Range * 1.05f; // 약간의 여유 허용
}
