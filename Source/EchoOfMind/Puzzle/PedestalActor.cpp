// Fill out your copyright notice in the Description page of Project Settings.


#include "PedestalActor.h"
#include "Echoplayer.h"
#include "PickupItem.h"
#include "EchoPlayerController.h"
#include "InventoryWidget.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"

// Sets default values
APedestalActor::APedestalActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;

	ItemAttachPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ItemAttachPoint"));
	ItemAttachPoint->SetupAttachment(RootComponent);

}

void APedestalActor::BeginPlay()
{
	Super::BeginPlay();
}


void APedestalActor::Interact(AEchoPlayer* Player)
{
	if (!Player) return;

	if (!PlacedItem && Player->HeldItem) {
		PlacedItem = Player->HeldItem;
		Player->HeldItem = nullptr;

		PlacedItem->SetActorHiddenInGame(false);
		PlacedItem->SetActorEnableCollision(false);
		PlacedItem->AttachToComponent(ItemAttachPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		if (Player->GetPlayerController()) {
			Player->GetPlayerController()->GetInventoryWidget()->SetItemIcon(nullptr);
		}
	}
	else if (PlacedItem && !Player->HeldItem) {
		PlacedItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		PlacedItem->OnPickedUp();
		Player->HeldItem = PlacedItem;
		PlacedItem = nullptr;

		if (Player->GetPlayerController()) {
			Player->GetPlayerController()->GetInventoryWidget()->SetItemIcon(Player->HeldItem->Icon);
		}
	}
}


