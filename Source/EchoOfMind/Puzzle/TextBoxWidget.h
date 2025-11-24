// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TextBoxWidget.generated.h"

class UEditableTextBox;
class UButton;
class ATextBoxActor;

UCLASS()
class ECHOOFMIND_API UTextBoxWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="TextBox")
	void Initialize(ATextBoxActor* InOwner, const FString& InCorrectCode);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnSubmitClicked();

	UPROPERTY(meta=(BindWidget))
	UEditableTextBox* CodeInputBox;

	UPROPERTY(meta=(BindWidget))
	UButton* SubmitButton;

private:
	ATextBoxActor* OwnerActor;
	FString CorrentCode;
	
};
