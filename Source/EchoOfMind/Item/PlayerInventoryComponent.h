#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemContainerComponent.h"                 // Backpack(기본 컨테이너)
#include "PlayerInventoryComponent.generated.h"

// 전방 선언(헤더 경량화)
class UConsumableContainerComponent;

/**
 * 플레이어 인벤토리
 * - Backpack: 5x3
 * - Consumables: 1x4 (포션/열쇠 등 소모품 전용)
 */
UCLASS(ClassGroup = (Inventory), meta = (BlueprintSpawnableComponent))
class ECHOOFMIND_API UPlayerInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerInventoryComponent();

	/** 일반 가방(장비/아이템 무엇이든 수용) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Inventory")
	UItemContainerComponent* Backpack;

	/** 소모품 전용 그리드(포션/열쇠 등). 장비는 불가 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UConsumableContainerComponent* Consumables;

	/** 세션 종료(사망/시간만료) 때 가방/소모품 비우기 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ResetSessionInventory();

	// === Getters ===
	UFUNCTION(BlueprintPure, Category = "Inventory")
	UItemContainerComponent* GetBackpack() const { return Backpack; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	UConsumableContainerComponent* GetConsumables() const { return Consumables; }

protected:
	virtual void BeginPlay() override;
};
