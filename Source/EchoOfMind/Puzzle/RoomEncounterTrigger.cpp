#include "RoomEncounterTrigger.h"
#include "Components/BoxComponent.h"
#include "DoorActor.h"
#include "AEnemyBase.h"
#include "AttributeComponent.h"
#include "EchoPlayer.h"
#include "RunWorldDirector.h"
#include "EngineUtils.h"

ARoomEncounterTrigger::ARoomEncounterTrigger()
{
	// ButtonActor 의 Collision 을 루트로 쓰고 있었으니, 그 위에 RoomVolume 을 추가
	RoomVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("RoomVolume"));
	RoomVolume->SetupAttachment(RootComponent ? RootComponent.Get() : Collision);

	// 플레이어만 감지
	RoomVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RoomVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	RoomVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RoomVolume->SetGenerateOverlapEvents(true);

	RoomVolume->OnComponentBeginOverlap.AddDynamic(this, &ARoomEncounterTrigger::OnRoomBeginOverlap);
}

void ARoomEncounterTrigger::BeginPlay()
{
	Super::BeginPlay();

	// 디렉터 자동 탐색 & 이벤트 구독
	if (!WorldDirector)
	{
		for (TActorIterator<ARunWorldDirector> It(GetWorld()); It; ++It) { WorldDirector = *It; break; }
	}
	if (WorldDirector)
	{
		WorldDirector->OnWorldRepopulated.AddDynamic(this, &ARoomEncounterTrigger::OnWorldRepopulated);
	}

	RefreshEnemies(); // 최초 바인딩
}

void ARoomEncounterTrigger::TryAutoCollect()
{
	Enemies.Reset();

	const FBox Bounds = RoomVolume->Bounds.GetBox();

	for (TActorIterator<AEnemyBase> It(GetWorld()); It; ++It)
	{
		AEnemyBase* E = *It;
		if (!IsValid(E)) continue;

		if (Bounds.IsInsideOrOn(E->GetActorLocation()))
		{
			Enemies.AddUnique(E);
		}
	}
}

void ARoomEncounterTrigger::BindAllEnemies()
{
	AliveEnemies = 0;

	for (AEnemyBase* E : Enemies)
	{
		BindOne(E);
	}
}

void ARoomEncounterTrigger::BindOne(AEnemyBase* Enemy)
{
	if (!IsValid(Enemy)) return;

	if (UAttributeComponent* Attr = Enemy->FindComponentByClass<UAttributeComponent>())
	{
		// 이미 죽은 애는 건너뛰고, 살아있으면 카운트 + 바인딩
		if (Attr->Health > 0.f)
		{
			++AliveEnemies;
			Attr->OnDeath.AddDynamic(this, &ARoomEncounterTrigger::OnEnemyDied);
		}
	}
}

void ARoomEncounterTrigger::CloseAllDoors()
{
	for (ADoorActor* Door : ControlledDoors)
	{
		if (IsValid(Door))
		{
			Door->SetDoorOpen(false);
		}
	}
}

void ARoomEncounterTrigger::OpenAllDoors()
{
	for (ADoorActor* Door : ControlledDoors)
	{
		if (IsValid(Door))
		{
			Door->SetDoorOpen(true);
		}
	}
}

void ARoomEncounterTrigger::OnRoomBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	// 플레이어만 트리거
	if (!Cast<AEchoPlayer>(OtherActor)) return;

	if (/*bOnceOnly &&*/ bTriggered) return;
	bTriggered = true;

	if (bCloseDoorsOnEnter)
	{
		CloseAllDoors();
	}

	// 혹시 방에 몬스터가 하나도 없으면 바로 문 열어주기
	if (AliveEnemies <= 0)
	{
		OpenAllDoors();
	}
}

void ARoomEncounterTrigger::OnEnemyDied()
{
	AliveEnemies = FMath::Max(0, AliveEnemies - 1);

	if (AliveEnemies <= 0)
	{
		OpenAllDoors();

		// 한 번만 쓰고 끝낼 거면 트리거 비활성화(선택)
		if (bOnceOnly)
		{
			RoomVolume->SetGenerateOverlapEvents(false);
			SetActorTickEnabled(false);
		}
	}
}

void ARoomEncounterTrigger::OnWorldRepopulated()
{
	// 맵 리셋/재스폰 끝났으니 새 적으로 다시 세팅
	RefreshEnemies();
}

void ARoomEncounterTrigger::RefreshEnemies()
{
	if (bAutoCollectEnemiesInVolume) TryAutoCollect();
	BindAllEnemies();

	// 세션 리셋 후에는 "입장 전" 상태로 돌려놓자
	bTriggered = false;
	OpenAllDoors(); // 플레이어가 들어오면 다시 닫히게
}

void ARoomEncounterTrigger::ResetDoorState()
{
	// ✅ 포탈로 던전 재진입 시: 문 상태/입장 여부만 초기화
	bTriggered = false;
	OpenAllDoors();
}
