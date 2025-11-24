// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perk.h"
#include "WarmongerPerk.generated.h"

/**
 * 전투광(Warmonger)
 * - 가하는 피해 +5%
 * - 받는 피해 +5%
 */

class AEchoPlayer;
class UAttributeComponent;

UCLASS(Blueprintable)
class ECHOOFMIND_API UWarmongerPerk : public UPerk
{
	GENERATED_BODY()

public:
	UWarmongerPerk();

	virtual void Apply_Implementation(AEchoPlayer* Player) override;
	
};
