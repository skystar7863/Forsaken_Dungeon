#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/ItemEnums.h"
#include "Item/ItemInstance.h"
#include "LootChestActor.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UEnemyLootComponent;
class UItemDefinition;
class AEchoPlayer;
class USoundBase; // ✅ 추가

UENUM(BlueprintType)
enum class EChestSize : uint8
{
	Small,
	Medium,
	Large
};

UCLASS()
class ECHOOFMIND_API ALootChestActor : public AActor
{
	GENERATED_BODY()

public:
	ALootChestActor();

	/** 라인트레이스 상호작용 시 호출 */
	UFUNCTION(BlueprintCallable, Category = "Chest")
	void Interact(AEchoPlayer* Player);

	/** 인벤토리 UI가 닫혔을 때(이제 아무 것도 안 함 – 상자는 계속 열린 상태 유지) */
	UFUNCTION(BlueprintCallable, Category = "Chest")
	void OnLootWidgetClosed();

protected:
	virtual void Tick(float DeltaSeconds) override;

	// ─── 컴포넌트 ─────────────────────────────────────────────
	UPROPERTY(VisibleAnywhere, Category = "Chest|Comp")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, Category = "Chest|Comp")
	UStaticMeshComponent* BaseMesh;

	/** 필요하면 힌지 위치 조정용(회전은 쓰지 않음) */
	UPROPERTY(VisibleAnywhere, Category = "Chest|Comp")
	USceneComponent* LidPivot;

	UPROPERTY(VisibleAnywhere, Category = "Chest|Comp")
	UStaticMeshComponent* LidMesh;

	/** 전리품 컨테이너(4x2) */
	UPROPERTY(VisibleAnywhere, Category = "Chest|Comp")
	UEnemyLootComponent* Loot;

	// ─── 설정 ────────────────────────────────────────────────
	UPROPERTY(EditAnywhere, Category = "Chest|Setup")
	EChestSize ChestSize = EChestSize::Small;

	/** 드롭 후보(에디터에서 세팅) */
	UPROPERTY(EditAnywhere, Category = "Chest|Setup")
	TArray<UItemDefinition*> ItemPool;

	/** 드롭 개수 오버라이드(0이면 사이즈 기본) */
	UPROPERTY(EditAnywhere, Category = "Chest|Setup")
	int32 DropsOverride = 0;

	/** LidMesh 로컬 Pitch( X축 )으로 열릴 각도(기본 -30˚) */
	UPROPERTY(EditAnywhere, Category = "Chest|Anim")
	float OpenAngle = -30.f;

	/** 열리는 시간(초) */
	UPROPERTY(EditAnywhere, Category = "Chest|Anim")
	float OpenDuration = 0.35f;

	// ✅ 상자 열릴 때 재생할 사운드
	UPROPERTY(EditAnywhere, Category = "Chest|Sound")
	USoundBase* ChestOpenSound;

	// ─── 상태 ────────────────────────────────────────────────
	UPROPERTY(VisibleAnywhere, Category = "Chest|State")
	bool bOpenedOnce = false;     // 최초 오픈하여 전리품 생성했는지

	UPROPERTY(VisibleAnywhere, Category = "Chest|State")
	bool bLidOpen = false;        // 현재 뚜껑 열림 상태

	bool   bPlayingAnim = false;
	float  AnimElapsed = 0.f;
	FRotator LidRot_Closed;
	FRotator LidRot_Open;

	// ─── 내부 로직 ───────────────────────────────────────────
	void GenerateLoot();
	int32 GetDropCount() const;
	EItemRarity RollRarity() const;
	bool IsEquipmentType(EItemType Type) const;
	void RollExtraPerks(const FItemInstance& Base, TArray<EExtraPerk>& OutPerks) const;

	void PlayOpen();              // 여는 애니메이션만 사용
	void UpdateLid(float Alpha);  // LidMesh에 회전 적용

	// ✅ 사운드 재생 헬퍼 함수
	void PlayOpenSound();
};
