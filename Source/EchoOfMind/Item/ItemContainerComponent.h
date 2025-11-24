#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/ItemInstance.h"
#include "ItemContainerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnContainerChanged);

/**
 * 공통 아이템 컨테이너(그리드)
 * - Width*Height 슬롯에 FItemInstance 저장
 * - Add/Remove/Swap/Move 간단 API
 * - CanAcceptItem 오버라이드해서 필터링 가능
 */
UCLASS(ClassGroup = (Inventory), meta = (BlueprintSpawnableComponent))
class ECHOOFMIND_API UItemContainerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UItemContainerComponent();

	/** 컨테이너 변경 시 UI 등 갱신 */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnContainerChanged OnContainerChanged;

	/** 컨테이너 초기화(슬롯 비우기 포함) */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void InitializeGrid(int32 InWidth, int32 InHeight);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetWidth() const { return Width; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetHeight() const { return Height; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetNumSlots() const { return Slots.Num(); }

	/** 슬롯 내용 읽기 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool GetSlot(int32 Index, FItemInstance& OutItem) const;

	/** 빈 슬롯 찾기(없으면 -1) */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 FindFirstEmptySlot() const;

	/** 아이템 추가(빈 슬롯 자동 탐색). 성공 시 index 반환, 실패 시 -1 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 AddItem(const FItemInstance& Item);

	/** 해당 슬롯 비우기 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveAt(int32 Index);

	/** 슬롯 간 스왑 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool SwapSlots(int32 IndexA, int32 IndexB);

	/** 다른 컨테이너로 이동 (toIndex는 대상 슬롯) */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool MoveTo(UItemContainerComponent* Target, int32 FromIndex, int32 ToIndex);

	// 추가 API 
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool HasItemAt(int32 Index) const;

	/** CanAcceptItem 체크 후 덮어쓰기(빈 값이면 지우기) */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool SetSlotDirect(int32 Index, const FItemInstance& Item);

	/** 전체 비우기 (세션 리셋 때 사용) */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void Clear();

	/** 수용 가능 여부(필요 시 파생 클래스에서 오버라이드) */
	virtual bool CanAcceptItem(const FItemInstance& Item) const { return Item.Def != nullptr; }
	/** 인덱스를 고려한 수용 여부(필요 시 오버라이드) */
	virtual bool CanAcceptItem(const FItemInstance& Item, int32 /*Index*/) const { return CanAcceptItem(Item); }

	/** 아이템별 최대 스택 (필요 시 오버라이드) */
	virtual int32 GetMaxStackForItem(const FItemInstance& /*Item*/) const { return 99; }

	/** 변경 브로드캐스트 */
	void BroadcastChanged() { OnContainerChanged.Broadcast(); }

	const FItemInstance* Peek(int32 Index) const;



protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 Width;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 Height;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FItemInstance> Slots;

	bool IsValidIndex(int32 Index) const { return Slots.IsValidIndex(Index); }
	int32 XYToIndex(int32 X, int32 Y) const { return (Y * Width + X); }
};
