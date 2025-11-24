// Fill out your copyright notice in the Description page of Project Settings.


#include "ButtonActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "DoorActor.h"

// Sets default values
AButtonActor::AButtonActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	RootComponent = Collision;

	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	ButtonMesh->SetupAttachment(RootComponent);

	//ControlledDoor = nullptr;
}

void AButtonActor::PressButton()
{
	for (ADoorActor* Door : ControlledDoors)
	{
		if (Door)
		{
			Door->OpenDoor();
		}
	}
}

// Called when the game starts or when spawned
//void AButtonActor::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}
//
//// Called every frame
//void AButtonActor::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

