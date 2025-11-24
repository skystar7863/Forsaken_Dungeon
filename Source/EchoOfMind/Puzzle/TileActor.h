// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileActor.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class UGridPuzzleActor;

UCLASS()
class ECHOOFMIND_API ATileActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATileActor();

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Tile")
	bool bIsSafe = false;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTileSteppedSignature, ATileActor*, Tile);

	UPROPERTY(BlueprintAssignable, Category = "Tile")
	FTileSteppedSignature OnTileStepped;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* Trigger;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

};
