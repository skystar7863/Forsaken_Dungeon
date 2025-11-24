#include "DoorActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

ADoorActor::ADoorActor()
{
	PrimaryActorTick.bCanEverTick = true;

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMech"));
	DoorBarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorBarMech"));

	RootComponent = DoorBarMesh;
	DoorMesh->SetupAttachment(RootComponent);
}

void ADoorActor::BeginPlay()
{
	Super::BeginPlay();

	// 배치된 상대 회전을 '닫힘' 기준으로 저장
	ClosedRotation = DoorMesh->GetRelativeRotation();
	OpenRotation = ClosedRotation + FRotator(0.f, OpenAngle, 0.f);

	// 기본 상태를 즉시 적용(애니메이션 없이)
	if (bStartOpen)
	{
		bIsOpen = true;
		bIsMoving = false;
		TargetRotation = OpenRotation;
		DoorMesh->SetRelativeRotation(OpenRotation);
	}
	else
	{
		bIsOpen = false;
		bIsMoving = false;
		TargetRotation = ClosedRotation;
		DoorMesh->SetRelativeRotation(ClosedRotation);
	}
}

void ADoorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsMoving)
	{
		const FRotator Current = DoorMesh->GetRelativeRotation();
		const FRotator NewRot = FMath::RInterpConstantTo(Current, TargetRotation, DeltaTime, DoorOpenSpeed);
		DoorMesh->SetRelativeRotation(NewRot);

		if (NewRot.Equals(TargetRotation, 0.01f))
		{
			DoorMesh->SetRelativeRotation(TargetRotation);
			bIsMoving = false;
		}
	}
}

void ADoorActor::OpenDoor()
{
	// 토글
	SetDoorOpen(!bIsOpen);
}

void ADoorActor::SetDoorOpen(bool bOpen)
{
	if (bIsOpen == bOpen)
	{
		// 이미 그 상태면 스킵
		return;
	}

	bIsOpen = bOpen;
	TargetRotation = bIsOpen ? OpenRotation : ClosedRotation;
	bIsMoving = true;

	// ✅ 문 열리거나 닫힐 때 사운드 재생
	PlayDoorSound(bIsOpen);

}

void ADoorActor::PlayDoorSound(bool bOpening)
{
	USoundBase* SoundToPlay = bOpening ? DoorOpenSound : DoorCloseSound;
	if (SoundToPlay)
	{
		UGameplayStatics::SpawnSoundAttached(
			SoundToPlay,
			DoorMesh,
			NAME_None,
			FVector::ZeroVector,
			EAttachLocation::KeepRelativeOffset,
			true);
	}
}
