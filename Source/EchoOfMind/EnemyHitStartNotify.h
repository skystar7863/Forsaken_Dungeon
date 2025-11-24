#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "EnemyHitStartNotify.generated.h"

/** 무기 히트 윈도우 시작(또는 맨손 스윕 1회) */
UCLASS(meta = (DisplayName = "Enemy Hit Start"))
class ECHOOFMIND_API UEnemyHitStartNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	// UE5.1+ 권장 시그니처
	virtual void Notify(USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

	// 구버전/호환 시그니처 (엔진에 따라 여전히 호출됨)
	virtual void Notify(USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation) override;
};
