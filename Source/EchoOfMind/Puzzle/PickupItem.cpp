// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupItem.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EchoPlayer.h"

// Sets default values
APickupItem::APickupItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	//RootComponent = Trigger;
	//Trigger->SetCollisionProfileName(TEXT("Trigger"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

}


// Called when the game starts or when spawned
void APickupItem::BeginPlay()
{
	Super::BeginPlay();
	//Trigger->OnComponentBeginOverlap.AddDynamic(this, &APickupItem::OnOverlapBegin);
	
}

//void APickupItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if (AEchoPlayer* Player = Cast<AEchoPlayer>(OtherActor)) {
//		Player->PickupItem(this);
//	}
//}

void APickupItem::OnPickedUp()
{
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
}


// Called every frame
//void APickupItem::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

