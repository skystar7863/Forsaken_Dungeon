#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PerformAttack.generated.h"

class UAnimInstance;
class UAnimMontage;

UCLASS()
class ECHOOFMIND_API UBTTask_PerformAttack : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_PerformAttack();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
    virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
    // ? 파괴될 수 있는 BT 컴포넌트는 Weak 포인터로 보관
    TWeakObjectPtr<UBehaviorTreeComponent> OwnerCompWeak;

    UPROPERTY() UAnimInstance* AnimInst = nullptr;
    UPROPERTY() UAnimMontage* CurrentMontage = nullptr;



    FTimerHandle EndTimerHandle;

    int32 UsedIndex = INDEX_NONE;
    float SavedSpeed = -1.f;
    bool  bSpeedLocked = false;
    bool   bFinished = false;   // ✅ 종료 한번만

    // AnimInstance의 Dynamic 델리게이트 시그니처에 맞춰 UFUNCTION 필요
    UFUNCTION()
    void OnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload);

    UFUNCTION()
    void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    // 타임아웃 시 안전하게 종료
    UFUNCTION()
    void OnForceEnd();

    void Cleanup();
    void RestoreSpeedIfLocked(class AEnemyBase* Enemy);

    void FinishIfPossible();      // ✅ 안전 종료 래퍼
};
