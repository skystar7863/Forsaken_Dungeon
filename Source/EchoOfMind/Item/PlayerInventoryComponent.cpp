#include "PlayerInventoryComponent.h"
#include "ItemContainerComponent.h"
#include "ConsumableContainerComponent.h"   // 소모품 전용 컨테이너

UPlayerInventoryComponent::UPlayerInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	Backpack = CreateDefaultSubobject<UItemContainerComponent>(TEXT("Backpack"));
	Consumables = CreateDefaultSubobject<UConsumableContainerComponent>(TEXT("Consumables"));
}

void UPlayerInventoryComponent::ResetSessionInventory()
{
	if (Backpack)
	{
		Backpack->Clear();          // ✅ ClearAll → Clear 로 통일
	}
	if (Consumables)
	{
		Consumables->Clear();       // ✅ 소모품도 Clear
	}
}

void UPlayerInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Backpack)    Backpack->InitializeGrid(5, 3);    // 5x3
	if (Consumables) Consumables->InitializeGrid(4, 1); // 1x4 (좌→우 4칸)
}
