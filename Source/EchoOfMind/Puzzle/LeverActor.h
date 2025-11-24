#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LeverActor.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLeverStateChanged, ALeverActor*, Lever, bool, bIsOn);

UCLASS()
class ECHOOFMIND_API ALeverActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALeverActor();

	UFUNCTION(BlueprintCallable, Category = "Lever")
	void ToggleLever();

	UPROPERTY(BlueprintReadOnly, Category = "Lever")
	bool bIsOn = false;

	UPROPERTY(BlueprintAssignable, Category = "Lever")
	FOnLeverStateChanged OnLeverStateChanged;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Lever")
	UStaticMeshComponent* LeverMesh;

	UPROPERTY(VisibleAnywhere, Category = "Lever")
	UBoxComponent* Collision;

	FRotator UpRotation;
	FRotator DownRotation;

//public:	
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;

};
