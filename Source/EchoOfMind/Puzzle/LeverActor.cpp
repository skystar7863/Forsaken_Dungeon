// Fill out your copyright notice in the Description page of Project Settings.


#include "LeverActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ALeverActor::ALeverActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	RootComponent = Collision;

	LeverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeverMesh"));
	LeverMesh->SetupAttachment(RootComponent);

}


// Called when the game starts or when spawned
void ALeverActor::BeginPlay()
{
	Super::BeginPlay();
	UpRotation = LeverMesh->GetRelativeRotation();
	DownRotation = UpRotation + FRotator(180.f, 0.f, 0.f);
	
}

void ALeverActor::ToggleLever()
{
	bIsOn = !bIsOn;
	LeverMesh->SetRelativeRotation(bIsOn ? DownRotation : UpRotation);
	OnLeverStateChanged.Broadcast(this, bIsOn);
}


//// Called every frame
//void ALeverActor::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}