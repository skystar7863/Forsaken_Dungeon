#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AEnemyAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Damage;

struct FAIStimulus;

UCLASS()
class ECHOOFMIND_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

protected:
	// === BT/BB ===
	UPROPERTY(Transient) UBehaviorTreeComponent* BTComp;
	UPROPERTY(Transient) UBlackboardComponent* BBComp;

	// === Perception (컨트롤러가 소유) ===
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* Perception;

	UPROPERTY() UAISenseConfig_Sight* SightConfig;
	UPROPERTY() UAISenseConfig_Damage* DamageConfig;

	// 감지 콜백
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	virtual void OnPossess(APawn* InPawn) override;

public:
	// BT/BB 핸들
	UBlackboardComponent* GetBB() const { return BBComp; }

	// ?? Perception 접근용(override 아님)
	UFUNCTION(BlueprintCallable, Category = "AI")
	FORCEINLINE UAIPerceptionComponent* GetPerceptionComp() const { return Perception; }
};
