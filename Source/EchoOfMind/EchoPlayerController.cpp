// 플레이어 컨트롤러 구현

#include "EchoPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "Blueprint/UserWidget.h"

#include "EchoPlayer.h"
#include "SkillComponent.h"
#include "Puzzle/InventoryWidget.h"
#include "PlayerStatusWidget.h"
#include "Item/InventoryRootWidget.h"
#include "AttributeComponent.h"

AEchoPlayerController::AEchoPlayerController()
{
	// Tick을 매 프레임 호출할 필요가 없으므로 비활성화
	// PrimaryActorTick.bCanEverTick = true;
}

void AEchoPlayerController::EnsureInventoryRoot()
{
	if (!InventoryRoot && InventoryRootClass)
	{
		InventoryRoot = CreateWidget<UInventoryRootWidget>(this, InventoryRootClass);
		if (InventoryRoot)
		{
			InventoryRoot->AddToViewport(10);
			InventoryRoot->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

// EchoPlayerController.cpp
void AEchoPlayerController::ShowInventory(bool bShow)
{
	EnsureInventoryRoot();
	if (!InventoryRoot) return;

	if (bShow)
	{
		bInventoryOpen = true;

		InventoryRoot->ShowInventoryOnly();
		InventoryRoot->SetVisibility(ESlateVisibility::Visible);
		bShowMouseCursor = true;

		// ✅ Game + UI 모드 (UIOnly 아님!)
		FInputModeGameAndUI Mode;
		Mode.SetWidgetToFocus(InventoryRoot->TakeWidget());
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(Mode);

		SetIgnoreMoveInput(true);
		SetIgnoreLookInput(true);
	}
	else
	{
		bInventoryOpen = false;

		InventoryRoot->CloseLoot();
		InventoryRoot->SetVisibility(ESlateVisibility::Hidden);
		bShowMouseCursor = false;

		// ✅ 게임 입력 복원
		FInputModeGameOnly Mode;
		SetInputMode(Mode);

		SetIgnoreMoveInput(false);
		SetIgnoreLookInput(false);

		// 🔕 인벤 닫고 F/Esc 입력 잔상 방지
		// BeginInventoryDebounce();
		if (PlayerInput)
		{
			PlayerInput->FlushPressedKeys();
		}
	}
}


void AEchoPlayerController::OnCloseInventoryRequested()
{
	if (bInventoryOpen)
	{
		ShowInventory(false);
	}
}





void AEchoPlayerController::BeginPlay()
{
	Super::BeginPlay();
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
	// 입력 서브시스템 설정
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (auto* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (IMC_Player)
			{
				Subsystem->AddMappingContext(IMC_Player, 0);
			}
		}
	}

	// 크로스헤어 UI
	if (IsLocalController() && CrosshairWidgetClass)
	{
		CrosshairWidget = CreateWidget<UUserWidget>(this, CrosshairWidgetClass);
		if (CrosshairWidget) CrosshairWidget->AddToViewport();
	}

	// (구) 인벤토리 위젯 — 필요 시 유지
	if (InventoryWidgetClass)
	{
		UUserWidget* Raw = CreateWidget<UUserWidget>(this, InventoryWidgetClass);
		InventoryWidget = Cast<UInventoryWidget>(Raw);
		if (InventoryWidget) InventoryWidget->AddToViewport();
	}

	// 플레이어 상태 UI
	if (PlayerStatusWidgetClass)
	{
		UPlayerStatusWidget* StatusWidget = CreateWidget<UPlayerStatusWidget>(this, PlayerStatusWidgetClass);
		PlayerStatusWidget = StatusWidget;
		if (StatusWidget)
		{
			StatusWidget->AddToViewport();

			// AttributeComponent 델리게이트 바인딩
			if (AEchoPlayer* P = Cast<AEchoPlayer>(GetPawn()))
			{
				if (P->Attributes)
				{
					P->Attributes->OnHealthChanged.AddDynamic(StatusWidget, &UPlayerStatusWidget::SetHealthPercent);
					P->Attributes->OnManaChanged.AddDynamic(StatusWidget, &UPlayerStatusWidget::SetManaPercent);
					P->Attributes->OnXPChanged.AddDynamic(StatusWidget, &UPlayerStatusWidget::HandleXPChanged);
				}
			}
		}
	}
}

void AEchoPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	// 각 입력 액션 바인딩
	if (IA_Move)            EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AEchoPlayerController::OnMoveTriggered);
	if (IA_Look)            EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AEchoPlayerController::OnLookTriggered);
	if (IA_Dodge)
		EnhancedInputComponent->BindAction(IA_Dodge, ETriggerEvent::Started, this, &AEchoPlayerController::OnDodgeTriggered);

	if (IA_Action)          EnhancedInputComponent->BindAction(IA_Action, ETriggerEvent::Started, this, &AEchoPlayerController::OnActionTriggered);
	if (IA_Attack)          EnhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Started, this, &AEchoPlayerController::OnAttackTriggered);
	if (IA_Skill1)          EnhancedInputComponent->BindAction(IA_Skill1, ETriggerEvent::Started, this, &AEchoPlayerController::OnSkill1Triggered);
	if (IA_Skill2)          EnhancedInputComponent->BindAction(IA_Skill2, ETriggerEvent::Started, this, &AEchoPlayerController::OnSkill2Triggered);
	if (IA_UseQuick1)       EnhancedInputComponent->BindAction(IA_UseQuick1, ETriggerEvent::Started, this, &AEchoPlayerController::OnUseQuick1Triggered);
	if (IA_UseQuick2)       EnhancedInputComponent->BindAction(IA_UseQuick2, ETriggerEvent::Started, this, &AEchoPlayerController::OnUseQuick2Triggered);
	if (IA_UseQuick3)       EnhancedInputComponent->BindAction(IA_UseQuick3, ETriggerEvent::Started, this, &AEchoPlayerController::OnUseQuick3Triggered);
	if (IA_UseQuick4)       EnhancedInputComponent->BindAction(IA_UseQuick4, ETriggerEvent::Started, this, &AEchoPlayerController::OnUseQuick4Triggered);
	if (IA_ToggleInventory) EnhancedInputComponent->BindAction(IA_ToggleInventory, ETriggerEvent::Started, this, &AEchoPlayerController::OnToggleInventory);
	// 닫기용 키 (‘I’, ‘F’, ‘Tab’, ‘Esc’) 를 동일 액션에 연결하거나 별도 입력으로
	EnhancedInputComponent->BindAction(IA_CloseInventory, ETriggerEvent::Started, this, &AEchoPlayerController::OnCloseInventoryRequested);
}

void AEchoPlayerController::OnToggleInventory(const FInputActionValue& /*value*/)
{
	EnsureInventoryRoot();
	if (!InventoryRoot) return;

	const bool bIsVisible = (InventoryRoot->GetVisibility() == ESlateVisibility::Visible);
	ShowInventory(!bIsVisible);
}

// 이동
void AEchoPlayerController::OnMoveTriggered(const FInputActionValue& Value)
{
	if (APawn* P = GetPawn())
	{
		const FVector Dir = Value.Get<FVector>();
		const FRotator YawRot(0.f, GetControlRotation().Yaw, 0.f);
		P->AddMovementInput(YawRot.RotateVector(Dir));
	}
}

// Look
void AEchoPlayerController::OnLookTriggered(const FInputActionValue& Value)
{
	const FVector2D Delta = Value.Get<FVector2D>();
	AddYawInput(Delta.X);
	AddPitchInput(Delta.Y);
}

// 점프
//void AEchoPlayerController::OnJumpTriggered(const FInputActionValue& /*Value*/)
//{
//	if (AEchoPlayer* P = Cast<AEchoPlayer>(GetPawn()))
//	{
//		P->Jump();
//	}
//}

void AEchoPlayerController::OnDodgeTriggered(const FInputActionValue& /*Value*/)
{
	if (AEchoPlayer* P = Cast<AEchoPlayer>(GetPawn()))
	{
		P->Dodge();
	}
}


// 상호작용
void AEchoPlayerController::OnActionTriggered(const FInputActionValue& /*Value*/)
{

	if (bInventoryOpen || bRecentlyClosedInventory) return;  // ⛔ 인벤 중/바로 닫은 직후엔 무시
	if (AEchoPlayer* P = Cast<AEchoPlayer>(GetPawn()))
	{
		P->PerformAction();
	}
}

// 기본 공격
void AEchoPlayerController::OnAttackTriggered(const FInputActionValue& /*Value*/)
{

	if (bInventoryOpen || bRecentlyClosedInventory) return;  // ⛔
	if (AEchoPlayer* P = Cast<AEchoPlayer>(GetPawn()))
	{
		P->Attack();
	}
}

// 예: 공격

// 스킬
void AEchoPlayerController::OnSkill1Triggered(const FInputActionValue& /*Value*/)
{
	if (AEchoPlayer* P = Cast<AEchoPlayer>(GetPawn()))
	{
		P->SkillComp->UseSkill(0);
	}
}
void AEchoPlayerController::OnSkill2Triggered(const FInputActionValue& /*Value*/)
{
	if (AEchoPlayer* P = Cast<AEchoPlayer>(GetPawn()))
	{
		P->SkillComp->UseSkill(1);
	}
}

// 소비 아이템
void AEchoPlayerController::OnUseQuick1Triggered(const FInputActionValue& /*Value*/)
{
	if (AEchoPlayer* P = GetPawn<AEchoPlayer>()) P->UseConsumableSlot(0);
}
void AEchoPlayerController::OnUseQuick2Triggered(const FInputActionValue& /*Value*/)
{
	if (AEchoPlayer* P = GetPawn<AEchoPlayer>()) P->UseConsumableSlot(1);
}
void AEchoPlayerController::OnUseQuick3Triggered(const FInputActionValue& /*Value*/)
{
	if (AEchoPlayer* P = GetPawn<AEchoPlayer>()) P->UseConsumableSlot(2);
}
void AEchoPlayerController::OnUseQuick4Triggered(const FInputActionValue& /*Value*/)
{
	if (AEchoPlayer* P = GetPawn<AEchoPlayer>()) P->UseConsumableSlot(3);
}
