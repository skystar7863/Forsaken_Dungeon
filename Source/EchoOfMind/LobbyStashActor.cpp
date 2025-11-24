#include "LobbyStashActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "Kismet/KismetMathLibrary.h"

#include "Kismet/GameplayStatics.h" // ✅ 추가
#include "RunWorldDirector.h"
#include "EchoPlayer.h"
#include "EchoPlayerController.h"
#include "Item/InventoryRootWidget.h"
#include "LobbyStashComponent.h"

ALobbyStashActor::ALobbyStashActor()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(Root);
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BaseMesh->SetCollisionResponseToAllChannels(ECR_Block);
	BaseMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // 라인트레이스 적중

	LidPivot = CreateDefaultSubobject<USceneComponent>(TEXT("LidPivot"));
	LidPivot->SetupAttachment(BaseMesh);

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidMesh"));
	LidMesh->SetupAttachment(LidPivot);
	LidMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	LidMesh->SetCollisionResponseToAllChannels(ECR_Block);

	LidRot_Closed = FRotator::ZeroRotator;
	LidRot_Open = FRotator(0.f, 0.f, OpenAngle); // Pitch 기준 오픈 OpenAngle
}

void ALobbyStashActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bPlayingAnim) return;

	AnimElapsed += DeltaSeconds;
	const float Alpha = FMath::Clamp(AnimElapsed / FMath::Max(0.01f, OpenDuration), 0.f, 1.f);
	UpdateLid(Alpha);

	if (Alpha >= 1.f)
	{
		bPlayingAnim = false;
		SetActorTickEnabled(false);
	}
}

void ALobbyStashActor::UpdateLid(float Alpha)
{
	const FRotator R = UKismetMathLibrary::RLerp(LidRot_Closed, FRotator(0, 0,OpenAngle), Alpha, true);
	LidMesh->SetRelativeRotation(R);
}

void ALobbyStashActor::PlayOpen()
{
	if (bLidOpen) return;          // 한 번 열리면 닫지 않음
	bLidOpen = true;
	bPlayingAnim = true;
	AnimElapsed = 0.f;
	SetActorTickEnabled(true);
	// ✅ 열리는 사운드 재생
	PlayOpenSound();
}

ARunWorldDirector* ALobbyStashActor::ResolveDirector() const
{
	if (DirectorRef) return DirectorRef;
	for (TActorIterator<ARunWorldDirector> It(GetWorld()); It; ++It)
	{
		return *It;
	}
	return nullptr;
}

void ALobbyStashActor::Interact(AEchoPlayer* Player)
{
	if (!Player) return;

	ARunWorldDirector* Dir = ResolveDirector();
	if (!Dir) return;

	if (ULobbyStashComponent* Stash = Dir->GetLobbyStash())
	{
		PlayOpen();

		if (AEchoPlayerController* PC = Player->GetPlayerController())
		{
			PC->ShowInventory(true);
			if (UInventoryRootWidget* RootW = PC->GetInventoryRoot())
			{
				RootW->OpenLobbyStash(Stash); // 전리품은 숨기고 스태시만 표시
			}
		}
	}
}

void ALobbyStashActor::PlayOpenSound()
{
	if (ChestOpenSound && LidMesh)
	{
		UGameplayStatics::SpawnSoundAttached(
			ChestOpenSound,
			LidMesh,
			NAME_None,
			FVector::ZeroVector,
			EAttachLocation::KeepRelativeOffset,
			true);
	}
}