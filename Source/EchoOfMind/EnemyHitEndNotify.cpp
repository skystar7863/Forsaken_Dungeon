#include "EnemyHitEndNotify.h"
#include "AEnemyBase.h"
#include "Components/SkeletalMeshComponent.h"

void UEnemyHitEndNotify::Notify(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	// 3파라미터 버전 → 2파라미터 버전으로 위임
	Notify(MeshComp, Animation);
}

void UEnemyHitEndNotify::Notify(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	if (AEnemyBase* Enemy = Cast<AEnemyBase>(MeshComp->GetOwner()))
	{
		Enemy->AnimNotify_EnemyHitEnd(); // 무기 히트박스 OFF
	}
}
