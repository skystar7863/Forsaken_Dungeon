#include "AEnemyAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISense_Damage.h"

#include "AEnemyBase.h"

AEnemyAIController::AEnemyAIController()
{
	// === BT/BB ===
	BTComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BTComp"));
	BBComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BBComp"));

	// === Perception ===
	Perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));

	if (SightConfig)
	{
		SightConfig->SightRadius = 1200.f;
		SightConfig->LoseSightRadius = 1500.f;
		SightConfig->PeripheralVisionAngleDegrees = 180.f;
		SightConfig->SetMaxAge(2.f);
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

		Perception->ConfigureSense(*SightConfig);
		Perception->SetDominantSense(SightConfig->GetSenseImplementation());
	}
	if (DamageConfig)
	{
		Perception->ConfigureSense(*DamageConfig); // 데미지 센스 등록
	}

	Perception->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (AEnemyBase* E = Cast<AEnemyBase>(InPawn))
	{
		// BB/BT 구동
		if (UBlackboardData* BBData = E->GetBlackboardAsset())
		{
			UseBlackboard(BBData, BBComp);
		}
		if (UBehaviorTree* BT = E->GetBehaviorTreeAsset())
		{
			RunBehaviorTree(BT);
		}

		if (Perception)
		{
			Perception->RequestStimuliListenerUpdate();
		}
	}
}

void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		if (BBComp)
		{
			static const FName BBKEY_TargetActor(TEXT("TargetActor"));
			BBComp->SetValueAsObject(BBKEY_TargetActor, Actor);
		}

		if (AEnemyBase* E = Cast<AEnemyBase>(GetPawn()))
		{
			E->OnTargetPerceptionUpdated(Actor, Stimulus);
		}
	}
}
