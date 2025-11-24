#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RunSessionSubsystem.generated.h"

class ARunWorldDirector;
class AEchoPlayer;

UENUM(BlueprintType)
enum class ERunPhase : uint8
{
	Lobby,
	Dungeon,
	Boss,
	Ended
};

UENUM(BlueprintType)
enum class ERunEndReason : uint8
{
	None,
	ExtractedToLobby,
	TimeOut,
	PlayerDead
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRunPhaseChanged, ERunPhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRunTimerTick);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRunEnded, ERunEndReason, Reason);

/** 같은 레벨에서 포탈로 구역만 바꾸는 세션 매니저 */
UCLASS()
class ECHOOFMIND_API URunSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** 현재 러닝 상태 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Run")
	ERunPhase Phase = ERunPhase::Lobby;

	/** 타이머 마감 절대시각(초) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Run")
	double DeadlineSeconds = 0.0;

	/** 델리게이트 */
	UPROPERTY(BlueprintAssignable) FOnRunPhaseChanged OnPhaseChanged;
	UPROPERTY(BlueprintAssignable) FOnRunTimerTick   OnRunTimerTick;
	UPROPERTY(BlueprintAssignable) FOnRunEnded       OnRunEnded;

public:
	/** 던전 러닝 시작(20분 기본) */
	UFUNCTION(BlueprintCallable, Category = "Run")
	void StartDungeonRunInPlace(ARunWorldDirector* Dir, float Minutes = 20.f);

	/** 보스방 입장(10분 기본) */
	UFUNCTION(BlueprintCallable, Category = "Run")
	void EnterBossRoomInPlace(ARunWorldDirector* Dir, float Minutes = 10.f);

	/** 로비로 추출(성공 종료) */
	UFUNCTION(BlueprintCallable, Category = "Run")
	void ExtractToLobbyInPlace(ARunWorldDirector* Dir);

	/** 실패(시간초과/사망) */
	UFUNCTION(BlueprintCallable, Category = "Run")
	void FailRunInPlace(ARunWorldDirector* Dir, ERunEndReason Reason);

	/** 남은 시간(초) */
	UFUNCTION(BlueprintCallable, Category = "Run")
	int32 GetRemainingSeconds() const;

	/** 매 프레임 틱(디렉터에서 호출) */
	void TickRun(float DeltaSeconds);

	/** 월드가 준비됨(디렉터 BeginPlay에서 호출) */
	void NotifyWorldReady();

	/** ✅ 러닝 상태/타이머 초기화(디렉터 리셋 이후 호출) */
	UFUNCTION(BlueprintCallable, Category = "Run")
	void ResetRunTimerAndPhase();

private:
	void SetTimerMinutes(float Minutes);
	AEchoPlayer* GetPlayer(UWorld* World) const;
	AEchoPlayer* GetPlayer() const;
};
