#include "EquipmentSlotWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

#include "Item/ItemInstance.h"
#include "ItemDefinition.h"              // EItemRarity, UItemDefinition
#include "ItemContainerComponent.h"
#include "ItemDragPayload.h"
#include "Blueprint/DragDropOperation.h"
#include "InputCoreTypes.h"
#include "InventoryRootWidget.h"

void UEquipmentSlotWidget::InitializeEquipment(UEquipmentComponent* InEquip)
{
    Equip = InEquip;
    Refresh();
}

void UEquipmentSlotWidget::Refresh()
{
    if (SlotName)
    {
        FText Label;
        switch (SlotType)
        {
        case EEquipSlot::Weapon: Label = FText::FromString(TEXT("Weapon")); break;
        case EEquipSlot::Helmet: Label = FText::FromString(TEXT("Helmet")); break;
        case EEquipSlot::Chest:  Label = FText::FromString(TEXT("Chest"));  break;
        case EEquipSlot::Leg:    Label = FText::FromString(TEXT("Leg"));    break;
        case EEquipSlot::Boots:  Label = FText::FromString(TEXT("Boots"));  break;
        default:                 Label = FText::GetEmpty();                  break;
        }
        SlotName->SetText(Label);
    }

    if (!Equip)
    {
        SetIcon(nullptr);
        UpdateRarityBorder(nullptr);
        return;
    }

    const FItemInstance* Found = Equip->Equipped.Find(SlotType);
    if (Found)
    {
        // 아이콘
        SetIcon((Found->Def) ? Found->Def->Icon : nullptr);
        // 희귀도 테두리
        UpdateRarityBorder(Found);
    }
    else
    {
        SetIcon(nullptr);
        UpdateRarityBorder(nullptr);
    }
}

bool UEquipmentSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    if (!Equip || !InOperation) return false;

    if (UItemDragPayload* Payload = Cast<UItemDragPayload>(InOperation->Payload))
    {
        if (!Payload->SourceContainer) return false;

        const bool bEquipped = Equip->EquipFrom(Payload->SourceContainer, Payload->SourceIndex, SlotType);
        if (bEquipped)
        {
            Refresh();
            return true;
        }
    }
    return false;
}

FReply UEquipmentSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    // 우클릭으로 해제 → 가방 첫 빈칸으로
    if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        if (Equip && TargetUnequipContainer)
        {
            if (Equip->UnequipTo(TargetUnequipContainer, SlotType))
            {
                Refresh();
                return FReply::Handled();
            }
        }
    }
    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UEquipmentSlotWidget::SetIcon(UTexture2D* Tex)
{
    if (!Icon) return;

    Icon->SetBrushFromTexture(Tex, true);
    Icon->SetOpacity(Tex ? 1.f : 0.15f);
}

void UEquipmentSlotWidget::UpdateRarityBorder(const FItemInstance* Instance)
{
    if (!RarityBorder)
        return; // 위젯에 테두리 없으면 스킵

    // 기본값
    EItemRarity FinalRarity = EItemRarity::Common;

    if (Instance)
    {
        // 1) 인스턴스 Rarity가 Common이 아니면 그 값을 우선
        if (Instance->Rarity != EItemRarity::Common)
        {
            FinalRarity = Instance->Rarity;
        }
        // 2) 아니면 Def가 있으면 Def의 Rarity로 폴백
        else if (Instance->Def)
        {
            FinalRarity = Instance->Def->Rarity;
        }
    }

    // Common이면 테두리 숨김, Rare 이상에서만 표시
    const bool bShow =
        (FinalRarity == EItemRarity::Rare) ||
        (FinalRarity == EItemRarity::Superior) ||
        (FinalRarity == EItemRarity::Heroic) ||
        (FinalRarity == EItemRarity::Legendary);

    if (!bShow)
    {
        RarityBorder->SetVisibility(ESlateVisibility::Hidden);
        return;
    }

    RarityBorder->SetBrushColor(GetRarityColor(FinalRarity));
    RarityBorder->SetVisibility(ESlateVisibility::Visible);
}

FLinearColor UEquipmentSlotWidget::GetRarityColor(EItemRarity Rarity)
{
    switch (Rarity)
    {
    case EItemRarity::Rare:       return FLinearColor(0.15f, 0.8f, 0.2f, 1.f);  // 초록
    case EItemRarity::Superior:   return FLinearColor(0.25f, 0.45f, 1.0f, 1.f); // 파랑
    case EItemRarity::Heroic:     return FLinearColor(0.7f, 0.3f, 0.9f, 1.f); // 보라
    case EItemRarity::Legendary:  return FLinearColor(1.0f, 0.3f, 0.2f, 1.f); // 주황/빨강 계열
    default:                      return FLinearColor::Transparent;
    }
}

void UEquipmentSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

    UInventoryRootWidget* Root = GetTypedOuter<UInventoryRootWidget>();
    if (!Root) return;

    const FItemInstance* Found = (Equip) ? Equip->Equipped.Find(SlotType) : nullptr;
    if (!Found || !Found->Def)
    {
        Root->HideItemTooltip();
        return;
    }

    // 유효할 때만 표시
    Root->ShowItemTooltip(*Found);
}

void UEquipmentSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);

    if (UInventoryRootWidget* Root = GetTypedOuter<UInventoryRootWidget>())
    {
        Root->HideItemTooltip();
    }
}
