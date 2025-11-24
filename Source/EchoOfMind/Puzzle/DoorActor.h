#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class ECHOOFMIND_API ADoorActor : public AActor
{
	GENERATED_BODY()

public:
	ADoorActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	UStaticMeshComponent* DoorBarMesh;

	/** 여는 각도(Yaw) — 에디터에서 배치된 각도를 '닫힘'으로 간주하고, 이 값만큼 Yaw를 더하면 '열림'이야 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float OpenAngle = 57.721236f;

	/** 게임 시작 시 기본 상태(열림/닫힘). 기본 true = 열림 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	bool bStartOpen = true;

	/** 문 토글(기존 동작 유지) */
	UFUNCTION(BlueprintCallable, Category = "Door")
	virtual void OpenDoor();

	/** 문 강제 상태 지정 (true=열기, false=닫기) — 파생 클래스가 재정의할 수 있도록 virtual */
	UFUNCTION(BlueprintCallable, Category = "Door")
	virtual void SetDoorOpen(bool bOpen);

	/** 헬퍼 */
	UFUNCTION(BlueprintCallable, Category = "Door")
	void Open() { SetDoorOpen(true); }
	UFUNCTION(BlueprintCallable, Category = "Door")
	void Close() { SetDoorOpen(false); }

	// ✅ 열릴 때, 닫힐 때 각각의 사운드
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	USoundBase* DoorOpenSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	USoundBase* DoorCloseSound;


protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	FRotator ClosedRotation;
	FRotator OpenRotation;

	UPROPERTY(VisibleAnywhere, Category = "Door")
	bool bIsOpen = false;

	bool bIsMoving = false;

	/** 인터폴레이션 속도(도/초) */
	UPROPERTY(EditAnywhere, Category = "Door")
	float DoorOpenSpeed = 90.f;

	FRotator TargetRotation;

	// ✅ 사운드 재생용 헬퍼
	void PlayDoorSound(bool bOpening);
};
