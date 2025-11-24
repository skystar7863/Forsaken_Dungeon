#include "InventoryRootWidget.h"
#include "Kismet/GameplayStatics.h"
#include "ItemGridWidget.h"
#include "EquipmentSlotWidget.h"
#include "EchoPlayer.h"
#include "PlayerInventoryComponent.h"
#include "EquipmentComponent.h"
#include "EnemyLootComponent.h"
#include "ItemContainerComponent.h"
#include "ItemInstance.h"
#include "LobbyStashComponent.h"
#include "ConsumableContainerComponent.h"
#include "Components/TextBlock.h"
#include "LootChestActor.h"


void UInventoryRootWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// UI 위젯 자체가 포커스를 먹어 드래그를 막지 않도록(필요 시)
	SetIsFocusable(false);

	// 기본 세팅
	SetupWithOwningPlayer();

	// 루트 인벤토리는 항상 히트 가능하게
	if (BackpackGrid) { BackpackGrid->SetVisibility(ESlateVisibility::Visible);        BackpackGrid->SetIsEnabled(true); }
	if (ConsumableGrid) { ConsumableGrid->SetVisibility(ESlateVisibility::Visible);      ConsumableGrid->SetIsEnabled(true); }

	// 전리품/스태시는 기본 닫힘 (레이아웃에서도 제거: Collapsed)
	if (LootGrid)            LootGrid->SetVisibility(ESlateVisibility::Collapsed);
	if (LootTitleText)       LootTitleText->SetVisibility(ESlateVisibility::Collapsed);
	if (LobbyStashGrid)      LobbyStashGrid->SetVisibility(ESlateVisibility::Collapsed);
	if (LobbyStashTitleText) LobbyStashTitleText->SetVisibility(ESlateVisibility::Collapsed);

	if (ItemTooltipClass && !ItemTooltip)
	{
		ItemTooltip = CreateWidget<UItemTooltipWidget>(GetWorld(), ItemTooltipClass);
		if (ItemTooltip)
		{
			ItemTooltip->AddToViewport(/*ZOrder*/ 3000);
			ItemTooltip->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	if (BackpackGrid)
		BackpackGrid->OnSlotClicked.AddUObject(this, &UInventoryRootWidget::HandleBackpackRightClick);

	if (LootGrid)
		LootGrid->OnSlotClicked.AddUObject(this, &UInventoryRootWidget::HandleLootRightClick);

	if (LobbyStashGrid)
		LobbyStashGrid->OnSlotClicked.AddUObject(this, &UInventoryRootWidget::HandleStashRightClick);
}

void UInventoryRootWidget::ShowItemTooltip(const FItemInstance Instance)
{
	if (!ItemTooltip) return;

	ItemTooltip->SetItem(Instance);
	// 왼쪽 상단 고정: 20,20
	ItemTooltip->SetAlignmentInViewport(FVector2D(0.f, 0.f));
	ItemTooltip->SetPositionInViewport(FVector2D(20.f, 20.f), false);
	ItemTooltip->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UInventoryRootWidget::HideItemTooltip()
{
	if (ItemTooltip)
		ItemTooltip->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryRootWidget::SetupWithOwningPlayer()
{
	APawn* Pawn = GetOwningPlayerPawn();
	PlayerRef = Pawn ? Cast<AEchoPlayer>(Pawn) : nullptr;
	if (!PlayerRef) return;

	InvRef = PlayerRef->FindComponentByClass<UPlayerInventoryComponent>();
	EquipRef = PlayerRef->FindComponentByClass<UEquipmentComponent>();

	// 가방/소모품 그리드 연결
	if (InvRef)
	{
		if (BackpackGrid) { BackpackGrid->InitializeWithContainer(InvRef->Backpack);   BackpackGrid->SetVisibility(ESlateVisibility::Visible);   BackpackGrid->SetIsEnabled(true); }
		if (ConsumableGrid) { ConsumableGrid->InitializeWithContainer(InvRef->Consumables); ConsumableGrid->SetVisibility(ESlateVisibility::Visible); ConsumableGrid->SetIsEnabled(true); }
	}

	// 장비 슬롯 연결 + 우클릭 해제 컨테이너 지정
	if (EquipRef)
	{
		if (EquipSlot_Weapon) { EquipSlot_Weapon->InitializeEquipment(EquipRef); EquipSlot_Weapon->TargetUnequipContainer = InvRef ? InvRef->Backpack : nullptr; }
		if (EquipSlot_Helmet) { EquipSlot_Helmet->InitializeEquipment(EquipRef); EquipSlot_Helmet->TargetUnequipContainer = InvRef ? InvRef->Backpack : nullptr; }
		if (EquipSlot_Chest) { EquipSlot_Chest->InitializeEquipment(EquipRef);  EquipSlot_Chest->TargetUnequipContainer = InvRef ? InvRef->Backpack : nullptr; }
		if (EquipSlot_Leg) { EquipSlot_Leg->InitializeEquipment(EquipRef);    EquipSlot_Leg->TargetUnequipContainer = InvRef ? InvRef->Backpack : nullptr; }
		if (EquipSlot_Boots) { EquipSlot_Boots->InitializeEquipment(EquipRef);  EquipSlot_Boots->TargetUnequipContainer = InvRef ? InvRef->Backpack : nullptr; }

		EquipRef->OnEquipmentChanged.AddDynamic(this, &UInventoryRootWidget::OnEquipmentChanged);
	}
}

void UInventoryRootWidget::OnEquipmentChanged()
{
	if (EquipSlot_Weapon) EquipSlot_Weapon->Refresh();
	if (EquipSlot_Helmet) EquipSlot_Helmet->Refresh();
	if (EquipSlot_Chest)  EquipSlot_Chest->Refresh();
	if (EquipSlot_Leg)    EquipSlot_Leg->Refresh();
	if (EquipSlot_Boots)  EquipSlot_Boots->Refresh();
}

// ───────────────────── 전리품 ─────────────────────
void UInventoryRootWidget::OpenLoot(UEnemyLootComponent* LootComp, ALootChestActor* ChestOwner /*=nullptr*/)
{
	// 로비 스태시는 닫고 전리품만 표시
	CloseLobbyStash();

	CurrentLoot = LootComp;
	CurrentChest = ChestOwner;

	if (LootGrid && LootComp)
	{
		LootGrid->InitializeWithContainer(LootComp);
		LootGrid->SetVisibility(ESlateVisibility::Visible);
	}
	if (LootTitleText)
	{
		LootTitleText->SetVisibility(ESlateVisibility::Visible);
	}

	// 루트 인벤토리는 계속 상호작용 가능 상태 유지
	if (BackpackGrid) { BackpackGrid->SetVisibility(ESlateVisibility::Visible);   BackpackGrid->SetIsEnabled(true); }
	if (ConsumableGrid) { ConsumableGrid->SetVisibility(ESlateVisibility::Visible); ConsumableGrid->SetIsEnabled(true); }
}

void UInventoryRootWidget::CloseLoot()
{
	if (LootGrid)      LootGrid->SetVisibility(ESlateVisibility::Collapsed);
	if (LootTitleText) LootTitleText->SetVisibility(ESlateVisibility::Collapsed);

	CurrentChest = nullptr;
	CurrentLoot = nullptr;
}

// ───────────────────── 로비 스태시 ─────────────────────
void UInventoryRootWidget::OpenLobbyStash(ULobbyStashComponent* InStash)
{
	// 전리품은 닫고 스태시만 표시
	CloseLoot();

	StashRef = InStash;

	if (LobbyStashGrid && StashRef)
	{
		LobbyStashGrid->InitializeWithContainer(StashRef);
		LobbyStashGrid->SetVisibility(ESlateVisibility::Visible);
	}
	if (LobbyStashTitleText)
	{
		LobbyStashTitleText->SetVisibility(ESlateVisibility::Visible);
	}

	// 루트 인벤토리는 계속 상호작용 가능 상태 유지
	if (BackpackGrid) { BackpackGrid->SetVisibility(ESlateVisibility::Visible);   BackpackGrid->SetIsEnabled(true); }
	if (ConsumableGrid) { ConsumableGrid->SetVisibility(ESlateVisibility::Visible); ConsumableGrid->SetIsEnabled(true); }
}

void UInventoryRootWidget::CloseLobbyStash()
{
	if (LobbyStashGrid)      LobbyStashGrid->SetVisibility(ESlateVisibility::Collapsed);
	if (LobbyStashTitleText) LobbyStashTitleText->SetVisibility(ESlateVisibility::Collapsed);
	StashRef = nullptr;
}

// ───────────────────── 인벤토리만 ─────────────────────
void UInventoryRootWidget::ShowInventoryOnly()
{
	// 전리품/스태시를 전부 Collapsed로 닫아 레이아웃/히트테스트에서 제거
	if (LootGrid)            LootGrid->SetVisibility(ESlateVisibility::Collapsed);
	if (LootTitleText)       LootTitleText->SetVisibility(ESlateVisibility::Collapsed);
	if (LobbyStashGrid)      LobbyStashGrid->SetVisibility(ESlateVisibility::Collapsed);
	if (LobbyStashTitleText) LobbyStashTitleText->SetVisibility(ESlateVisibility::Collapsed);

	// 인벤토리는 명시적으로 활성화
	if (BackpackGrid) { BackpackGrid->SetVisibility(ESlateVisibility::Visible);   BackpackGrid->SetIsEnabled(true); }
	if (ConsumableGrid) { ConsumableGrid->SetVisibility(ESlateVisibility::Visible); ConsumableGrid->SetIsEnabled(true); }
}


// 🔸 Loot/Stash → Backpack
static bool MoveAnyToBackpack(UItemContainerComponent* From, UItemContainerComponent* Backpack, int32 FromIndex)
{
	if (!From || !Backpack) return false;
	if (!From->HasItemAt(FromIndex)) return false;

	const int32 ToIndex = Backpack->FindFirstEmptySlot();
	if (ToIndex == INDEX_NONE) return false;

	return From->MoveTo(Backpack, FromIndex, ToIndex);
}

// 🔸 Backpack → 열린 Loot(or Stash)
static bool MoveBackpackToTarget(UItemContainerComponent* Backpack, UItemContainerComponent* Target, int32 FromIndex)
{
	if (!Backpack || !Target) return false;
	if (!Backpack->HasItemAt(FromIndex)) return false;

	const int32 ToIndex = Target->FindFirstEmptySlot();
	if (ToIndex == INDEX_NONE) return false;

	return Backpack->MoveTo(Target, FromIndex, ToIndex);
}

// ▷ Backpack 슬롯 우클릭: 열린 Loot가 우선, 없으면 Stash로 이동
void UInventoryRootWidget::HandleBackpackRightClick(int32 Index)
{
	if (!InvRef || !InvRef->Backpack) return;

	// 열린 우선순위: Loot → Stash
	if (CurrentLoot)
	{
		MoveBackpackToTarget(InvRef->Backpack, CurrentLoot, Index);
	}
	else if (StashRef)
	{
		MoveBackpackToTarget(InvRef->Backpack, StashRef, Index);
	}
	// 둘 다 없으면 아무 것도 안 함
}

// ▷ Loot 슬롯 우클릭: Backpack로 이동
void UInventoryRootWidget::HandleLootRightClick(int32 Index)
{
	if (!InvRef || !InvRef->Backpack || !CurrentLoot) return;
	MoveAnyToBackpack(CurrentLoot, InvRef->Backpack, Index);
}

// ▷ Stash 슬롯 우클릭: Backpack로 이동
void UInventoryRootWidget::HandleStashRightClick(int32 Index)
{
	if (!InvRef || !InvRef->Backpack || !StashRef) return;
	MoveAnyToBackpack(StashRef, InvRef->Backpack, Index);
}
