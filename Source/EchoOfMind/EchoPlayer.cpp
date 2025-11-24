// 플레이어 캐릭터 구현

#include "EchoPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "DrawDebugHelpers.h"
#include "Puzzle/ButtonActor.h"
#include "Puzzle/LeverActor.h"
#include "Puzzle/PickupItem.h"
#include "Puzzle/InventoryWidget.h"
#include "EchoPlayerController.h"
#include "Puzzle/PedestalActor.h"
#include "Puzzle/ItemPlacementPuzzleActor.h"
#include "AttributeComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "SkillComponent.h"
#include "PerkComponent.h"
#include "LootChestActor.h"
#include "PortalActor.h"
#include "LobbyStashActor.h"
#include "AEnemyBase.h"
#include "Item/ConsumableContainerComponent.h"
#include "Item/PlayerInventoryComponent.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"

// 생성자: 컴포넌트 및 기본값 초기화
AEchoPlayer::AEchoPlayer()
{
	// 캡슐 크기
	GetCapsuleComponent()->InitCapsuleSize(35.f, 90.0f);

	// 컨트롤러 회전 비활성화
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 이동 방향으로 자동 회전
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	// 카메라 붐 (스프링암)
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 400.f;
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SocketOffset = FVector(0.f, 50.f, 0.f);

	// 카메라
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;

	// 회전 속도
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// 컴포넌트 생성
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	SkillComp = CreateDefaultSubobject<USkillComponent>(TEXT("SkillComp"));
	PerkComp = CreateDefaultSubobject<UPerkComponent>(TEXT("PerkComp"));
}

// 게임 시작 시
void AEchoPlayer::BeginPlay()
{
	Super::BeginPlay();
	// 레벨업 이벤트 바인딩
	Attributes->OnLevelUp.AddDynamic(this, &AEchoPlayer::HandleLevelUp);
}

// 오브젝트 상호작용 수행
void AEchoPlayer::PerformAction()
{
	FVector Start = CameraComponent->GetComponentLocation();
	FVector Forward = CameraComponent->GetForwardVector();
	FVector End = Start + Forward * InteractionDistance;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	// 디버그 라인
	DrawDebugLine(GetWorld(), Start, bHit ? Hit.Location : End, bHit ? FColor::Green : FColor::Red, false, 2.0f, 0, 1.0f);

	if (bHit && Hit.GetActor())
	{
		// 아이템 줍기
		if (APickupItem* Item = Cast<APickupItem>(Hit.GetActor())) {
			if (!HeldItem) {
				HeldItem = Item;
				Item->OnPickedUp();

				if (AEchoPlayerController* PC = Cast<AEchoPlayerController>(GetController()))
				{
					if (UInventoryWidget* Inv = PC->GetInventoryWidget())
					{
						Inv->SetItemIcon(Item->Icon);
					}
				}
			}
			return;
		}

		// 퍼즐 오브젝트 상호작용
		if (APedestalActor* Ped = Cast<APedestalActor>(Hit.GetActor()))
		{
			Ped->Interact(this);
			return;
		}

		if (AItemPlacementPuzzleActor* Puzzle = Cast<AItemPlacementPuzzleActor>(Hit.GetActor()))
		{
			Puzzle->PressButton();
			return;
		}

		if (AButtonActor* Button = Cast<AButtonActor>(Hit.GetActor()))
		{
			Button->PressButton();
			UE_LOG(LogTemp, Warning, TEXT("Button Pressed: %s"), *Button->GetName());
			return;
		}

		if (ALeverActor* Lever = Cast<ALeverActor>(Hit.GetActor()))
		{
			Lever->ToggleLever();
			return;
		}
		if (ALootChestActor* Chest = Cast<ALootChestActor>(Hit.GetActor()))
		{
			Chest->Interact(this);
			return;
		}
		if (AEnemyBase* Corpse = Cast<AEnemyBase>(Hit.GetActor()))
		{
			Corpse->Interact(this);
			return;
		}
		if (APortalActor* Portal = Cast<APortalActor>(Hit.GetActor()))
		{
			Portal->Interact(this);
			return;
		}
		if (ALobbyStashActor* Stash = Cast<ALobbyStashActor>(Hit.GetActor()))
		{
			Stash->Interact(this);
			return;
		}

	}
}

// 플레이어 컨트롤러 반환
AEchoPlayerController* AEchoPlayer::GetPlayerController() const
{
	return Cast<AEchoPlayerController>(GetController());
}

// 아이템 줍기 처리
void AEchoPlayer::PickupItem(APickupItem* Item)
{
	if (!Item || HeldItem) return;

	HeldItem = Item;
	Item->OnPickedUp();

	if (APlayerController* PC = GetController<APlayerController>())
	{
		if (AEchoPlayerController* EPC = Cast<AEchoPlayerController>(PC)) {
			if (UInventoryWidget* Inv = EPC->GetInventoryWidget())
			{
				Inv->SetItemIcon(Item->Icon);
			}
		}
	}
}

// 근접 공격 처리
void AEchoPlayer::Attack()
{
	if (bIsDodging) return;
	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * AttackRange;
	FCollisionShape Shape = FCollisionShape::MakeSphere(AttackRadius);

	TArray<FHitResult> Hits;
	bool bHit = GetWorld()->SweepMultiByChannel(Hits, Start, End, FQuat::Identity, ECC_Pawn, Shape);

#if WITH_EDITOR
	//DrawDebugSphere(GetWorld(), End, AttackRadius, 12, bHit ? FColor::Red : FColor::Green, false, 1.f);
#endif

	if (bHit && Attributes)
	{
		const float BasePower = Attributes->AttackPower;
		const float AdjustedPower = BasePower * Attributes->OutgoingDamageMultiplier;

		TSet<AActor*> Damaged; // 중복 타격 방지(선택)
		for (auto& Hit : Hits)
		{
			AActor* Other = Hit.GetActor();
			if (!Other || Other == this || Damaged.Contains(Other)) continue;

			// ✅ 직접 컴포넌트로 데미지 주지 말고 ApplyDamage 사용
			UGameplayStatics::ApplyDamage(
				/* DamagedActor   */ Other,
				/* BaseDamage     */ AdjustedPower,
				/* EventInstigator*/ GetController(),
				/* DamageCauser   */ this,
				/* DamageType     */ nullptr
			);

			Damaged.Add(Other);
		}
	}
}

void AEchoPlayer::Dodge()
{
	if (!bCanDodge || !DodgeMontage) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	if (AnimInstance->Montage_IsPlaying(DodgeMontage)) return;

	bCanDodge = false;
	bIsDodging = true; // ✅ 회피 중 플래그 ON

	GetWorldTimerManager().SetTimer(DodgeCooldownTimerHandle, this, &AEchoPlayer::ResetDodgeCooldown, DodgeCooldown, false);

	AnimInstance->Montage_Play(DodgeMontage);

	// 루트모션 종료 시 이동 복원 + 회피 종료
	const float MontageLength = DodgeMontage->GetPlayLength();
	FTimerHandle EndHandle;
	GetWorldTimerManager().SetTimer(EndHandle, [this]()
		{
			bIsDodging = false; // ✅ 회피 끝
			//GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}, MontageLength-0.6f, false);
}


void AEchoPlayer::ResetDodgeCooldown()
{
	bCanDodge = true;
}
// 입력 바인딩
void AEchoPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// (입력 처리 바인딩은 아직 없음)
}


// 레벨 업 처리
void AEchoPlayer::HandleLevelUp(int32 NewLevel)
{
	UE_LOG(LogTemp, Log, TEXT("=== Level UP! Now Level %d"), NewLevel);
}

void AEchoPlayer::PlayPortalFocusMontage()
{
	if (!PortalFocusMontage) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	// 이동 막기 (루트모션만 허용)
	GetCharacterMovement()->DisableMovement();

	AnimInstance->Montage_Play(PortalFocusMontage, 1.f);
}

void AEchoPlayer::StopPortalFocusMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && PortalFocusMontage)
	{
		AnimInstance->Montage_Stop(0.2f, PortalFocusMontage);
	}

	// 다시 이동 가능하게
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void AEchoPlayer::PlayPortalTravelMontage()
{
	if (!PortalTravelMontage) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(PortalTravelMontage, 1.f);
	}
}

float AEchoPlayer::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	if (!Attributes) return 0.f;
	return Attributes->TakeDamage(DamageAmount);
}

void AEchoPlayer::UseConsumableSlot(int32 Index)
{
	if (Index < 0 || Index > 3) return;

	// 플레이어 인벤토리에서 Consumables 컨테이너 얻기
	if (UPlayerInventoryComponent* Inv = FindComponentByClass<UPlayerInventoryComponent>())
	{
		if (UConsumableContainerComponent* Cons = Inv->GetConsumables()) // 네 컴포넌트의 Getter 이름에 맞춰줘
		{
			Cons->ConsumeAt(Index, this);
		}
	}
}

