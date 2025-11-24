#pragma once

#include "CoreMinimal.h"
#include "ItemContainerComponent.h"
#include "EnemyLootComponent.generated.h"

/**
 * 적/상자 전리품(4x2)
 */
UCLASS(ClassGroup = (Inventory), meta = (BlueprintSpawnableComponent))
class ECHOOFMIND_API UEnemyLootComponent : public UItemContainerComponent
{
	GENERATED_BODY()

public:
	UEnemyLootComponent();

	/** 테이블에서 드롭 채우기(추후 구현) */
	UFUNCTION(BlueprintCallable, Category = "Loot")
	void FillFromLootTable(); // TODO

	/** 전리품이 비어있는지(모든 슬롯 Def==nullptr 혹은 Stack==0) */
	UFUNCTION(BlueprintPure, Category = "Loot")
	bool IsEmpty() const;

	/** 전리품 총 스택 수(소모품 수량 같은 표시용) */
	UFUNCTION(BlueprintPure, Category = "Loot")
	int32 GetTotalItemCount() const;
};
