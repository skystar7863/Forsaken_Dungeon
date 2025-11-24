// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemPlacementPuzzleActor.h"
#include "PedestalActor.h"
#include "PickupItem.h"
#include "DoorActor.h"

AItemPlacementPuzzleActor::AItemPlacementPuzzleActor() {}

void AItemPlacementPuzzleActor::PressButton()
{
	if (IsCorrect()) {
		for (ADoorActor* Door : ControlledDoors) {
			if (Door) Door->OpenDoor();
		}
	}
	else {
		ResetPedestals();
	}
}

bool AItemPlacementPuzzleActor::IsCorrect() const
{
	if(Pedestals.Num() != CorrectClasses.Num())
	return false;

	for (int32 i = 0; i < Pedestals.Num(); ++i) {
		APedestalActor* Ped = Pedestals[i];
		if (!Ped || !Ped->PlacedItem)
			return false;

		if (!Ped || !Ped->PlacedItem)
			return false;

		if (!Ped->PlacedItem->IsA(CorrectClasses[i]))
			return false;
	}
	return true;
}

void AItemPlacementPuzzleActor::ResetPedestals()
{
	for (APedestalActor* Ped : Pedestals) {
		if (Ped && Ped->PlacedItem) {
			Ped->PlacedItem->Destroy();
			Ped->PlacedItem=nullptr;
		}
	}

}
