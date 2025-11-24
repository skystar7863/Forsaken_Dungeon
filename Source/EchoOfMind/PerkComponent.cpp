#include "PerkComponent.h"
#include "Perk.h"
#include "EchoPlayer.h"
#include "AttributeComponent.h"
#include "GameFramework/Actor.h"

UPerkComponent::UPerkComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// 슬롯 잠금 상태 초기화
	bSlot1Opened = true;
	bSlot2Opened = false;
}

void UPerkComponent::BeginPlay()
{
	Super::BeginPlay();

	// AttributeComponent로부터 레벨업 이벤트 바인딩
	if (UAttributeComponent* Attr = GetOwner()->FindComponentByClass<UAttributeComponent>()) {
		Attr->OnLevelUp.AddDynamic(this, &UPerkComponent::HandleLevelUp);
	}
}

void UPerkComponent::AddPerk(int32 SlotIndex, TSubclassOf<UPerk> PerkClass)
{
	// 슬롯 인덱스 유효성 확인 (1 or 2만 가능)
	if (SlotIndex < 1 || SlotIndex > 2) return;

	// 해당 슬롯이 열려 있는지 확인
	bool bOpened = (SlotIndex == 1 ? bSlot1Opened : bSlot2Opened);
	if (!bOpened) return;

	// 기존 Perk 제거
	int32 ArrayIndex = SlotIndex - 1;
	if (ActivePerks.IsValidIndex(ArrayIndex) && ActivePerks[ArrayIndex]) {
		ActivePerks[ArrayIndex]->ConditionalBeginDestroy();
		ActivePerks[ArrayIndex] = nullptr;
	}

	// 새 Perk 생성 및 적용
	if (PerkClass) {
		UPerk* NewPerk = NewObject<UPerk>(this, PerkClass);
		ActivePerks.SetNum(2);
		ActivePerks[ArrayIndex] = NewPerk;

		if (AActor* Owner = GetOwner()) {
			if (AEchoPlayer* Player = Cast<AEchoPlayer>(Owner)) {
				NewPerk->Apply(Player);
			}
		}
	}
}

void UPerkComponent::HandleLevelUp(int32 NewLevel)
{
	// 일정 레벨에 도달 시 슬롯 개방 + 이벤트 발생
	if (NewLevel >= 5 && !bSlot1Opened) {
		bSlot1Opened = true;
		OnPerkSlotOpened.Broadcast(1, NewLevel);
	}
	if (NewLevel >= 10 && !bSlot2Opened) {
		bSlot2Opened = true;
		OnPerkSlotOpened.Broadcast(2, NewLevel);
	}
}
