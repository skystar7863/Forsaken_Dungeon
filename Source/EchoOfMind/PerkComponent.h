// Perk 컴포넌트: 플레이어에 장착되는 Perk 관리 (슬롯 2개)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PerkComponent.generated.h"

class UPerk;
class UAttributeComponent;

// 슬롯이 열릴 때 UI 등에 알릴 수 있는 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPerkSlotOpened, int32, SlotIndex, int32, Level);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ECHOOFMIND_API UPerkComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPerkComponent();

	// 선택 가능한 Perk 클래스 목록
	UPROPERTY(EditDefaultsOnly, Category = "Perks")
	TArray<TSubclassOf<UPerk>> AvailablePerkClasses;

	// 현재 장착된 Perk 인스턴스들 (슬롯 1~2)
	UPROPERTY(Transient, EditDefaultsOnly, Category = "Perks")
	TArray<UPerk*> ActivePerks;

	// 슬롯 오픈 시 브로드캐스트
	UPROPERTY(BlueprintAssignable, Category = "Perks")
	FOnPerkSlotOpened OnPerkSlotOpened;

	// Perk 장착 (슬롯 번호: 1 또는 2)
	UFUNCTION(BlueprintCallable, Category = "perks")
	void AddPerk(int32 SlotIndex, TSubclassOf<UPerk> PerkClass);

protected:
	virtual void BeginPlay() override;

private:
	// 레벨업 시 슬롯 개방 여부 확인
	UFUNCTION()
	void HandleLevelUp(int32 NewLevel);

	// 슬롯 개방 여부 플래그
	bool bSlot1Opened;
	bool bSlot2Opened;
};
