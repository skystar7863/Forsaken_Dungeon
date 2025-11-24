// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PedestalActor.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class APickupItem;

UCLASS()
class ECHOOFMIND_API APedestalActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APedestalActor();

	UFUNCTION()
	void Interact(class AEchoPlayer* Player);

	UPROPERTY(BlueprintReadOnly, Category = "Pedestal")
	APickupItem* PlacedItem = nullptr;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* ItemAttachPoint;

public:	
	

};
