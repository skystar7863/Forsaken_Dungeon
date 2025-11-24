#include "EnemyLootComponent.h"
// UItemContainerComponent 안에 Slots(TArray<FItemInstance>)가 있다고 가정
// FItemInstance: Def(UItemDefinition*), StackCount(int32) 등

UEnemyLootComponent::UEnemyLootComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	InitializeGrid(4, 2); // 4x2 전리품 그리드
}

void UEnemyLootComponent::FillFromLootTable()
{
	// TODO: LootTable DataAsset 기반으로 Slots 채우기
	// Slots[..] = FItemInstance{ ... };
	BroadcastChanged();
}

bool UEnemyLootComponent::IsEmpty() const
{
	for (const FItemInstance& It : Slots)
	{
		if (It.Def != nullptr && It.StackCount > 0)
		{
			return false;
		}
	}
	return true;
}

int32 UEnemyLootComponent::GetTotalItemCount() const
{
	int32 Sum = 0;
	for (const FItemInstance& It : Slots)
	{
		if (It.Def != nullptr && It.StackCount > 0)
		{
			Sum += It.StackCount;
		}
	}
	return Sum;
}
