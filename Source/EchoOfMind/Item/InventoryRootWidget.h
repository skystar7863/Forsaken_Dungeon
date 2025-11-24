#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemTooltipWidget.h"
#include "InventoryRootWidget.generated.h"

class AEchoPlayer;
class UItemGridWidget;
class UEquipmentSlotWidget;
class UPlayerInventoryComponent;
class UEquipmentComponent;
class UEnemyLootComponent;
class ULobbyStashComponent;
class UTextBlock;
class ALootChestActor;

/**
 * 인벤토리 루트 (가방/소모품/전리품/장비/로비스태시)
 */
UCLASS()
class ECHOOFMIND_API UInventoryRootWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 전리품 열고/닫기
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void OpenLoot(UEnemyLootComponent* LootComp, ALootChestActor* ChestOwner = nullptr);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void CloseLoot();

	// 로비 스태시 열고/닫기
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void OpenLobbyStash(ULobbyStashComponent* InStash);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void CloseLobbyStash();

	// 인벤토리만(전리품/스태시 비가시화)
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ShowInventoryOnly();

	UPROPERTY(EditAnywhere, Category = "Tooltip")
	TSubclassOf<UItemTooltipWidget> ItemTooltipClass;

	// 슬롯에서 호출
	void ShowItemTooltip( FItemInstance Instance);
	void HideItemTooltip();

	

	UFUNCTION()
	void HandleBackpackRightClick(int32 Index);

	UFUNCTION()
	void HandleLootRightClick(int32 Index);

	UFUNCTION()
	void HandleStashRightClick(int32 Index);



protected:
	virtual void NativeConstruct() override;
	void SetupWithOwningPlayer();

	// BindWidget
	UPROPERTY(meta = (BindWidget)) UItemGridWidget* BackpackGrid;
	UPROPERTY(meta = (BindWidget)) UItemGridWidget* ConsumableGrid;

	UPROPERTY(meta = (BindWidget)) UItemGridWidget* LootGrid;
	UPROPERTY(meta = (BindWidget)) UTextBlock* LootTitleText;

	// ✅ 로비 스태시 섹션(“Loot” 위치에 배치)
	UPROPERTY(meta = (BindWidget)) UItemGridWidget* LobbyStashGrid;
	UPROPERTY(meta = (BindWidget)) UTextBlock* LobbyStashTitleText;

	UPROPERTY(meta = (BindWidget)) UEquipmentSlotWidget* EquipSlot_Weapon;
	UPROPERTY(meta = (BindWidget)) UEquipmentSlotWidget* EquipSlot_Helmet;
	UPROPERTY(meta = (BindWidget)) UEquipmentSlotWidget* EquipSlot_Chest;
	UPROPERTY(meta = (BindWidget)) UEquipmentSlotWidget* EquipSlot_Leg;
	UPROPERTY(meta = (BindWidget)) UEquipmentSlotWidget* EquipSlot_Boots;

private:
	// 캐시
	UPROPERTY() AEchoPlayer* PlayerRef = nullptr;
	UPROPERTY() UPlayerInventoryComponent* InvRef = nullptr;
	UPROPERTY() UEquipmentComponent* EquipRef = nullptr;
	UPROPERTY() UEnemyLootComponent* CurrentLoot = nullptr;
	UPROPERTY() ULobbyStashComponent* StashRef = nullptr;
	UPROPERTY() UItemTooltipWidget* ItemTooltip = nullptr;

	/** 열려 있던 상자(전리품 위젯 닫을 때 뚜껑 처리용 – 현재는 무시) */
	TWeakObjectPtr<ALootChestActor> CurrentChest;

	UFUNCTION() void OnEquipmentChanged();
};
