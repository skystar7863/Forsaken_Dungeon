// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ButtonActor.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class ADoorActor;

UCLASS()
class ECHOOFMIND_API AButtonActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AButtonActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Button")
	UStaticMeshComponent* ButtonMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Button")
	UBoxComponent* Collision;

	//이 버튼이 열어줄 문들(여러개 가능), 레벨에 배치한 BP 에서 배열로 문을 드래그&드롭하세요
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
	TArray<ADoorActor*> ControlledDoors;

	UFUNCTION(BlueprintCallable, Category = "Button")
	virtual void PressButton();

//protected:
//	// Called when the game starts or when spawned
//	virtual void BeginPlay() override;
//
//public:	
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;

};
