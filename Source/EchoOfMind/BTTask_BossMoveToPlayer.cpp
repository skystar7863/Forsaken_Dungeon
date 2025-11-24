#include "BTTask_BossMoveToPlayer.h"
#include "AIController.h"
#include "BossEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_BossMoveToPlayer::UBTTask_BossMoveToPlayer()
{
    NodeName = TEXT("Boss Move To Player");
}

EBTNodeResult::Type UBTTask_BossMoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AICon = OwnerComp.GetAIOwner();
    if (!AICon) return EBTNodeResult::Failed;

    ABossEnemy* Boss = Cast<ABossEnemy>(AICon->GetPawn());
    if (!Boss) return EBTNodeResult::Failed;

    AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")));
    if (!Target) return EBTNodeResult::Failed;

    // 공격 중일 땐 멈춤 (근접/원거리 모두)
    if (Boss->bIsAttacking)
    {
        Boss->GetCharacterMovement()->StopMovementImmediately();
        return EBTNodeResult::Succeeded;
    }

    // 목표 방향으로 회전
    const FVector BossLoc = Boss->GetActorLocation();
    const FVector TargetLoc = Target->GetActorLocation();
    const FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(BossLoc, TargetLoc);
    Boss->SetActorRotation(FRotator(0.f, LookRot.Yaw, 0.f));

    // 일정 거리보다 멀면 이동
    const float Dist = FVector::Dist(BossLoc, TargetLoc);
    if (Dist > AcceptableRadius)
    {
        // MoveTo 사용 → 내비메시 따라감
        FAIMoveRequest MoveReq;
        MoveReq.SetGoalActor(Target);
        MoveReq.SetAcceptanceRadius(AcceptableRadius);
        UAIBlueprintHelperLibrary::SimpleMoveToActor(AICon, Target);
    }
    else
    {
        // 가까우면 멈춤
        AICon->StopMovement();
    }

    return EBTNodeResult::Succeeded;
}
