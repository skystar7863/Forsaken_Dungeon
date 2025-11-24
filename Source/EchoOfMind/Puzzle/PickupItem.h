// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupItem.generated.h"

class UStaticMeshComponent;
//class UBoxComponent;
class UTexture2D;

UCLASS()
class ECHOOFMIND_API APickupItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupItem();

	void OnPickedUp();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Pickup")
	UTexture2D* Icon;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*UPROPERTY(VisibleAnywhere)
	UBoxComponent* Trigger;*/
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	/*UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);*/

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

};
