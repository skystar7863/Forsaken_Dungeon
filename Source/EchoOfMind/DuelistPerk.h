// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perk.h"
#include "DuelistPerk.generated.h"


class AEchoPlayer;
class UAttributeComponent;
class UCharacterMovementComponent;

UCLASS()
class ECHOOFMIND_API UDuelistPerk : public UPerk
{
	GENERATED_BODY()

public:
	UDuelistPerk();

	virtual void Apply_Implementation(AEchoPlayer* Player) override;

private:
    // 데미지 입으면 호출될 함수
    UFUNCTION()
    void OnOwnerTakeDamage(float DamageAmount);

    // 디버프 해제용 타이머
    FTimerHandle TimerHandle_RemoveDebuff;

    // 원래·버프 후 속도 저장
    float BuffedSpeed;
    TWeakObjectPtr<AEchoPlayer> OwnerPlayer;

    // 디버프 끝나면 호출
    void RestoreSpeed();
	
};
