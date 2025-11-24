#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalActor.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class AEchoPlayer;
class ARunWorldDirector;

UENUM(BlueprintType)
enum class EPortalMode : uint8
{
	/** 그냥 TeleportTarget으로 텔레포트 */
	Teleport,

	/** 러닝 시작: 던전(디렉터 Reset + DungeonAnchor 텔레포트 + 20분 타이머) */
	StartDungeon,

	/** 러닝 진행: 보스방(디렉터 Reset + BossAnchor 텔레포트 + 10분 타이머) */
	EnterBoss,

	/** 러닝 종료: 로비로 추출(디렉터 Reset + LobbyAnchor 텔레포트) */
	ExtractToLobby
};

/**
 * 상호작용 포탈
 * - 상호작용 시작 후 FocusDuration 동안 '정신집중'
 * - 집중 중 플레이어가 이동 입력을 주면 취소
 * - 성공 시 모드에 따라 RunSessionSubsystem 호출 또는 Teleport
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPortalFocusEvent);

UCLASS()
class ECHOOFMIND_API APortalActor : public AActor
{
	GENERATED_BODY()

public:
	APortalActor();

	/** 라인트레이스로 맞춘 뒤 플레이어가 호출 (상호작용 시작) */
	UFUNCTION(BlueprintCallable, Category = "Portal")
	void Interact(AEchoPlayer* Player);

	/** UI에서 게이지용 진행도 (0~1) */
	UFUNCTION(BlueprintPure, Category = "Portal")
	float GetFocusProgress() const { return FocusDuration > 0.f ? FMath::Clamp(FocusElapsed / FocusDuration, 0.f, 1.f) : 0.f; }

protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	/** 집중 시작/취소/완료 */
	void StartFocus(AEchoPlayer* Player);
	void CancelFocus(const TCHAR* Reason);
	void FinishFocus();

	/** Teleport 모드에서만 사용 */
	void DoSimpleTeleport();

	/** 세션/디렉터 기반 모드 실행 */
	void DoStartDungeon();
	void DoEnterBoss();
	void DoExtractToLobby();

protected:
	/** 포탈 시각화용 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
	UStaticMeshComponent* Mesh;

	/** (옵션) 사용 범위 박스 콜리전 – 라인트레이스용 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
	UBoxComponent* UseBox;

	/** 포탈 동작 모드 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal")
	EPortalMode Mode = EPortalMode::Teleport;

	/** 몇 초 동안 집중해야 하는가 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal")
	float FocusDuration = 3.0f;

	/** 이동 입력 감지 임계값 (눌리기만 해도 0이 아님) */
	UPROPERTY(EditAnywhere, Category = "Portal")
	float MoveInputCancelThreshold = 0.05f;

	/** Teleport 모드에서 목적지 (레벨에 놓은 타겟 액터/타겟 포인트 등) */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Portal|Teleport")
	AActor* TeleportTarget = nullptr;

	/** 목적지의 회전을 그대로 적용할지 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal|Teleport")
	bool bUseTargetRotation = true;

	/** 성공 시 플레이어 위치 오프셋(문 앞 살짝 앞으로 내보내기 등) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal|Teleport")
	FVector TeleportOffset = FVector::ZeroVector;

	/** 디렉터(맵에 1개). 비워두면 BeginPlay에서 자동 탐색 */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Portal|Run")
	ARunWorldDirector* WorldDirector = nullptr;

	/** 던전/보스 타이머(분) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal|Run")
	float DungeonMinutes = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal|Run")
	float BossMinutes = 5.f;

	/** 내부 상태 */
	UPROPERTY() AEchoPlayer* FocusingPlayer = nullptr;
	bool  bIsFocusing = false;
	float FocusElapsed = 0.f;

public:
	/** UI용 이벤트 */
	UPROPERTY(BlueprintAssignable, Category = "Portal|Event")
	FPortalFocusEvent OnFocusStarted;

	UPROPERTY(BlueprintAssignable, Category = "Portal|Event")
	FPortalFocusEvent OnFocusCanceled;

	UPROPERTY(BlueprintAssignable, Category = "Portal|Event")
	FPortalFocusEvent OnFocusFinished;
};