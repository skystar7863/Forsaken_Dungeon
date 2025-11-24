#include "BTTask_PerformAttack.h"
#include "EnemyBBKeys.h"
#include "AEnemyBase.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "EnemyAttackPattern.h"
#include "AttributeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "TimerManager.h"

UBTTask_PerformAttack::UBTTask_PerformAttack()
{
    bCreateNodeInstance = true;   // ✅ 개별 인스턴스 상태
    bNotifyTick = true;           // ✅ 틱 감시
    NodeName = TEXT("Perform Attack");
}

EBTNodeResult::Type UBTTask_PerformAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    OwnerCompWeak = &OwnerComp;
    bFinished = false;
    UsedIndex = INDEX_NONE;
    SavedSpeed = -1.f;
    bSpeedLocked = false;
    CurrentMontage = nullptr;

    auto* C = OwnerComp.GetAIOwner();
    auto* BB = OwnerComp.GetBlackboardComponent();
    if (!C || !BB) return EBTNodeResult::Failed;

    AEnemyBase* E = Cast<AEnemyBase>(C->GetPawn());
    if (!E) return EBTNodeResult::Failed;

    UsedIndex = BB->GetValueAsInt(BBKEY_AttackIndex);

    FEnemyAttackPattern P;
    if (!E->GetAttackPatternAt(UsedIndex, P))
    {
        UE_LOG(LogTemp, Warning, TEXT("[PerformAttack] Invalid AttackIndex=%d"), UsedIndex);
        return EBTNodeResult::Failed;
    }
    if (!P.Montage)
    {
        UE_LOG(LogTemp, Warning, TEXT("[PerformAttack] Pattern '%s' has no Montage"), *P.Name.ToString());
        return EBTNodeResult::Failed;
    }

    UAnimInstance* AI = E->GetMesh() ? E->GetMesh()->GetAnimInstance() : nullptr;
    if (!AI)
    {
        UE_LOG(LogTemp, Warning, TEXT("[PerformAttack] No AnimInstance"));
        return EBTNodeResult::Failed;
    }
    AnimInst = AI;

    // Dynamic 멀티캐스트 → AddDynamic (UFUNCTION 필요)
    AnimInst->OnPlayMontageNotifyBegin.AddDynamic(this, &UBTTask_PerformAttack::OnNotifyBegin);
    AnimInst->OnMontageEnded.AddDynamic(this, &UBTTask_PerformAttack::OnMontageEnded);

    // 이동 잠금
    bSpeedLocked = false;
    SavedSpeed = -1.f;
    if (P.bRootMotionLock && E->GetCharacterMovement())
    {
        bSpeedLocked = true;
        SavedSpeed = E->GetCharacterMovement()->MaxWalkSpeed;
        E->GetCharacterMovement()->MaxWalkSpeed = 0.f;
    }

    // 몽타주 재생 (0 이하 = 실패)
    const float PlayLen = AnimInst->Montage_Play(P.Montage, 1.f, EMontagePlayReturnType::Duration, 0.f, true);
    if (PlayLen > 0.f)
    {
        CurrentMontage = P.Montage; // ✅ 이거 꼭 넣자
    }
    UE_LOG(LogTemp, Log, TEXT("[PerformAttack] Play '%s' (Idx=%d) len=%.2f"), *P.Name.ToString(), UsedIndex, PlayLen);


    //E->StartAttackCooldown(1.0f); // ✅ 1초 쿨타임 추가

    if (PlayLen <= 0.f)
    {
        RestoreSpeedIfLocked(E);
        Cleanup();
        return EBTNodeResult::Failed;
    }

    // 안전망 타이머: UObject 바인딩(Weak)
    if (UWorld* W = E->GetWorld())
    {
        FTimerDelegate Del;
        Del.BindUObject(this, &UBTTask_PerformAttack::OnForceEnd);
        W->GetTimerManager().SetTimer(EndTimerHandle, Del, PlayLen + 0.2f, false);
    }

    return EBTNodeResult::InProgress;
}

void UBTTask_PerformAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    // ✅ 감시: AnimInst나 몽타주가 끊겼으면 즉시 종료
    if (bFinished) return;

    if (!AnimInst || !CurrentMontage || !AnimInst->Montage_IsPlaying(CurrentMontage))
    {
        OnForceEnd();
    }
}

void UBTTask_PerformAttack::OnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload)
{
    auto* Comp = OwnerCompWeak.Get();
    if (!Comp) return;

    AAIController* C = Comp->GetAIOwner();
    if (!C) return;

    AEnemyBase* E = Cast<AEnemyBase>(C->GetPawn());
    if (!E) return;

    FEnemyAttackPattern P;
    if (!E->GetAttackPatternAt(UsedIndex, P)) return;

    if (NotifyName == P.HitNotifyName)
    {
        const float Damage = E->Attributes ? (E->Attributes->AttackPower * P.DamageMultiplier)
            : (20.f * P.DamageMultiplier);
        E->ApplyMeleeHit(P.Range, 100.f, Damage);
    }
}

void UBTTask_PerformAttack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    auto* Comp = OwnerCompWeak.Get();
    if (!Comp) { Cleanup(); return; }

    AAIController* C = Comp->GetAIOwner();
    AEnemyBase* E = C ? Cast<AEnemyBase>(C->GetPawn()) : nullptr;

    if (E)
    {
        FEnemyAttackPattern Dummy;
        if (E->GetAttackPatternAt(UsedIndex, Dummy))
        {
            E->MarkAttackUsed(UsedIndex);
        }


        RestoreSpeedIfLocked(E);

        if (UWorld* W = E->GetWorld())
        {
            W->GetTimerManager().ClearTimer(EndTimerHandle);
        }
    }

    Cleanup();
    // ? FinishLatentTask 호출 전에 반드시 OwnerComp 유효성 확인
    if (OwnerCompWeak.IsValid())
    {
        FinishLatentTask(*Comp, EBTNodeResult::Succeeded);
    }
}

EBTNodeResult::Type UBTTask_PerformAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    OwnerCompWeak = &OwnerComp; // 보수적 세팅

    AAIController* C = OwnerComp.GetAIOwner();
    if (AEnemyBase* E = C ? Cast<AEnemyBase>(C->GetPawn()) : nullptr)
    {
        RestoreSpeedIfLocked(E);
        if (UWorld* W = E->GetWorld())
        {
            W->GetTimerManager().ClearTimer(EndTimerHandle);
        }
    }
    Cleanup();
    return EBTNodeResult::Aborted;
}

void UBTTask_PerformAttack::OnForceEnd()
{
    // ✅ OwnerComp가 살아 있든 말든 무조건 Cleanup 호출
    Cleanup();
}

void UBTTask_PerformAttack::RestoreSpeedIfLocked(AEnemyBase* Enemy)
{
    if (bSpeedLocked && Enemy && Enemy->GetCharacterMovement() && SavedSpeed >= 0.f)
    {
        Enemy->GetCharacterMovement()->MaxWalkSpeed = SavedSpeed;
    }
}

void UBTTask_PerformAttack::Cleanup()
{
    // 델리게이트 정리
    if (AnimInst)
    {
        AnimInst->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UBTTask_PerformAttack::OnNotifyBegin);
        AnimInst->OnMontageEnded.RemoveDynamic(this, &UBTTask_PerformAttack::OnMontageEnded);
        AnimInst = nullptr;
    }

    UsedIndex = INDEX_NONE;
    SavedSpeed = -1.f;
    bSpeedLocked = false;

    // ✅ 여기 추가!
    // OwnerComp가 여전히 살아있다면 안전하게 태스크 종료
    if (OwnerCompWeak.IsValid())
    {
        UBehaviorTreeComponent* Comp = OwnerCompWeak.Get();
        if (Comp)
        {
            FinishLatentTask(*Comp, EBTNodeResult::Succeeded);
        }
    }
}


void UBTTask_PerformAttack::FinishIfPossible()
{
    if (bFinished) return;
    bFinished = true;

    if (OwnerCompWeak.IsValid())
    {
        FinishLatentTask(*OwnerCompWeak.Get(), EBTNodeResult::Succeeded);
    }
}