#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RunWorldDirector.generated.h"

class UBoxComponent;
class URunSessionSubsystem;
class UAttributeComponent;
class ULobbyStashComponent;
class AEchoPlayer;
class ARoomEncounterTrigger;

USTRUCT()
struct FSpawnSnapshotEntry
{
	GENERATED_BODY()
	UPROPERTY() TSubclassOf<AActor> Class;
	UPROPERTY() FTransform Transform;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWorldRepopulated);

UCLASS()
class ECHOOFMIND_API ARunWorldDirector : public AActor
{
	GENERATED_BODY()
public:
	ARunWorldDirector();

	UPROPERTY(VisibleAnywhere) UBoxComponent* CaptureVolume;
	UPROPERTY(EditAnywhere)  AActor* LobbySpawn = nullptr;
	UPROPERTY(EditAnywhere)  AActor* DungeonSpawn = nullptr;
	UPROPERTY(EditAnywhere)  AActor* BossSpawn = nullptr;

	// 스냅샷
	UPROPERTY(EditAnywhere) bool bBuildSnapshotOnBeginPlay = true;
	UPROPERTY(EditAnywhere) bool bDebugLog = false;

	UPROPERTY(BlueprintAssignable) FOnWorldRepopulated OnWorldRepopulated;

	// ✅ 로비 스태시(세션 외 보관)
	UPROPERTY(VisibleAnywhere, Category = "Run|Stash")
	ULobbyStashComponent* LobbyStash = nullptr;

	// 접근자
	UFUNCTION(BlueprintPure, Category = "Run|Stash")
	ULobbyStashComponent* GetLobbyStash() const { return LobbyStash; }

	// 공개 API
	UFUNCTION(BlueprintCallable, Category = "Run")
	void ResetRunSession();

	UFUNCTION(BlueprintCallable, Category = "Run")
	void ResetPlayerVitalsAndCooldowns( bool bResetXP = true);

	// 텔레포트 헬퍼
	UFUNCTION(BlueprintCallable, Category = "Run")
	void TeleportPlayerToLobby();
	UFUNCTION(BlueprintCallable, Category = "Run")
	void TeleportPlayerToDungeon();
	UFUNCTION(BlueprintCallable, Category = "Run")
	void TeleportPlayerToBoss();

	// Encounter 트리거 상태만 전체 리셋(문 열기 + 입장 플래그 초기화)
	UFUNCTION(BlueprintCallable, Category = "Run")
	void ResetAllEncounterTriggers() const;

	void ResetMapFromSnapshot();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	TWeakObjectPtr<URunSessionSubsystem> RunSS;
	FBox VolumeBox;

	// 스냅샷 데이터
	TArray<FSpawnSnapshotEntry> EnemySnapshot;
	TArray<FSpawnSnapshotEntry> ChestSnapshot;

	AEchoPlayer* GetPlayer() const;
	bool IsInsideCapture(const FVector& Location) const;
	void CollectInside(TSubclassOf<AActor> FilterClass, TArray<AActor*>& Out) const;

	void BuildSnapshot();
	void DestroyInside(TSubclassOf<AActor> FilterClass);
	void SpawnFrom(const TArray<FSpawnSnapshotEntry>& Snapshot);

	// 이벤트 핸들러
	UFUNCTION() void OnPlayerDied();

	// 세부 동작
	void ResetPlayerState(AEchoPlayer* P);
	void ClearPlayerSessionInventory(AEchoPlayer* P);
	void ResetWorld();
	void TeleportToAnchor(AActor* Anchor) const;
};
