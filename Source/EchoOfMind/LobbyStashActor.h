#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LobbyStashActor.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class AEchoPlayer;
class ARunWorldDirector;
class USoundBase; // ✅ 추가

/** 로비 스태시 상자: 전리품 생성 없이 Director의 Stash를 연다 */
UCLASS()
class ECHOOFMIND_API ALobbyStashActor : public AActor
{
	GENERATED_BODY()
public:
	ALobbyStashActor();

	/** 라인트레이스 F키 상호작용 */
	UFUNCTION(BlueprintCallable, Category = "Stash")
	void Interact(AEchoPlayer* Player);

	// ✅ 상자 열릴 때 재생할 사운드
	UPROPERTY(EditAnywhere, Category = "Chest|Sound")
	USoundBase* ChestOpenSound;

protected:
	virtual void Tick(float DeltaSeconds) override;

	// ── Components ─────────────────────────────────────────
	UPROPERTY(VisibleAnywhere, Category = "Stash|Comp")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, Category = "Stash|Comp")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, Category = "Stash|Comp")
	USceneComponent* LidPivot;

	UPROPERTY(VisibleAnywhere, Category = "Stash|Comp")
	UStaticMeshComponent* LidMesh;

	// ── Settings ───────────────────────────────────────────
	/** 덮개가 오픈될 각도(기본 Pitch -30˚) */
	UPROPERTY(EditAnywhere, Category = "Stash|Anim")
	float OpenAngle = -30.f;

	/** 열리는 시간(초) */
	UPROPERTY(EditAnywhere, Category = "Stash|Anim")
	float OpenDuration = 0.35f;

	/** 명시 연결(없으면 자동 탐색) */
	UPROPERTY(EditAnywhere, Category = "Stash|Ref")
	ARunWorldDirector* DirectorRef = nullptr;

	// ── State ──────────────────────────────────────────────
	UPROPERTY(VisibleAnywhere, Category = "Stash|State")
	bool bLidOpen = false;

	bool  bPlayingAnim = false;
	float AnimElapsed = 0.f;
	FRotator LidRot_Closed;
	FRotator LidRot_Open;

	// ── Internal ───────────────────────────────────────────
	void PlayOpen();
	void UpdateLid(float Alpha);
	ARunWorldDirector* ResolveDirector() const;
	// ✅ 사운드 재생 헬퍼 함수
	void PlayOpenSound();
};
