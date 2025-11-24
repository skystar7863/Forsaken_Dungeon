#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/ItemInstance.h"
#include "Item/ItemEnums.h"
#include "Item/ItemStats.h"
#include "EquipmentComponent.generated.h"

class UAttributeComponent;
class UItemContainerComponent;
class AEchoPlayer;

UENUM(BlueprintType)
enum class EEquipSlot : uint8
{
	Weapon,
	Helmet,
	Chest,
	Leg,
	Boots
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipmentChanged);

/**
 * 장비 컴포넌트
 * - 슬롯별 장착
 * - 총합 스탯 계산→어트리뷰트 반영
 * - 이속%는 MoveComp에 반영
 */
UCLASS(ClassGroup = (Inventory), meta = (BlueprintSpawnableComponent))
class ECHOOFMIND_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEquipmentComponent();

	UPROPERTY(BlueprintAssignable, Category = "Equipment")
	FOnEquipmentChanged OnEquipmentChanged;

	/** 현재 장착 아이템 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TMap<EEquipSlot, FItemInstance> Equipped;

	/** 인벤토리 컨테이너에서 장착(그리드 인덱스) */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool EquipFrom(UItemContainerComponent* FromContainer, int32 FromIndex, EEquipSlot Slot);

	/** 장비를 컨테이너로 해제(반납) */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool UnequipTo(UItemContainerComponent* ToContainer, EEquipSlot Slot);
	// ✅ 현재 장착 중인 모든 아이템의 최종 합산 스탯(희귀도/추가옵션 적용 후)을 반환
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	FItemStats GetTotalEquippedStats() const;

protected:
	virtual void BeginPlay() override;

private:
	TWeakObjectPtr<AEchoPlayer> OwnerPlayer;
	TWeakObjectPtr<UAttributeComponent> Attr;

	/** 현재 적용된 합산 스탯(Delta 계산용) */
	FItemStats AppliedTotalStats;

	/** 장착 변경 후 총합 스탯 재계산 → Attribute/이속 반영 */
	void RecalculateAndApply();

	/** 슬롯과 아이템 타입 호환 검사 */
	bool IsTypeAllowedForSlot(const FItemInstance& Item, EEquipSlot Slot) const;

	/** Attribute 에 Delta 적용(+/-) */
	void ApplyDeltaToAttributes(const FItemStats& Delta);

	/** 이동속도 재계산 */
	void ReapplyMoveSpeed(float TotalMoveSpeedPct);
};
