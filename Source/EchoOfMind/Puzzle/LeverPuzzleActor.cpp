// Fill out your copyright notice in the Description page of Project Settings.


#include "LeverPuzzleActor.h"
#include "LeverActor.h"
#include "DoorActor.h"

// Sets default values
ALeverPuzzleActor::ALeverPuzzleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ALeverPuzzleActor::PressButton()
{
	if (IsCombinationCorrect())
	{
		for (ADoorActor* Door : ControlledDoors)
		{
			if (Door)
			{
				Door->OpenDoor();
			}
		}
	}
	else
	{
		ResetLevers();
	}
}

bool ALeverPuzzleActor::IsCombinationCorrect() const
{
	if (Levers.Num() != CorrectCombination.Len())
	{
		return false;
	}
	for (int32 i = 0; i < Levers.Num(); ++i)
	{
		const ALeverActor* Lever = Levers[i];
		bool bOn = Lever && Lever->bIsOn;
		TCHAR ExpectedChar = CorrectCombination[i];
		if ((bOn && ExpectedChar != '1') ||
			(!bOn && ExpectedChar != '0'))
		{
			return false;
		}
	}
	return true;
}

void ALeverPuzzleActor::ResetLevers()
{
	for (ALeverActor* Lever : Levers)
	{
		if (Lever && Lever->bIsOn)
		{
			Lever->ToggleLever();
		}
	}
}



// Called every frame
//void ALeverPuzzleActor::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

