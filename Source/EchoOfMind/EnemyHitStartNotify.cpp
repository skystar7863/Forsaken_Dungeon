#include "EnemyHitStartNotify.h"
#include "AEnemyBase.h"
#include "Components/SkeletalMeshComponent.h"

void UEnemyHitStartNotify::Notify(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	// 3파라미터 버전 → 2파라미터 버전으로 위임(엔진 호환)
	Notify(MeshComp, Animation);
}

void UEnemyHitStartNotify::Notify(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	if (AEnemyBase* Enemy = Cast<AEnemyBase>(MeshComp->GetOwner()))
	{
		Enemy->AnimNotify_EnemyHitStart(); // 무기면 히트박스 ON, 맨손이면 스윕 1회
	}
}
