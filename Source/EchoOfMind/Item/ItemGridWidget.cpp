#include "ItemGridWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "ItemContainerComponent.h"

#include "ItemSlotWidget.h"

void UItemGridWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UItemGridWidget::InitializeWithContainer(UItemContainerComponent* InContainer)
{
    Container = InContainer;
    if (!Container || !Grid || !ItemSlotWidgetClass) return;

    // 컨테이너 변경시 자동 새로고침
    Container->OnContainerChanged.AddDynamic(this, &UItemGridWidget::Rebuild);
    Rebuild();
}

void UItemGridWidget::SetKeyLabels(const TArray<FText>& InLabels)
{
    KeyLabels = InLabels;
    // 이미 빌드되어 있으면 갱신
    Rebuild();
}

void UItemGridWidget::Rebuild()
{
    if (!Container || !Grid || !ItemSlotWidgetClass) return;

    ClearGrid();

    const int32 W = Container->GetWidth();
    const int32 H = Container->GetHeight();

    for (int32 y = 0; y < H; ++y)
    {
        for (int32 x = 0; x < W; ++x)
        {
            const int32 Index = y * W + x;

            // ?? 여기 수정
            UItemSlotWidget* CellWidget = CreateWidget<UItemSlotWidget>(this, ItemSlotWidgetClass);
            if (!CellWidget) continue;

            CellWidget->Setup(Container, Index);
            CellWidget->SetReadOnly(bReadOnly);

            if (KeyLabels.IsValidIndex(Index))
                CellWidget->SetKeyLabel(KeyLabels[Index]);
            else
                CellWidget->SetKeyLabel(FText::GetEmpty());

            CellWidget->SetOnClicked(FOnSlotClicked::CreateUObject(this, &UItemGridWidget::HandleSlotClicked));

            if (UUniformGridSlot* GridSlot = Grid->AddChildToUniformGrid(CellWidget, y, x))
            {
                // GridSlot->SetHorizontalAlignment(HAlign_Fill);
                // GridSlot->SetVerticalAlignment(VAlign_Fill);
            }
        }
    }
}

void UItemGridWidget::HandleSlotClicked(int32 Index)
{
    OnSlotClicked.Broadcast(Index);
}

void UItemGridWidget::ClearGrid()
{
    if (!Grid) return;
    Grid->ClearChildren();
}
