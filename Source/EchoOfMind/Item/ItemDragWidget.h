#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/ItemEnums.h"               // EItemRarity
#include "ItemDragWidget.generated.h"

class UImage;
class UTextBlock;
class UTexture2D;
class UBorder;

UCLASS()
class ECHOOFMIND_API UItemDragWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable) void SetIcon(UTexture2D* Tex);
	UFUNCTION(BlueprintCallable) void SetStack(int32 Count);

	/** 드래그 미리보기에도 등급 프레임을 줄 수 있음(없어도 동작함) */
	UFUNCTION(BlueprintCallable) void SetRarity(EItemRarity Rarity);

protected:
	UPROPERTY(meta = (BindWidget)) UImage* DragIcon;
	UPROPERTY(meta = (BindWidget)) UTextBlock* StackText;

	/** Optional: 프레임 Border를 배치했다면 색 입힘 */
	UPROPERTY(meta = (BindWidgetOptional)) UBorder* DragFrame;
};
