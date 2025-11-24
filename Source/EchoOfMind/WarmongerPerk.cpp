// Fill out your copyright notice in the Description page of Project Settings.


#include "WarmongerPerk.h"
#include "EchoPlayer.h"
#include "AttributeComponent.h"

UWarmongerPerk::UWarmongerPerk()
{
	PerkName = FName(TEXT("Warmonger"));
	Description = FText::FromString(TEXT("Increases damage dealt and received by 5%"));
}

void UWarmongerPerk::Apply_Implementation(AEchoPlayer* Player)
{
	if (!Player) return;

	if (auto* Attr = Player->FindComponentByClass<UAttributeComponent>()) {
		Attr->OutgoingDamageMultiplier += 0.05f; // 가하는 피해 +5%
		Attr->IncomingDamageMultiplier += 0.05f; // 받는 피해 +5%
	}
}
