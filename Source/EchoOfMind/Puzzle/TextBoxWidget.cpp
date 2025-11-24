// Fill out your copyright notice in the Description page of Project Settings.


#include "TextBoxWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "TextBoxActor.h"

void UTextBoxWidget::Initialize(ATextBoxActor* InOwner, const FString& InCorrectCode)
{
	OwnerActor = InOwner;
	CorrentCode = InCorrectCode;
}

void UTextBoxWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SubmitButton) {
		SubmitButton->OnClicked.AddDynamic(this, &UTextBoxWidget::OnSubmitClicked);
	}
}

void UTextBoxWidget::OnSubmitClicked()
{
	if (!OwnerActor || !CodeInputBox) return;

	FString Entered = CodeInputBox->GetText().ToString();
	OwnerActor->OnCodeSubmitted(Entered);
}
