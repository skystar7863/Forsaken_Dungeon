#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/ItemEnums.h"
#include "ItemSlotWidget.generated.h"

class UImage;
class UTextBlock;
class UItemContainerComponent;
class UTexture2D;
class UDragDropOperation;
class UBorder;
class UWidgetAnimation;

/** 슬롯 클릭 (읽기 전용 모드에서 사용) */
DECLARE_DELEGATE_OneParam(FOnSlotClicked, int32 /*Index*/);

UCLASS()
class ECHOOFMIND_API UItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable) void Setup(UItemContainerComponent* InContainer, int32 InIndex);
	UFUNCTION(BlueprintCallable) void Refresh();

	/** 드래그 비주얼 클래스 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> DragVisualClass;

	/** 읽기 전용: 드래그/드롭 비활성 */
	UFUNCTION(BlueprintCallable) void SetReadOnly(bool bIn) { bReadOnly = bIn; }

	/** 좌상단 키 라벨 */
	UFUNCTION(BlueprintCallable) void SetKeyLabel(const FText& Label);
	void SetOnClicked(FOnSlotClicked In) { OnSlotClicked = In; }

	/** 등급 컬러 (원하면 에디터에서 조절) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rarity") FLinearColor RareColor = FLinearColor(0.20f, 0.80f, 0.30f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rarity") FLinearColor SuperiorColor = FLinearColor(0.20f, 0.55f, 1.00f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rarity") FLinearColor HeroicColor = FLinearColor(0.65f, 0.35f, 1.00f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rarity") FLinearColor LegendaryColor = FLinearColor(1.00f, 0.25f, 0.25f);

	/** ?? 프레임 텍스처(가운데가 투명인 PNG/UMG용 머티리얼) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rarity|Frame")
	UTexture2D* RarityFrameTexture = nullptr;

	/** ?? 9-슬라이스 마진(프레임 텍스처 테두리 두께 비율) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rarity|Frame", meta = (ClampMin = "0.0", ClampMax = "0.49"))
	float RarityFrameMargin = 0.45f;

protected:
	// 바인딩
	UPROPERTY(meta = (BindWidget)) UImage* Icon;
	UPROPERTY(meta = (BindWidget)) UTextBlock* StackText;
	UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* KeyLabel;

	// ?? 테두리/글로우
	UPROPERTY(meta = (BindWidgetOptional)) UBorder* RarityBorder;   // 단색 테두리(옵션)
	UPROPERTY(meta = (BindWidgetOptional)) UImage* RarityGlow;     // 프레임 텍스처(컬러 틴트)

	// ?? UMG 애니메이션(이름 GlowPulse 로 만들어두면 자동 바인딩)
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* GlowPulse;

	UPROPERTY() UItemContainerComponent* Container = nullptr;
	UPROPERTY() int32 SlotIndex = INDEX_NONE;
	UPROPERTY() bool bReadOnly = false;
	FOnSlotClicked OnSlotClicked;

	// 입력/드래그
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void   NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool   NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool   NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	void SetIconAndStack(UTexture2D* Tex, int32 Count);

	// 등급 스타일
	void ApplyRarityStyle(EItemRarity Rarity);
	FLinearColor GetColorForRarity(EItemRarity Rarity) const;

	// ?? 프레임 브러시 적용 도우미
	void EnsureRarityGlowBrush();

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	/** 현재 슬롯 아이템 얻기 (컨테이너/인덱스 기반) */
	const struct FItemInstance* GetItemInstance() const;
};
