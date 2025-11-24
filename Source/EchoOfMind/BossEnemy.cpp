#include "BossEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/OverlapResult.h"
#include "Particles/ParticleSystemComponent.h"
#include "AttributeComponent.h"
#include "EchoPlayer.h"
#include "TimerManager.h"

ABossEnemy::ABossEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABossEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (Attributes)
	{
		Attributes->OnHealthChanged.AddDynamic(this, &ABossEnemy::OnHealthChanged);
	}
}

void ABossEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}


void ABossEnemy::ApplyStormDamage(FVector Center, float Radius, float DamagePerTick)
{
	TArray<FOverlapResult> Hits;
	FCollisionShape Shape = FCollisionShape::MakeSphere(Radius);
	bool bHit = GetWorld()->OverlapMultiByChannel(Hits, Center, FQuat::Identity, ECC_Pawn, Shape);

	for (auto& H : Hits)
	{
		if (AEchoPlayer* Player = Cast<AEchoPlayer>(H.GetActor()))
		{
			UGameplayStatics::ApplyDamage(Player, DamagePerTick, GetController(), this, nullptr);
		}
	}
}

void ABossEnemy::OnHealthChanged(float NewHealth)
{
	if (Phase == 1 && Attributes && NewHealth <= Attributes->MaxHealth * 0.5f)
	{
		Phase = 2;
		UE_LOG(LogTemp, Log, TEXT("Boss entered Phase 2!"));
	}
}
