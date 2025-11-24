// Fill out your copyright notice in the Description page of Project Settings.


#include "TextBoxActor.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "TextBoxWidget.h"
#include "DoorActor.h"


ATextBoxActor::ATextBoxActor()
{
	// ±âº»°ª
	CorrectCode = TEXT("0000");
	TextBoxWidgetInstance = nullptr;
}


void ATextBoxActor::PressButton()
{
	if (!TextBoxWidgetClass) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	if (TextBoxWidgetInstance == nullptr) {
		UTextBoxWidget* TBW = CreateWidget<UTextBoxWidget>(PC, TextBoxWidgetClass);
		if (!TBW) return;

		TextBoxWidgetInstance = TBW;
		TBW->Initialize(this, CorrectCode);
		TBW->AddToViewport();

		FInputModeUIOnly Mode;
		Mode.SetWidgetToFocus(TBW->TakeWidget());
		PC->SetInputMode(Mode);
		PC->bShowMouseCursor = true;

	}
}

void ATextBoxActor::OnCodeSubmitted(const FString& EnteredCode)
{
	if (EnteredCode.Equals(CorrectCode)) {
		for (ADoorActor* Door : ControlledDoors)
		{
			if (Door)
			{
				Door->OpenDoor();
			}
		}
	}

	if (TextBoxWidgetInstance) {
		TextBoxWidgetInstance->RemoveFromParent();
		TextBoxWidgetInstance = nullptr;
	
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		{
			FInputModeGameOnly Mode;
			PC->SetInputMode(Mode);
			PC->bShowMouseCursor = false;
		}
	}
}
