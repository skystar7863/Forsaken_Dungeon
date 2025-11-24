#include "BTTask_BossMeleeAttack.h"
#include "BossEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AttributeComponent.h"
#include "Animation/AnimInstance.h"

EBTNodeResult::Type UBTTask_BossMeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    auto* C = OwnerComp.GetAIOwner();
    auto* Boss = Cast<ABossEnemy>(C->GetPawn());
    if (!Boss || Boss->bIsAttacking) return EBTNodeResult::Failed;

    Boss->bIsAttacking = true;

    UAnimInstance* Anim = Boss->GetMesh()->GetAnimInstance();
    if (!Anim || !Boss->MeleeMontage) return EBTNodeResult::Failed;

    Anim->Montage_Play(Boss->MeleeMontage);
    FTimerHandle DamageDelayHandle;
    Boss->GetWorldTimerManager().SetTimer(DamageDelayHandle, [Boss]()
        {
            if (!Boss || !Boss->Attributes) return;

            Boss->ApplyMeleeHit(200.f, 100.f, Boss->Attributes->AttackPower);

            if (Boss->Phase == 2)
            {
                // ✅ Phase 2일 때 넉백 효과 추가 가능
            }

        }, 0.6f, false); // ← 1초 후 실행

    // 0.5초 쿨타임 추가 (공격 직후 바로 재공격 방지)
    const float Cooldown = 1.5f;
    FTimerHandle AttackCooldownHandle;
    Boss->GetWorldTimerManager().SetTimer(AttackCooldownHandle, [Boss]()
        {
            Boss->bIsAttacking = false;
        }, Cooldown, false);

    return EBTNodeResult::Succeeded;
}
