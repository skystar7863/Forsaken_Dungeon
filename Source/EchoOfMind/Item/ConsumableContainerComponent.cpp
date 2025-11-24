#include "ConsumableContainerComponent.h"
#include "ConsumableDefinition.h"
#include "EchoPlayer.h"

bool UConsumableContainerComponent::IsConsumable(const FItemInstance& Item) const
{
    if (!Item.Def) return false;
    if (const UConsumableDefinition* C = Cast<UConsumableDefinition>(Item.Def))
        return C->bIsConsumable;
    return false;
}

bool UConsumableContainerComponent::CanAcceptItem(const FItemInstance& Item, int32 /*Index*/) const
{
    return IsConsumable(Item);
}

int32 UConsumableContainerComponent::GetMaxStackForItem(const FItemInstance& Item) const
{
    if (const UConsumableDefinition* C = Cast<UConsumableDefinition>(Item.Def))
        return FMath::Max(1, C->MaxStack);
    return 1;
}

bool UConsumableContainerComponent::ConsumeAt(int32 Index, AEchoPlayer* Player)
{
    if (!IsValid(Player)) return false;

    FItemInstance Item;
    if (!GetSlot(Index, Item) || !Item.Def) return false;
    const UConsumableDefinition* Def = Cast<UConsumableDefinition>(Item.Def);
    if (!Def) return false;

    if (!Def->Consume(Player))
        return false;

    // 1개 소모
    Item.StackCount = FMath::Max(0, Item.StackCount - 1);
    if (Item.StackCount <= 0)
    {
        RemoveAt(Index);
    }
    else
    {
        SetSlotDirect(Index, Item); // 스택 갱신
    }

    BroadcastChanged();
    return true;
}
