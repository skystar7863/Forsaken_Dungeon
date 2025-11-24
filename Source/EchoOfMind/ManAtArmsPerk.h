// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perk.h"
#include "ManAtArmsPerk.generated.h"

class AEchoPlayer;

/**
 * 중보병(Man-At-Arms)
 * - 받는 피해 15% 감소
 * - 가하는 피해 10% 감소
 * - 이동 속도 5% 감소
 */
UCLASS(Blueprintable)
class ECHOOFMIND_API UManAtArmsPerk : public UPerk
{
	GENERATED_BODY()

public:
	UManAtArmsPerk();

	virtual void Apply_Implementation(AEchoPlayer* Player) override;

	
};
