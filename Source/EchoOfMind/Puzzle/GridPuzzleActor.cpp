// Fill out your copyright notice in the Description page of Project Settings.


#include "GridPuzzleActor.h"
#include "TileActor.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGridPuzzleActor::AGridPuzzleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AGridPuzzleActor::BeginPlay()
{
	Super::BeginPlay();
	Tiles.Reserve(GridSize * GridSize);
	for (int32 Y = 0; Y < GridSize; Y++) {
		for (int X = 0; X < GridSize; X++) {
			FVector Loc = GetActorLocation() + FVector(X * TileSpacing, Y * TileSpacing, 0.0f);
			ATileActor* Tile = GetWorld()->SpawnActor<ATileActor>(TileClass, Loc, FRotator::ZeroRotator);
			Tiles.Add(Tile);
			Tile->OnTileStepped.AddDynamic(this, &AGridPuzzleActor::HandleTileStepped);
		}
	}

	for (int32 i = 0; i < Tiles.Num(); ++i) {
		Tiles[i]->bIsSafe = SafeTileIndices.Contains(i);
	}

	/*if (ACharacter* PC = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) {
		PlayerStartLocation = PC->GetActorLocation();
		PlayerStartRotation = PC->GetActorRotation();
	}*/
}

void AGridPuzzleActor::HandleTileStepped(ATileActor* Tile)
{
	if (!Tile) return;
	if (Tile->bIsSafe) {
		return;
	}
	FailAndReset();
}

void AGridPuzzleActor::FailAndReset()
{
	
	if (ACharacter* PC = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) {
		PC->SetActorLocation(PlayerStartLocation);
		PC->SetActorRotation(PlayerStartRotation);
	}
	for (int32 i = 0; i < Tiles.Num(); ++i) {
		Tiles[i]->bIsSafe = SafeTileIndices.Contains(i);
	}

}

// Called every frame
//void AGridPuzzleActor::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

