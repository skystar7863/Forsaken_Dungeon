#include "EquipmentComponent.h"
#include "ItemContainerComponent.h"
#include "AttributeComponent.h"
#include "Item/ItemMath.h"
#include "EchoPlayer.h"
#include "ItemDefinition.h"
#include "GameFramework/CharacterMovementComponent.h"

UEquipmentComponent::UEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	if (AEchoPlayer* P = Cast<AEchoPlayer>(GetOwner()))
	{
		OwnerPlayer = P;
		Attr = P->FindComponentByClass<UAttributeComponent>();
	}
	AppliedTotalStats = FItemStats{}; // 0으로
}

bool UEquipmentComponent::IsTypeAllowedForSlot(const FItemInstance& Item, EEquipSlot Slot) const
{
	if (!Item.Def) return false;

	switch (Slot)
	{
	case EEquipSlot::Weapon:
		return (Item.Def->ItemType == EItemType::Weapon_Sword || Item.Def->ItemType == EItemType::Weapon_Staff);
	case EEquipSlot::Helmet:
		return Item.Def->ItemType == EItemType::Helmet;
	case EEquipSlot::Chest:
		return Item.Def->ItemType == EItemType::Chest;
	case EEquipSlot::Leg:
		return Item.Def->ItemType == EItemType::Leg;
	case EEquipSlot::Boots:
		return Item.Def->ItemType == EItemType::Boots;
	default:
		return false;
	}
}

bool UEquipmentComponent::EquipFrom(UItemContainerComponent* FromContainer, int32 FromIndex, EEquipSlot Slot)
{
	if (!FromContainer) return false;

	FItemInstance InItem;
	if (!FromContainer->GetSlot(FromIndex, InItem) || InItem.Def == nullptr)
		return false;
	if (!IsTypeAllowedForSlot(InItem, Slot))
		return false;

	const bool bHadOld = (Equipped.Contains(Slot) && Equipped[Slot].Def != nullptr);
	FItemInstance OldItem = bHadOld ? Equipped[Slot] : FItemInstance{};

	// 1) 장착 교체
	Equipped.Add(Slot, InItem);

	// 2) 소스 컨테이너 처리 (스왑/대피)
	if (bHadOld)
	{
		if (!FromContainer->SetSlotDirect(FromIndex, OldItem))
		{
			const int32 Free = FromContainer->FindFirstEmptySlot();
			if (Free == INDEX_NONE)
			{
				// 되돌리기
				Equipped[Slot] = OldItem;
				return false;
			}
			FromContainer->RemoveAt(FromIndex);
			FromContainer->SetSlotDirect(Free, OldItem);
		}
	}
	else
	{
		FromContainer->RemoveAt(FromIndex);
	}

	// 3) 어트리뷰트/이속 재적용 & 갱신 이벤트
	RecalculateAndApply();
	OnEquipmentChanged.Broadcast();
	FromContainer->BroadcastChanged();
	return true;
}

bool UEquipmentComponent::UnequipTo(UItemContainerComponent* ToContainer, EEquipSlot Slot)
{
	if (!ToContainer) return false;
	if (!Equipped.Contains(Slot)) return false;

	FItemInstance Item = Equipped[Slot];
	if (Item.Def == nullptr) return false;

	int32 idx = ToContainer->AddItem(Item);
	if (idx == INDEX_NONE) return false;

	Equipped[Slot] = FItemInstance{};
	RecalculateAndApply();
	OnEquipmentChanged.Broadcast();
	return true;
}

FItemStats UEquipmentComponent::GetTotalEquippedStats() const
{
	FItemStats Total{}; // 전부 0으로 시작

	for (const auto& KV : Equipped)
	{
		const FItemInstance& Inst = KV.Value;
		if (!Inst.Def) continue;

		// 희귀도 배율 + 추가옵션까지 반영된 최종 스탯
		const FItemStats Final = ItemMath::ComputeFinalStats(Inst);

		Total.Strength += Final.Strength;
		// Total.Agility     += Final.Agility;   // 민첩 사용 중이면 해제
		Total.Intelligence += Final.Intelligence;
		Total.Vitality += Final.Vitality;
		Total.Defense += Final.Defense;
		Total.MoveSpeedPct += Final.MoveSpeedPct;
	}

	return Total;
}


void UEquipmentComponent::RecalculateAndApply()
{
	// 1) 새 총합 계산
	FItemStats NewTotal{};
	for (auto& KV : Equipped)
	{
		if (KV.Value.Def)
		{
			FItemStats S = ItemMath::ComputeFinalStats(KV.Value);
			NewTotal.Strength += S.Strength;
			//NewTotal.Agility      += S.Agility;        // ? 민첩은 비활성화
			NewTotal.Intelligence += S.Intelligence;
			NewTotal.Vitality += S.Vitality;
			NewTotal.Defense += S.Defense;
			NewTotal.MoveSpeedPct += S.MoveSpeedPct;
		}
	}

	// 2) Delta = New - Applied (Agility는 무시)
	FItemStats Delta;
	Delta.Strength = NewTotal.Strength - AppliedTotalStats.Strength;
	Delta.Agility = 0; //NewTotal.Agility - AppliedTotalStats.Agility;
	Delta.Intelligence = NewTotal.Intelligence - AppliedTotalStats.Intelligence;
	Delta.Vitality = NewTotal.Vitality - AppliedTotalStats.Vitality;
	Delta.Defense = NewTotal.Defense - AppliedTotalStats.Defense;
	Delta.MoveSpeedPct = NewTotal.MoveSpeedPct - AppliedTotalStats.MoveSpeedPct;

	// 3) 어트리뷰트/이속 반영
	ApplyDeltaToAttributes(Delta);
	ReapplyMoveSpeed(NewTotal.MoveSpeedPct);

	AppliedTotalStats = NewTotal;
}

void UEquipmentComponent::ApplyDeltaToAttributes(const FItemStats& D)
{
	if (!Attr.IsValid()) return;

	// 장비로 증가/감소된 주 스탯 가산
	//Attr->Strength += D.Strength;
	/* Attr->Agility   += D.Agility;*/   // 민첩 비활성화
	Attr->Intelligence += D.Intelligence;
	Attr->Vitality += D.Vitality;
	Attr->Defense += D.Defense;

	 //1️⃣ 힘 → 공격력 증가
	if (D.Strength != 0)
	{
		Attr->ModifyStrength(static_cast<int32>(D.Strength));
		//Attr->ModifyStrength(D.Strength);
	}

	// 2️⃣ 활력 → 체력 증가
	/*if (D.Vitality != 0)
	{
		Attr->ModifyVitality(static_cast<int32>(D.Vitality));
		Attr->ModifyMaxHP(D.Vitality * 10.f);
	}*/

	//// 3️⃣ 지능은 추후 마력 공격력용으로 확장 가능
	//if (D.Intelligence != 0)
	//{
	//	Attr->ModifyIntelligence(static_cast<int32>(D.Intelligence));
	//}

	// 4️⃣ 방어력
	/*if (D.Defense != 0)
	{
		Attr->ModifyDefense(D.Defense);
	}*/

	// 주 스탯 기반 파생값 재산출(비율 유지)
	Attr->RecalculateFromPrimary(true);
	// Health/Mana 브로드캐스트는 RecalculateFromPrimary 안에서 호출됨
}

void UEquipmentComponent::ReapplyMoveSpeed(float TotalMoveSpeedPct)
{
	if (!OwnerPlayer.IsValid()) return;
	if (auto* Move = OwnerPlayer->GetCharacterMovement())
	{
		// 기준속도: 프로젝트 기본 보행 속도로 교체해도 됨(예: 600)
		const float Base = 600.f;
		const float Mult = 1.f + (TotalMoveSpeedPct * 0.01f);
		Move->MaxWalkSpeed = Base * Mult;
	}
}
