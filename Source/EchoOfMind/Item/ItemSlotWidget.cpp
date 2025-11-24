#include "ItemSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/DragDropOperation.h"

#include "ItemContainerComponent.h"
#include "Item/ItemInstance.h"
#include "ConsumableContainerComponent.h"
#include "ItemDefinition.h"
#include "ItemDragPayload.h"
#include "ItemDragWidget.h"
#include "Item/ItemEnums.h"
#include "InventoryRootWidget.h"

void UItemSlotWidget::Setup(UItemContainerComponent* InContainer, int32 InIndex)
{
	Container = InContainer;
	SlotIndex = InIndex;
	Refresh();
}

// ItemSlotWidget.cpp
void UItemSlotWidget::Refresh()
{
	if (!Container || SlotIndex == INDEX_NONE)
	{
		SetIconAndStack(nullptr, 0);
		ApplyRarityStyle(EItemRarity::Common);
		return;
	}

	FItemInstance Item;
	if (Container->GetSlot(SlotIndex, Item) && Item.Def)
	{
		// 아이콘/스택
		SetIconAndStack(Item.Def->Icon, Item.StackCount);

		// 👇 중요: 인스턴스 Rarity 우선, 없으면 Def Rarity
		const EItemRarity FinalRarity =
			(Item.Rarity != EItemRarity::Common) ? Item.Rarity :
			(Item.Def ? Item.Def->Rarity : EItemRarity::Common);

		ApplyRarityStyle(FinalRarity);

		// (선택) 디버그
		// UE_LOG(LogTemp, Warning, TEXT("[Slot %d] Def=%s, InstR=%d, DefR=%d, Final=%d"),
		//     SlotIndex, *Item.Def->GetName(),
		//     (int32)Item.Rarity, (int32)Item.Def->Rarity, (int32)FinalRarity);
	}
	else
	{
		SetIconAndStack(nullptr, 0);
		ApplyRarityStyle(EItemRarity::Common);
	}
}


void UItemSlotWidget::SetKeyLabel(const FText& Label)
{
	if (KeyLabel) KeyLabel->SetText(Label);
}

void UItemSlotWidget::SetIconAndStack(UTexture2D* Tex, int32 Count)
{
	if (Icon)      Icon->SetBrushFromTexture(Tex, true);
	if (StackText) StackText->SetText(Count > 1 ? FText::AsNumber(Count) : FText::GetEmpty());
}

FReply UItemSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bReadOnly)
	{
		// 읽기 전용: 드래그 시작 막고 클릭 처리만 OnMouseButtonUp에서
		return FReply::Handled();
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;

	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (OnSlotClicked.IsBound())
		{
			OnSlotClicked.Execute(SlotIndex); // 그리드로 우클릭 인덱스 전달
		}
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UItemSlotWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bReadOnly && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (OnSlotClicked.IsBound())
		{
			OnSlotClicked.Execute(SlotIndex);
			return FReply::Handled();
		}
	}
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UItemSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	if (bReadOnly || !Container || SlotIndex == INDEX_NONE)
		return;

	// 빈 슬롯이면 드래그 안 함
	FItemInstance Item;
	if (!(Container->GetSlot(SlotIndex, Item) && Item.Def))
		return;

	UDragDropOperation* Op = UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass());
	OutOperation = Op;

	UItemDragPayload* Payload = NewObject<UItemDragPayload>();
	Payload->SourceContainer = Container;
	Payload->SourceIndex = SlotIndex;
	Op->Payload = Payload;

	// 드래그 비주얼
	if (DragVisualClass)
	{
		UItemDragWidget* Visual = CreateWidget<UItemDragWidget>(GetWorld(), DragVisualClass);
		if (Visual)
		{
			Visual->SetIcon(Item.Def->Icon);
			Visual->SetStack(Item.StackCount);
			Visual->SetRarity(Item.Def->Rarity); // 있을 때만 색 입힘(DragFrame 없으면 무시)
			Op->DefaultDragVisual = Visual;
		}
	}
	Op->Pivot = EDragPivot::MouseDown;
}

bool UItemSlotWidget::NativeOnDrop(const FGeometry& G, const FDragDropEvent& E, UDragDropOperation* Op)
{
	if (bReadOnly || !Container || SlotIndex == INDEX_NONE || !Op) return false;

	if (UItemDragPayload* Payload = Cast<UItemDragPayload>(Op->Payload))
	{
		if (!Payload->SourceContainer) return false;

		// 🔹 사전 검사: 목적지 컨테이너에서 허용하는지 확인
		FItemInstance DragItem;
		if (Payload->SourceContainer->GetSlot(Payload->SourceIndex, DragItem))
		{
			if (!Container->CanAcceptItem(DragItem, SlotIndex))
			{
				return false; // 무기→소모품칸 같은 드롭 거절
			}
		}

		if (Payload->SourceContainer == Container)
		{
			return Container->SwapSlots(Payload->SourceIndex, SlotIndex);
		}
		else
		{
			return Payload->SourceContainer->MoveTo(Container, Payload->SourceIndex, SlotIndex);
		}
	}
	return false;
}

bool UItemSlotWidget::NativeOnDragOver(const FGeometry& G, const FDragDropEvent& E, UDragDropOperation* Op)
{
	if (bReadOnly || !Container || SlotIndex == INDEX_NONE || !Op) return false;

	if (UItemDragPayload* Payload = Cast<UItemDragPayload>(Op->Payload))
	{
		FItemInstance DragItem;
		if (Payload->SourceContainer && Payload->SourceContainer->GetSlot(Payload->SourceIndex, DragItem))
		{
			// 🔹 허용 안 되면 false 반환 → 드롭 효과/하이라이트를 엔진이 막아줌
			return Container->CanAcceptItem(DragItem, SlotIndex);
		}
	}
	return false;
}

FLinearColor UItemSlotWidget::GetColorForRarity(EItemRarity Rarity) const
{
	switch (Rarity)
	{
	case EItemRarity::Rare:      return RareColor;
	case EItemRarity::Superior:  return SuperiorColor;
	case EItemRarity::Heroic:    return HeroicColor;
	case EItemRarity::Legendary: return LegendaryColor;
	default:                     return FLinearColor::Transparent; // Common/Consumable
	}
}

void UItemSlotWidget::ApplyRarityStyle(EItemRarity Rarity)
{
	// 1) 테두리 처리
	if (RarityBorder)
	{
		const FLinearColor C = GetColorForRarity(Rarity);

		if (Rarity == EItemRarity::Rare ||
			Rarity == EItemRarity::Superior ||
			Rarity == EItemRarity::Heroic ||
			Rarity == EItemRarity::Legendary)
		{
			RarityBorder->SetBrushColor(C);
			RarityBorder->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			RarityBorder->SetVisibility(ESlateVisibility::Hidden); // Common/Consumable
			//RarityBorder->SetBrushColor(FLinearColor(1.00f, 0.25f, 0.25f));
		}
	}

	// 2) 빛나는 효과(옵션) - Heroic 이상만
	if (RarityGlow)
	{
		if (Rarity == EItemRarity::Heroic || Rarity == EItemRarity::Legendary)
		{
			RarityGlow->SetColorAndOpacity(GetColorForRarity(Rarity));
			RarityGlow->SetVisibility(ESlateVisibility::HitTestInvisible);

			// 애니메이션 있으면 실행
			if (GlowPulse && !IsAnimationPlaying(GlowPulse))
			{
				PlayAnimation(GlowPulse, 0.f, 0 /*loop forever*/, EUMGSequencePlayMode::Forward);
			}
		}
		else
		{
			if (GlowPulse && IsAnimationPlaying(GlowPulse))
				StopAnimation(GlowPulse);

			RarityGlow->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

const FItemInstance* UItemSlotWidget::GetItemInstance() const
{
	if (!Container) return nullptr;            // 너의 기존 멤버: UItemContainerComponent* Container
	return Container->Peek(SlotIndex);
}

void UItemSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (UInventoryRootWidget* Root = GetTypedOuter<UInventoryRootWidget>())
	{
		const FItemInstance* Inst = GetItemInstance();
		if (!Inst || !Inst->Def)
		{
			Root->HideItemTooltip();
			return;
		}
		Root->ShowItemTooltip(*Inst);

	}
}


void UItemSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	if (UInventoryRootWidget* Root = GetTypedOuter<UInventoryRootWidget>())
	{
		Root->HideItemTooltip();
	}
}

/*
void UItemSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

    UInventoryRootWidget* Root = GetTypedOuter<UInventoryRootWidget>();
    if (!Root) return;

    // 현재 슬롯 아이템 읽기
    FItemInstance Item;
    const bool bHasItem =
        (Container && SlotIndex != INDEX_NONE &&
         Container->GetSlot(SlotIndex, Item) &&
         Item.Def != nullptr);

    if (!bHasItem)
    {
        Root->HideItemTooltip();
        return;
    }

    // 유효할 때만 표시
    Root->ShowItemTooltip(Item); // const FItemInstance& 로 전달
}

void UItemSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);

    if (UInventoryRootWidget* Root = GetTypedOuter<UInventoryRootWidget>())
    {
        Root->HideItemTooltip();
    }
}
*/