#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Canvas 루트 생성
	UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	WidgetTree->RootWidget = RootCanvas;

	// ----- Title Text -----
	//TitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	//TitleText->SetText(FText::FromString(TEXT("Forsaken Dungeon")));
	//TitleText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 48));
	//TitleText->SetColorAndOpacity(FSlateColor(FLinearColor::White));

	//if (UCanvasPanelSlot* TitleSlot = RootCanvas->AddChildToCanvas(TitleText))
	//{
	//	TitleSlot->SetAnchors(FAnchors(0.5f, 0.3f));
	//	TitleSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	//	TitleSlot->SetAutoSize(true);
	//}

	//// ----- Game Start Button -----
	//GameStartButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	//if (GameStartButton)
	//{
	//	if (UCanvasPanelSlot* BtnSlot = RootCanvas->AddChildToCanvas(GameStartButton))
	//	{
	//		BtnSlot->SetAnchors(FAnchors(0.5f, 0.6f));
	//		BtnSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	//		BtnSlot->SetSize(FVector2D(300.f, 100.f));
	//	}

	//	// 버튼 안의 텍스트
	//	UTextBlock* BtnText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	//	BtnText->SetText(FText::FromString(TEXT("Game Start")));
	//	BtnText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 30));
	//	GameStartButton->AddChild(BtnText);

	//	// 클릭 이벤트 연결
	//	GameStartButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnGameStartClicked);
	//}
}

void UMainMenuWidget::OnGameStartClicked()
{
	UGameplayStatics::OpenLevel(this, FName(TEXT("L_Dungeon1")));
}
