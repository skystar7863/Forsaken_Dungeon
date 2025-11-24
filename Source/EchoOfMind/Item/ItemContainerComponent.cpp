#include "ItemContainerComponent.h"

UItemContainerComponent::UItemContainerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	Width = 0;
	Height = 0;
}

void UItemContainerComponent::InitializeGrid(int32 InWidth, int32 InHeight)
{
	Width = FMath::Max(0, InWidth);
	Height = FMath::Max(0, InHeight);
	Slots.SetNum(Width * Height);
	BroadcastChanged();
}

bool UItemContainerComponent::GetSlot(int32 Index, FItemInstance& OutItem) const
{
	if (!IsValidIndex(Index)) return false;
	OutItem = Slots[Index];
	return (Slots[Index].Def != nullptr || Slots[Index].StackCount > 0);
}

int32 UItemContainerComponent::FindFirstEmptySlot() const
{
	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		if (Slots[i].Def == nullptr || Slots[i].StackCount <= 0)
			return i;
	}
	return INDEX_NONE;
}

int32 UItemContainerComponent::AddItem(const FItemInstance& Item)
{
	if (!CanAcceptItem(Item) || Item.Def == nullptr) return INDEX_NONE;

	// 간단: 스택 합치기는 추후 확장. 지금은 빈 슬롯에만 배치
	int32 FreeIdx = FindFirstEmptySlot();
	if (FreeIdx == INDEX_NONE) return INDEX_NONE;

	Slots[FreeIdx] = Item;
	if (Slots[FreeIdx].StackCount <= 0)
		Slots[FreeIdx].StackCount = 1;

	BroadcastChanged();
	return FreeIdx;
}

bool UItemContainerComponent::RemoveAt(int32 Index)
{
	if (!IsValidIndex(Index)) return false;
	Slots[Index] = FItemInstance{};
	BroadcastChanged();
	return true;
}

bool UItemContainerComponent::SwapSlots(int32 IndexA, int32 IndexB)
{
	if (!IsValidIndex(IndexA) || !IsValidIndex(IndexB)) return false;
	Slots.Swap(IndexA, IndexB);
	BroadcastChanged();
	return true;
}

// 교차 컨테이너 이동(스왑 지원)
bool UItemContainerComponent::MoveTo(UItemContainerComponent* Target, int32 FromIndex, int32 ToIndex)
{
	if (!Target || !IsValidIndex(FromIndex) || !Target->IsValidIndex(ToIndex))
		return false;

	FItemInstance& Src = Slots[FromIndex];
	if (Src.Def == nullptr || Src.StackCount <= 0)
		return false;

	FItemInstance& Dst = Target->Slots[ToIndex];

	// 타깃 컨테이너가 Src 아이템을 수용할 수 있어야 한다
	if (!Target->CanAcceptItem(Src, ToIndex)) return false;

	if (Dst.Def != nullptr && Dst.StackCount > 0)
	{
		// 스왑 시, 우리 쪽이 Dst도 수용 가능한지 체크
		if (!CanAcceptItem(Dst, FromIndex))
		{
			// 못 받으면, 우리 컨테이너의 빈 슬롯에 임시 대피
			const int32 Free = FindFirstEmptySlot();
			if (Free == INDEX_NONE) return false;
			Slots[Free] = Dst;  // 대피
			Dst = Src;   // 타깃에는 Src 배치
			Src = FItemInstance{};
		}
		else
		{
			Swap(Src, Dst);
		}
	}
	else
	{
		// 타깃이 비었으면 단순 이동
		Dst = Src;
		Src = FItemInstance{};
	}

	BroadcastChanged();
	Target->BroadcastChanged();
	return true;
}

bool UItemContainerComponent::HasItemAt(int32 Index) const
{
	if (!IsValidIndex(Index)) return false;
	const FItemInstance& S = Slots[Index];
	return (S.Def != nullptr && S.StackCount > 0);
}

bool UItemContainerComponent::SetSlotDirect(int32 Index, const FItemInstance& Item)
{
	if (!IsValidIndex(Index)) return false;

	// 빈 값이면 지우기로 처리
	if (Item.Def == nullptr || Item.StackCount <= 0)
	{
		Slots[Index] = FItemInstance{};
		BroadcastChanged();
		return true;
	}

	// 정상 아이템이면 수용 가능성 체크
	if (!CanAcceptItem(Item, Index)) return false;

	Slots[Index] = Item;
	BroadcastChanged();
	return true;
}

void UItemContainerComponent::Clear()
{
	for (FItemInstance& It : Slots)
	{
		It = FItemInstance{};
	}
	BroadcastChanged();
}

const FItemInstance* UItemContainerComponent::Peek(int32 Index) const
{
	if (!IsValidIndex(Index))
	{
		return nullptr;
	}
	return &Slots[Index]; // 내부 TArray<FItemInstance> 같은 저장소에서 참조 반환
}


