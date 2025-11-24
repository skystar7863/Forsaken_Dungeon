#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "EnemyHitEndNotify.generated.h"

/** 무기 히트 윈도우 종료 */
UCLASS(meta = (DisplayName = "Enemy Hit End"))
class ECHOOFMIND_API UEnemyHitEndNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	// UE5.1+ 권장 시그니처
	virtual void Notify(USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

	// 구버전/호환 시그니처
	virtual void Notify(USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation) override;
};
