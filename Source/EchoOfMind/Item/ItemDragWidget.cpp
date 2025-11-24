#include "ItemDragWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

void UItemDragWidget::SetIcon(UTexture2D* Tex)
{
	if (DragIcon)
		DragIcon->SetBrushFromTexture(Tex, true);
}

void UItemDragWidget::SetStack(int32 Count)
{
	if (StackText)
		StackText->SetText(Count > 1 ? FText::AsNumber(Count) : FText::GetEmpty());
}

void UItemDragWidget::SetRarity(EItemRarity Rarity)
{
	if (!DragFrame) return;

	FLinearColor C = FLinearColor::Transparent;
	switch (Rarity)
	{
	case EItemRarity::Rare:      C = FLinearColor(0.20f, 0.80f, 0.30f); break; // 초록
	case EItemRarity::Superior:  C = FLinearColor(0.20f, 0.55f, 1.00f); break; // 파랑
	case EItemRarity::Heroic:    C = FLinearColor(0.65f, 0.35f, 1.00f); break; // 보라
	case EItemRarity::Legendary: C = FLinearColor(1.00f, 0.25f, 0.25f); break; // 빨강 (무지개는 머티리얼 권장)
	default: break;
	}

	DragFrame->SetBrushColor(C);
	DragFrame->SetVisibility(C.A > 0.f ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
}
