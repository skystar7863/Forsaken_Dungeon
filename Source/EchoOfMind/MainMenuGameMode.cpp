#include "MainMenuGameMode.h"
#include "MainMenuWidget.h"
#include "Blueprint/UserWidget.h"

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (!MenuWidgetClass)
	{
		MenuWidgetClass = UMainMenuWidget::StaticClass(); // 기본 C++ 클래스 사용
	}

	MenuWidget = CreateWidget<UMainMenuWidget>(GetWorld(), MenuWidgetClass);
	if (MenuWidget)
	{
		MenuWidget->AddToViewport();

		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC)
		{
			PC->bShowMouseCursor = true;
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(MenuWidget->TakeWidget());
			PC->SetInputMode(InputMode);
		}
	}
}
