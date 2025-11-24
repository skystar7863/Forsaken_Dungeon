// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ButtonActor.h"
#include "TextBoxActor.generated.h"

class UUserWidget;
class UTextBoxWidget;


UCLASS()
class ECHOOFMIND_API ATextBoxActor : public AButtonActor
{
	GENERATED_BODY()
	

public:
	ATextBoxActor();

	virtual void PressButton() override;

	void OnCodeSubmitted(const FString& EnteredCode);


protected:
	UPROPERTY(EditAnywhere, Category = "TextBox")
	TSubclassOf<UUserWidget> TextBoxWidgetClass;

	UPROPERTY()
	UUserWidget* TextBoxWidgetInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TextBox")
	FString CorrectCode;
};
