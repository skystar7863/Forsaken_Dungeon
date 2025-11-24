// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "Components/Image.h"


void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (ItemImage) {
		ItemImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInventoryWidget::SetItemIcon(UTexture2D* Icon)
{
	if (!ItemImage) return;
	if (Icon) {
		ItemImage->SetBrushFromTexture(Icon);
		ItemImage->SetVisibility(ESlateVisibility::Visible);
	}
	else {
		ItemImage->SetVisibility(ESlateVisibility::Hidden);
	}
}
