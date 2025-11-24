// Fill out your copyright notice in the Description page of Project Settings.


#include "HasteSkill.h"
#include "EchoPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"

UHasteSkill::UHasteSkill()
{
	SkillName = FName(TEXT("Haste"));
	Cooldown = 240.f; // 4 minutes
	Duration = 20.f;
	SpeedMultiplier = 1.2f; // +20%
	LastUseTime = -1000.f;
}

void UHasteSkill::Execute_Implementation(AEchoPlayer* Player)
{
	if (!Player) return;

	OwnerPlayer = Player;

	if (auto* MoveComp = Player->GetCharacterMovement()) {
		// 원래 속도 저장
		OriginalSpeed = MoveComp->MaxWalkSpeed;
		// 속도 증가
		MoveComp->MaxWalkSpeed = OriginalSpeed * SpeedMultiplier;
		// 2) 타이머 설정: Duration 후 EndHaste 호출
		Player->GetWorld()->GetTimerManager().SetTimer(TimerHandle_EndHaste, this, &UHasteSkill::EndHaste, Duration, false);
	}
}

void UHasteSkill::EndHaste()
{
	if (!OwnerPlayer.IsValid()) return;

	if (auto* MoveComp = OwnerPlayer->GetCharacterMovement()) {
		// 원래 속도로 되돌리기
		MoveComp->MaxWalkSpeed = OriginalSpeed;
	}
}
