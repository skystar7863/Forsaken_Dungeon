#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class ECHOOFMIND_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	/*UPROPERTY(meta = (BindWidget))
	UButton* GameStartButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TitleText;*/

	UFUNCTION()
	void OnGameStartClicked();
};
