#include "PortalActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

#include "EchoPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "RunSessionSubsystem.h"
#include "RunWorldDirector.h"

APortalActor::APortalActor()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false); // 집중 중에만 틱

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetGenerateOverlapEvents(false);

	UseBox = CreateDefaultSubobject<UBoxComponent>(TEXT("UseBox"));
	UseBox->SetupAttachment(Mesh);
	UseBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	UseBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	UseBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // 라인트레이스에 맞도록
	UseBox->SetBoxExtent(FVector(50.f));
}

void APortalActor::BeginPlay()
{
	Super::BeginPlay();

	// 디렉터가 비어있으면 자동 탐색 (레벨에 1개 전제)
	if (!WorldDirector)
	{
		for (TActorIterator<ARunWorldDirector> It(GetWorld()); It; ++It)
		{
			WorldDirector = *It;
			break;
		}
	}
}

void APortalActor::Interact(AEchoPlayer* Player)
{
	if (bIsFocusing) return;
	if (!Player) return;

	// Teleport 모드: 목적지 없으면 경고
	if (Mode == EPortalMode::Teleport && !TeleportTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Portal] TeleportTarget not set on %s"), *GetName());
		return;
	}

	// 세션 모드: 디렉터 필요
	if ((Mode == EPortalMode::StartDungeon || Mode == EPortalMode::EnterBoss || Mode == EPortalMode::ExtractToLobby) && !WorldDirector)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Portal] WorldDirector not found: %s"), *GetName());
		return;
	}

	StartFocus(Player);
}

void APortalActor::StartFocus(AEchoPlayer* Player)
{
	FocusingPlayer = Player;
	bIsFocusing = true;
	FocusElapsed = 0.f;

	SetActorTickEnabled(true);
	OnFocusStarted.Broadcast();

	if (FocusingPlayer)
	{
		FocusingPlayer->PlayPortalFocusMontage();
	}

	UE_LOG(LogTemp, Log, TEXT("[Portal] Focus start by %s (Mode=%d)"), *Player->GetName(), (int32)Mode);
}

void APortalActor::CancelFocus(const TCHAR* Reason)
{
	if (!bIsFocusing) return;

	if (FocusingPlayer)
	{
		FocusingPlayer->StopPortalFocusMontage();
	}

	UE_LOG(LogTemp, Log, TEXT("[Portal] Focus canceled: %s (%s)"), Reason ? Reason : TEXT(""), *GetName());

	bIsFocusing = false;
	FocusingPlayer = nullptr;
	FocusElapsed = 0.f;
	SetActorTickEnabled(false);

	OnFocusCanceled.Broadcast();
}

void APortalActor::FinishFocus()
{
	if (!bIsFocusing || !FocusingPlayer)
	{
		CancelFocus(TEXT("Invalid state"));
		return;
	}

	// 정신집중 끝났으니 이동 애니메이션 실행
	FocusingPlayer->StopPortalFocusMontage();
	FocusingPlayer->PlayPortalTravelMontage();

	// 약간의 딜레이 후 실제 이동 실행 (애니메이션 자연스럽게)
	const float TravelDelay = 0.5f;
	FTimerHandle DelayHandle;
	GetWorldTimerManager().SetTimer(DelayHandle, [this]()
		{
			switch (Mode)
			{
			case EPortalMode::Teleport:      DoSimpleTeleport();  break;
			case EPortalMode::StartDungeon:  DoStartDungeon();    break;
			case EPortalMode::EnterBoss:     DoEnterBoss();       break;
			case EPortalMode::ExtractToLobby:DoExtractToLobby();  break;
			default: break;
			}
		}, TravelDelay, false);

	bIsFocusing = false;
	FocusingPlayer = nullptr;
	FocusElapsed = 0.f;
	SetActorTickEnabled(false);
	OnFocusFinished.Broadcast();
}


void APortalActor::DoSimpleTeleport()
{
	if (!FocusingPlayer || !TeleportTarget)
		return;

	const FVector DestLoc = TeleportTarget->GetActorLocation() + TeleportOffset;
	const FRotator DestRot = bUseTargetRotation ? TeleportTarget->GetActorRotation()
		: FocusingPlayer->GetActorRotation();

	FocusingPlayer->TeleportTo(DestLoc, DestRot, false, true);

	UE_LOG(LogTemp, Log, TEXT("[Portal] Teleported %s to %s"),
		*FocusingPlayer->GetName(), *DestLoc.ToString());
}

void APortalActor::DoStartDungeon()
{
	if (!WorldDirector) return;

	if (UGameInstance* GI = GetGameInstance())
		if (URunSessionSubsystem* SS = GI->GetSubsystem<URunSessionSubsystem>())
		{
			SS->StartDungeonRunInPlace(WorldDirector, DungeonMinutes);
			UE_LOG(LogTemp, Log, TEXT("[Portal] Start Dungeon (%.1f min)"), DungeonMinutes);
		}
}

void APortalActor::DoEnterBoss()
{
	if (!WorldDirector) return;

	if (UGameInstance* GI = GetGameInstance())
		if (URunSessionSubsystem* SS = GI->GetSubsystem<URunSessionSubsystem>())
		{
			SS->EnterBossRoomInPlace(WorldDirector, BossMinutes);
			UE_LOG(LogTemp, Log, TEXT("[Portal] Enter Boss (%.1f min)"), BossMinutes);
		}
}

void APortalActor::DoExtractToLobby()
{
	if (!WorldDirector) return;

	if (UGameInstance* GI = GetGameInstance())
		if (URunSessionSubsystem* SS = GI->GetSubsystem<URunSessionSubsystem>())
		{
			// ✅ 포탈로 로비 이동 직전: 체/마 풀충, XP 0, 스킬쿨타임 초기화
			/*if (AEchoPlayer* P = FocusingPlayer)
			{
			}*/
				WorldDirector->ResetPlayerVitalsAndCooldowns( /*bResetXP=*/true);

			SS->ExtractToLobbyInPlace(WorldDirector); // 기존 타이머/위치 이동 로직
			UE_LOG(LogTemp, Log, TEXT("[Portal] Extract to Lobby"));
		}
}


void APortalActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bIsFocusing) return;

	// 플레이어 유효성 체크
	if (!IsValid(FocusingPlayer) || FocusingPlayer->IsActorBeingDestroyed())
	{
		CancelFocus(TEXT("Player invalid"));
		return;
	}

	// 이동 입력 감지 → 취소
	const FVector LastInput = FocusingPlayer->GetLastMovementInputVector();
	if (LastInput.SizeSquared() > (MoveInputCancelThreshold * MoveInputCancelThreshold))
	{
		CancelFocus(TEXT("Move input"));
		return;
	}

	// 진행도 누적
	FocusElapsed += DeltaSeconds;
	if (FocusElapsed >= FocusDuration)
	{
		FinishFocus();
	}
}
