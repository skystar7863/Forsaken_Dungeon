// Fill out your copyright notice in the Description page of Project Settings.


#include "ManAtArmsPerk.h"
#include "EchoPlayer.h"
#include "AttributeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UManAtArmsPerk::UManAtArmsPerk()
{

	PerkName = FName("Man-At-Arms");
	Description = FText::FromString("Reduces incoming damage by 15%, outgoing damage by 10%, and movement speed by 5%.");
}

void UManAtArmsPerk::Apply_Implementation(AEchoPlayer* Player)
{
	if (!Player) return;

	if (UAttributeComponent* Attr = Player->FindComponentByClass<UAttributeComponent>()) {
		Attr->IncomingDamageMultiplier *= 0.85f; // Reduce incoming damage by 15%
		Attr->OutgoingDamageMultiplier *= 0.90f; // Reduce outgoing damage by 10%
	}

	if (auto* MoveComp = Player->GetCharacterMovement()) {
		MoveComp->MaxWalkSpeed *= 0.95f; // Reduce movement speed by 5%
	}
}
