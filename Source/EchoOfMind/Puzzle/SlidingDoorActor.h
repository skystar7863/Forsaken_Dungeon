#pragma once

#include "CoreMinimal.h"
#include "DoorActor.h"
#include "SlidingDoorActor.generated.h"

/** 회전 대신 문판(DoorMesh)을 Z축으로 슬라이드해 여닫는 도어 */
UCLASS()
class ECHOOFMIND_API ASlidingDoorActor : public ADoorActor
{
	GENERATED_BODY()

public:
	ASlidingDoorActor();

	// 부모에 이미 UFUNCTION(BlueprintCallable) 존재 → 여기서는 UFUNCTION 제거하고 override만!
	virtual void OpenDoor() override;
	virtual void SetDoorOpen(bool bOpen) override;

protected:
	// ⚠ 부모 BeginPlay(회전 초기화)를 호출하지 않는다!
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/** 문판이 올라갈 거리(+Z). 프레임은 DoorBarMesh가 그대로, DoorMesh만 움직임 */
	UPROPERTY(EditAnywhere, Category = "Sliding Door")
	float SlideDistance = 300.f;

	/** 이동 속도(유닛/초) */
	UPROPERTY(EditAnywhere, Category = "Sliding Door")
	float SlideSpeed = 200.f;

private:
	// DoorMesh의 상대 위치 기준
	FVector ClosedRelLoc;
	FVector OpenRelLoc;

	bool bSlidingOpen = false;   // 현재 목표 상태
	bool bIsSliding = false;   // 인터폴레이션 중인지
};
