#include "BTTask_BossRangeAttack.h"
#include "BossEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NiagaraFunctionLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"

EBTNodeResult::Type UBTTask_BossRangeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	ABossEnemy* Boss = Cast<ABossEnemy>(AICon->GetPawn());
	if (!Boss || Boss->bIsAttacking) return EBTNodeResult::Failed;

	AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")));
	if (!Target) return EBTNodeResult::Failed;

	const float Now = Boss->GetWorld()->GetTimeSeconds();
	const float Cooldown = (Boss->GetPhase() == 2) ? Boss->RangeCooldown_Phase2 : Boss->RangeCooldown_Phase1;
	if (Now - Boss->LastRangeAttackTime < Cooldown) return EBTNodeResult::Failed;

	Boss->bIsAttacking = true;
	Boss->LastRangeAttackTime = Now;

	// ✅ 1. 보스 몽타주 실행
	if (Boss->RangeMontage)
	{
		if (UAnimInstance* Anim = Boss->GetMesh()->GetAnimInstance())
		{
			Anim->Montage_Play(Boss->RangeMontage);
		}
	}

	// ✅ 2. 경고 이펙트 (플레이어 발 밑, 90 아래)
	FVector TargetLoc = Target->GetActorLocation();
	TargetLoc.Z -= 90.f; // 높이 보정

	if (Boss->WarningCircleFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(Boss->GetWorld(), Boss->WarningCircleFX, TargetLoc);
	}

	// ✅ 3. 0.5초 후 스톰 시작
	FTimerHandle StormHandle;
	Boss->GetWorldTimerManager().SetTimer(StormHandle, [Boss, TargetLoc]()
		{
			if (Boss->StormFX)
			{
				UParticleSystemComponent* Storm = UGameplayStatics::SpawnEmitterAtLocation(
					Boss->GetWorld(), Boss->StormFX, TargetLoc, FRotator::ZeroRotator, FVector(1.f));

				if (Storm)
				{
					const float Interval = Boss->StormTickInterval;  // 0.5초
					const float Duration = Boss->StormDuration;       // 2초
					const int32 TickCount = FMath::FloorToInt(Duration / Interval) + 3; // ✅ 5틱 보장

					for (int32 i = 0; i < TickCount; ++i)
					{
						const float Delay = i * Interval;
						FTimerHandle TickHandle;
						Boss->GetWorldTimerManager().SetTimer(TickHandle, [Boss, TargetLoc]()
							{
								Boss->ApplyStormDamage(TargetLoc, Boss->StormRadius, Boss->StormDamagePerTick);
							}, Delay, false);
					}

					// 스톰 종료 (비활성화)
					FTimerHandle EndHandle;
					Boss->GetWorldTimerManager().SetTimer(EndHandle, [Storm]()
						{
							if (Storm) Storm->Deactivate();
						}, Duration, false);
				}
			}

			// 공격 종료 플래그 해제
			Boss->bIsAttacking = false;

		}, 0.5f, false); // 0.5초 시전 딜레이

	return EBTNodeResult::Succeeded;
}
