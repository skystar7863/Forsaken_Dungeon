#pragma once

#include "CoreMinimal.h"
#include "ButtonActor.h"                 // ControlledDoors 재사용
#include "RoomEncounterTrigger.generated.h"

class UBoxComponent;
class AEnemyBase;
class UAttributeComponent;
class ARunWorldDirector;

/**
 * 방에 들어오면 문 닫고, 방 몬스터 전멸 시 문 열기
 * - 레벨에 배치해서 ControlledDoors 에 문들 드래그
 * - Enemies 를 직접 지정하거나 bAutoCollectEnemiesInVolume 사용
 * - 맵 리셋/재스폰 후에는 디렉터 이벤트를 받아 자동 재수집
 */
UCLASS()
class ECHOOFMIND_API ARoomEncounterTrigger : public AButtonActor
{
	GENERATED_BODY()

public:
	ARoomEncounterTrigger();

	/** 외부에서 강제로 새로고침하고 싶을 때 호출 가능 (재스폰 후 바인딩 재구성) */
	UFUNCTION(BlueprintCallable, Category = "Encounter")
	void RefreshEnemies();

	/** ✅ 문/트리거 상태만 초기화(열려있게 만들고, 입장 여부를 초기화) */
	UFUNCTION(BlueprintCallable, Category = "Encounter")
	void ResetDoorState();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Encounter|Comp")
	UBoxComponent* RoomVolume;

	/** 트리거 1회만 동작할지 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Encounter")
	bool bOnceOnly = true;

	/** 들어오면 즉시 문 닫기 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Encounter")
	bool bCloseDoorsOnEnter = true;

	/** BeginPlay 에서 영역 내 몬스터 자동 수집 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Encounter")
	bool bAutoCollectEnemiesInVolume = true;

	/** 수동 지정할 몬스터들 (자동 수집을 끄면 이 배열만 사용) */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Encounter")
	TArray<AEnemyBase*> Enemies;

	/** 디렉터(레벨에 1개). 비워두면 BeginPlay에서 자동 탐색 */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Encounter")
	ARunWorldDirector* WorldDirector = nullptr;

	/** 내부 상태 */
	bool  bTriggered = false;
	int32 AliveEnemies = 0;

	// 유틸
	void TryAutoCollect();
	void BindAllEnemies();
	void BindOne(AEnemyBase* Enemy);
	void CloseAllDoors();
	void OpenAllDoors();

	// 디렉터 이벤트
	UFUNCTION()
	void OnWorldRepopulated();

	// 적 죽음 카운팅
	UFUNCTION()
	void OnEnemyDied();

	UFUNCTION()
	void OnRoomBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	virtual void BeginPlay() override;
};
