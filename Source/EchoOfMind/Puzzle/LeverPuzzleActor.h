// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ButtonActor.h"
#include "LeverPuzzleActor.generated.h"

class ALeverActor;

UCLASS()
class ECHOOFMIND_API ALeverPuzzleActor : public AButtonActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALeverPuzzleActor();

	virtual void PressButton() override;

protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly, Category = "Puzzle")
	TArray<ALeverActor*> Levers;

	UPROPERTY(EditAnywhere, Category = "Puzzle")
	FString CorrectCombination = TEXT("00000");


private:	
	bool IsCombinationCorrect() const;

	void ResetLevers();

};
