// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridPuzzleActor.generated.h"

class ATileActor;

UCLASS()
class ECHOOFMIND_API AGridPuzzleActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridPuzzleActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Grid")
	TSubclassOf<ATileActor> TileClass;
	UPROPERTY(EditAnywhere, Category = "Grid")
	int32 GridSize = 6;
	UPROPERTY(EditAnywhere, Category = "Grid")
	float TileSpacing = 110.0f;
	UPROPERTY(EditAnywhere, Category = "Grid")
	TArray<int32> SafeTileIndices;

	UPROPERTY()
	TArray<ATileActor*> Tiles;
	UPROPERTY(EditAnywhere, Category = "Grid")
	FVector PlayerStartLocation;
	UPROPERTY(EditAnywhere, Category = "Grid")
	FRotator PlayerStartRotation;
	UFUNCTION()
	void HandleTileStepped(ATileActor* Tile);

	void FailAndReset();

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

};
