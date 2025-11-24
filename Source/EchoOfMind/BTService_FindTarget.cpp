#include "BTService_FindTarget.h"
#include "EnemyBBKeys.h"
#include "AEnemyAIController.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Damage.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"

#include "AttributeComponent.h"
#include "EchoPlayer.h"               // 플레이어 클래스

UBTService_FindTarget::UBTService_FindTarget()
{
    bNotifyBecomeRelevant = true;
    bNotifyTick = true;
    Interval = 0.30f;
    RandomDeviation = 0.05f;
}

static bool IsAliveActor(AActor* A)
{
    if (!A) return false;
    if (const UAttributeComponent* Attr = A->FindComponentByClass<UAttributeComponent>())
    {
        return Attr->Health > 0.f;
    }
    return true; // 능력치가 없으면 일단 살아있다고 간주
}

// 🔹 플레이어만 유효 타깃으로 인정
static bool IsValidPlayerTarget(AActor* A)
{
    if (!A) return false;

    // 우리 프로젝트의 플레이어 Pawn
    if (Cast<AEchoPlayer>(A)) return true;

    // (보조) 혹시 다른 Pawn이지만 PlayerController가 조종 중인 경우
    if (const APawn* P = Cast<APawn>(A))
    {
        return P->IsPlayerControlled();
    }
    return false;
}

void UBTService_FindTarget::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // ★ 서비스가 켜지는 순간, 현재 타깃이 적/무효면 즉시 클리어
    if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
    {
        if (AActor* Cur = Cast<AActor>(BB->GetValueAsObject(BBKEY_TargetActor)))
        {
            if (!IsValidPlayerTarget(Cur))
            {
                BB->ClearValue(BBKEY_TargetActor);
            }
        }
    }
    // ★ 첫 프레임 딜레이 없이 즉시 한 번 스캔
    TickNode(OwnerComp, NodeMemory, 0.f);
}

void UBTService_FindTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* /*NodeMemory*/, float /*DeltaSeconds*/)
{
    //check(false);
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    AEnemyAIController* AC = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
    if (!BB || !AC) return;

    // 컨트롤러의 Perception (override 아님)
    UAIPerceptionComponent* PC = AC->GetPerceptionComp();
    if (!PC) return;

    // 후보 수집: 시야(현재 인지) + 데미지(최근 인지) 중 "플레이어"만
    TSet<AActor*> Candidates;

    {
        TArray<AActor*> SeenSight;
        PC->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), SeenSight);
        for (AActor* A : SeenSight)
        {
            if (IsAliveActor(A) && IsValidPlayerTarget(A))
            {
                Candidates.Add(A);
            }
        }
    }
    {
        TArray<AActor*> KnownDamage;
        PC->GetKnownPerceivedActors(UAISense_Damage::StaticClass(), KnownDamage);
        for (AActor* A : KnownDamage)
        {
            if (IsAliveActor(A) && IsValidPlayerTarget(A))
            {
                Candidates.Add(A);
            }
        }
    }

    // 가장 가까운 플레이어 선택 (없으면 nullptr로 클리어)
    const APawn* Pawn = AC->GetPawn();
    const FVector MyLoc = Pawn ? Pawn->GetActorLocation() : FVector::ZeroVector;

    float BestDistSq = TNumericLimits<float>::Max();
    AActor* Best = nullptr;

    for (AActor* A : Candidates)
    {
        const float D = FVector::DistSquared(MyLoc, A->GetActorLocation());
        if (D < BestDistSq)
        {
            BestDistSq = D;
            Best = A;
        }
    }

    BB->SetValueAsObject(BBKEY_TargetActor, Best); // Best가 없으면 nullptr로 설정되어 기존 타깃 해제
}
