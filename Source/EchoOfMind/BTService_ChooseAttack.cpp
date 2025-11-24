#include "BTService_ChooseAttack.h"
#include "EnemyBBKeys.h"
#include "AEnemyBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTService_ChooseAttack::UBTService_ChooseAttack()
{
    // 서비스 실행 주기 (초)
    Interval = 0.25f;
    // 랜덤 편차 (모든 AI가 동시에 평가하지 않도록 약간의 랜덤 딜레이 추가)
    RandomDeviation = 0.05f;
}

void UBTService_ChooseAttack::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    auto* BB = OwnerComp.GetBlackboardComponent();
    auto* C = OwnerComp.GetAIOwner();
    if (!BB || !C) return;

    // 현재 조종 중인 Pawn이 Enemy인지 확인
    AEnemyBase* E = Cast<AEnemyBase>(C->GetPawn());
    AActor* Target = Cast<AActor>(BB->GetValueAsObject(BBKEY_TargetActor));
    if (!E || !Target) return;

    // 타겟과의 거리 계산
    const float Dist = FVector::Dist(E->GetActorLocation(), Target->GetActorLocation());

    // 가장 적절한 공격 패턴 찾기
    int32 BestIdx = INDEX_NONE;
    float BestScore = FLT_MAX;

    const int32 Num = E->GetAttackPatternNum();
    for (int32 i = 0; i < Num; ++i)
    {
        FEnemyAttackPattern P;
        if (!E->GetAttackPatternAt(i, P)) continue;
        if (!E->CanUseAttackIndex(i))     continue; // 쿨다운 중이면 패스

        // Range와 현재 거리의 차이가 작은 공격일수록 점수가 높음
        const float Score = FMath::Abs(P.Range - Dist);
        if (Score < BestScore)
        {
            BestScore = Score;
            BestIdx = i;
        }
    }

    // 블랙보드에 선택된 공격 패턴 인덱스와 목표 사거리 기록
    if (BestIdx != INDEX_NONE)
    {
        FEnemyAttackPattern P;
        E->GetAttackPatternAt(BestIdx, P);
        BB->SetValueAsInt(BBKEY_AttackIndex, BestIdx);
        BB->SetValueAsFloat(BBKEY_DesiredRange, P.Range * 0.5f); // 살짝 가까운 위치를 목표로
    }
}
