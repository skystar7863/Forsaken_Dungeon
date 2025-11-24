#include "SlidingDoorActor.h"
#include "Components/StaticMeshComponent.h"

ASlidingDoorActor::ASlidingDoorActor()
{
	PrimaryActorTick.bCanEverTick = true;
	// 문판은 반드시 움직일 수 있어야 함
	if (DoorMesh)
	{
		DoorMesh->SetMobility(EComponentMobility::Movable);
	}
}

void ASlidingDoorActor::BeginPlay()
{
	// 부모가 회전 도어 초기화를 수행하므로,
	// Super 호출 "전"에 현재 회전을 저장해두고, 호출 "후" 저장값으로 복원한다.
	const FRotator InitialRelRot = DoorMesh ? DoorMesh->GetRelativeRotation() : FRotator::ZeroRotator;

	Super::BeginPlay(); // 틱, 기타 부모 초기화 유지

	if (DoorMesh)
	{
		// 부모가 적용한 회전을 원래 값으로 되돌림(슬라이딩 도어는 회전 안 함)
		DoorMesh->SetRelativeRotation(InitialRelRot);

		// 슬라이드 기준 위치 계산
		ClosedRelLoc = DoorMesh->GetRelativeLocation();
		OpenRelLoc = ClosedRelLoc + FVector(0.f, 0.f, SlideDistance);

		// 시작 상태(열림/닫힘)에 맞춰 위치만 설정
		if (bStartOpen)
		{
			bSlidingOpen = true;
			bIsSliding = false;
			DoorMesh->SetRelativeLocation(OpenRelLoc);
		}
		else
		{
			bSlidingOpen = false;
			bIsSliding = false;
			DoorMesh->SetRelativeLocation(ClosedRelLoc);
		}

		// 혹시 BP에서 Static이면 한 번 더 보장
		DoorMesh->SetMobility(EComponentMobility::Movable);
	}
}

void ASlidingDoorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsSliding || !DoorMesh) return;

	const FVector Target = bSlidingOpen ? OpenRelLoc : ClosedRelLoc;
	const FVector Current = DoorMesh->GetRelativeLocation();

	const FVector NewLoc = FMath::VInterpConstantTo(Current, Target, DeltaTime, SlideSpeed);
	DoorMesh->SetRelativeLocation(NewLoc);

	if (NewLoc.Equals(Target, 1.0f))
	{
		DoorMesh->SetRelativeLocation(Target);
		bIsSliding = false;
	}
}

void ASlidingDoorActor::OpenDoor()
{
	// 토글
	SetDoorOpen(!bSlidingOpen);
}

void ASlidingDoorActor::SetDoorOpen(bool bOpen)
{
	if (bSlidingOpen == bOpen) return;

	bSlidingOpen = bOpen;
	bIsSliding = true;
}
