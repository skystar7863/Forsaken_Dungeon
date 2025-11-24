#include "RunSessionSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "RunWorldDirector.h"
#include "EngineUtils.h"
#include "SkillComponent.h" // ✅ 쿨타임 리셋용
#include "EchoPlayer.h"
#include "AttributeComponent.h" // ✅ 타임아웃 시 사망 처리용

void URunSessionSubsystem::SetTimerMinutes(float Minutes)
{
	const double Now = FPlatformTime::Seconds();
	DeadlineSeconds = Now + (double)Minutes * 60.0;
}

int32 URunSessionSubsystem::GetRemainingSeconds() const
{
	const double Now = FPlatformTime::Seconds();
	const double Remain = FMath::Max(0.0, DeadlineSeconds - Now);
	return (int32)FMath::RoundHalfFromZero(Remain);
}

void URunSessionSubsystem::TickRun(float /*DeltaSeconds*/)
{
	if (Phase == ERunPhase::Dungeon || Phase == ERunPhase::Boss)
	{
		OnRunTimerTick.Broadcast();

		if (GetRemainingSeconds() <= 0)
		{
			// 타임아웃 → Fail 처리 (아래에서 '사망'으로 유도)
			FailRunInPlace(nullptr, ERunEndReason::TimeOut);
		}
	}
}

AEchoPlayer* URunSessionSubsystem::GetPlayer(UWorld* World) const
{
	if (!World) return nullptr;
	return Cast<AEchoPlayer>(UGameplayStatics::GetPlayerPawn(World, 0));
}

void URunSessionSubsystem::StartDungeonRunInPlace(ARunWorldDirector* Dir, float Minutes)
{
	if (!Dir) return;
	Phase = ERunPhase::Dungeon;
	OnPhaseChanged.Broadcast(Phase);

	Dir->ResetMapFromSnapshot();
	Dir->TeleportPlayerToDungeon();
	if (AEchoPlayer* P = GetPlayer())
	{
		if (USkillComponent* SK = P->FindComponentByClass<USkillComponent>())
		{
			SK->ResetAllCooldowns();
		}
	}

	SetTimerMinutes(Minutes);
}

void URunSessionSubsystem::EnterBossRoomInPlace(ARunWorldDirector* Dir, float Minutes)
{
	if (!Dir) return;
	Phase = ERunPhase::Boss;
	OnPhaseChanged.Broadcast(Phase);

	Dir->ResetMapFromSnapshot();
	Dir->TeleportPlayerToBoss();

	SetTimerMinutes(Minutes);
}

void URunSessionSubsystem::ExtractToLobbyInPlace(ARunWorldDirector* Dir)
{
	if (!Dir) return;
	Phase = ERunPhase::Lobby;
	OnPhaseChanged.Broadcast(Phase);

	OnRunEnded.Broadcast(ERunEndReason::ExtractedToLobby);

	Dir->ResetMapFromSnapshot();
	Dir->TeleportPlayerToLobby();
}

void URunSessionSubsystem::FailRunInPlace(ARunWorldDirector* Dir, ERunEndReason Reason)
{
	// ⬇️ 타임아웃은 "사망"을 유도해서 디렉터의 ResetRunSession 흐름을 태운다.
	if (Reason == ERunEndReason::TimeOut)
	{
		if (UWorld* World = GetWorld())
		{
			if (AEchoPlayer* P = GetPlayer(World))
			{
				if (UAttributeComponent* Attr = P->FindComponentByClass<UAttributeComponent>())
				{
					// 큰 피해로 즉사 → OnDeath 브로드캐스트 → ARunWorldDirector::OnPlayerDied() → ResetRunSession()
					Attr->TakeDamage(1e9f);
					return; // 여기서 더 이상의 직접 TP/리셋은 하지 않음
				}
			}
		}
		// 혹시 위가 실패했다면(안전망): 기존 로직으로 폴백
	}

	// 타임아웃 외(혹은 폴백) 기본 처리: 즉시 로비로
	Phase = ERunPhase::Ended;
	OnPhaseChanged.Broadcast(Phase);
	OnRunEnded.Broadcast(Reason);

	if (Dir)
	{
		Dir->ResetMapFromSnapshot();
		Dir->TeleportPlayerToLobby();
	}
	else if (UWorld* World = GetWorld())
	{
		for (TActorIterator<ARunWorldDirector> It(World); It; ++It)
		{
			ARunWorldDirector* DirFound = *It;
			if (DirFound)
			{
				DirFound->ResetMapFromSnapshot();
				DirFound->TeleportPlayerToLobby();
				break;
			}
		}
	}
}

void URunSessionSubsystem::NotifyWorldReady()
{
	// 필요 시 UI 초기화 등
}

void URunSessionSubsystem::ResetRunTimerAndPhase()
{
	DeadlineSeconds = 0.0;
	Phase = ERunPhase::Lobby;
	OnPhaseChanged.Broadcast(Phase);

	// 즉시 UI 갱신을 원하면 호출
	OnRunTimerTick.Broadcast();
}

AEchoPlayer* URunSessionSubsystem::GetPlayer() const
{
	if (UWorld* World = GetWorld())
	{
		return Cast<AEchoPlayer>(UGameplayStatics::GetPlayerPawn(World, 0));
	}
	return nullptr;
}