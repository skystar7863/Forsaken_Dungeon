// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ButtonActor.h"
#include "ItemPlacementPuzzleActor.generated.h"

class APedestalActor;
class APickupItem;

UCLASS()
class ECHOOFMIND_API AItemPlacementPuzzleActor : public AButtonActor
{
	GENERATED_BODY()

public:
	AItemPlacementPuzzleActor();

	virtual void PressButton() override;

protected:
	UPROPERTY(EditInstanceOnly, Category = "Puzzle")
	TArray<APedestalActor*> Pedestals;

	UPROPERTY(EditAnywhere, Category = "Puzzle")
	TArray<TSubclassOf<APickupItem>> CorrectClasses;

private:
	bool IsCorrect() const;
	void ResetPedestals();
	
	
};
