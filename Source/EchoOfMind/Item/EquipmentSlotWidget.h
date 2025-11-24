#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentComponent.h"            // EEquipSlot, UEquipmentComponent
#include "EquipmentSlotWidget.generated.h"

class UImage;
class UTextBlock;
class UEquipmentComponent;
class UItemContainerComponent;
class UDragDropOperation;
class UBorder;

/**
 * 장비 슬롯 UI (무기/투구/상의/하의/신발)
 * - 아이콘 표시
 * - 희귀도 테두리(RarityBorder) 색상/가시성 갱신
 * - 드래그앤드롭 장착/해제
 */
UCLASS()
class ECHOOFMIND_API UEquipmentSlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** 어떤 장비 슬롯인지 지정(무기/투구/...) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
    EEquipSlot SlotType = EEquipSlot::Weapon;

    /** 연결할 장비 컴포넌트 */
    UFUNCTION(BlueprintCallable) void InitializeEquipment(UEquipmentComponent* InEquip);

    /** 장착 상태 UI 갱신 */
    UFUNCTION(BlueprintCallable) void Refresh();

    /** (선택) 우클릭 시 가방으로 해제할 때 쓸 컨테이너 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
    UItemContainerComponent* TargetUnequipContainer = nullptr;



protected:
    UPROPERTY(meta = (BindWidget)) UImage* Icon = nullptr;
    UPROPERTY(meta = (BindWidget)) UTextBlock* SlotName = nullptr;

    /** 희귀도 테두리 (옵션) - BP에 없으면 무시 */
    UPROPERTY(meta = (BindWidgetOptional)) UBorder* RarityBorder = nullptr;

    UPROPERTY() UEquipmentComponent* Equip = nullptr;

    virtual bool   NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
private:
    void SetIcon(UTexture2D* Tex);
    void UpdateRarityBorder(const struct FItemInstance* Instance);

    static FLinearColor GetRarityColor(EItemRarity Rarity);

};
