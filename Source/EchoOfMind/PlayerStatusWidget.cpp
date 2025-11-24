#include "PlayerStatusWidget.h"

#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"
#include "RunSessionSubsystem.h"
#include "Kismet/GameplayStatics.h"

#include "EchoPlayer.h"
#include "SkillComponent.h"
#include "AttributeComponent.h"
#include "Skill.h"

// 인벤토리/퀵바
#include "Item/PlayerInventoryComponent.h"
#include "Item/ConsumableContainerComponent.h"
#include "Item/ItemInstance.h"
#include "Item/ItemGridWidget.h"

// 타이머
#include "RunSessionSubsystem.h"

static FString FormatMMSS(int32 Sec)
{
	Sec = FMath::Max(0, Sec);
	const int32 M = Sec / 60;
	const int32 S = Sec % 60;
	return FString::Printf(TEXT("%02d:%02d"), M, S);
}

void UPlayerStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerController* PC = GetOwningPlayer())
		if (AEchoPlayer* P = Cast<AEchoPlayer>(PC->GetPawn()))
		{
			OwnerPlayer = P;
			SkillComp = P->FindComponentByClass<USkillComponent>();

			if (UAttributeComponent* Attr = P->Attributes)
			{
				if (PB_Health) PB_Health->SetPercent(FMath::Clamp(Attr->Health / FMath::Max(Attr->MaxHealth, 1.f), 0.f, 1.f));
				if (PB_Mana)   PB_Mana->SetPercent(FMath::Clamp(Attr->Mana / FMath::Max(Attr->MaxMana, 1.f), 0.f, 1.f));
				if (PB_XP)     PB_XP->SetPercent(FMath::Clamp(Attr->CurrentXP / FMath::Max(Attr->XPPerLevel, 1.f), 0.f, 1.f));
			}

			// 퀵 소모품 그리드 연결 (읽기 전용)
			if (QuickbarGrid)
			{
				if (UPlayerInventoryComponent* Inv = P->FindComponentByClass<UPlayerInventoryComponent>())
					if (UConsumableContainerComponent* Cons = Inv->GetConsumables())
					{
						QuickbarGrid->bReadOnly = true;
						QuickbarGrid->InitializeWithContainer(Cons);
						QuickbarGrid->SetKeyLabels({
							FText::FromString(TEXT("1")),
							FText::FromString(TEXT("2")),
							FText::FromString(TEXT("3")),
							FText::FromString(TEXT("4"))
							});
						// 클릭 시 사용
						QuickbarGrid->OnSlotClicked.AddLambda([P](int32 Index)
							{
								P->UseConsumableSlot(Index);
							});
					}
			}
		}

	BindRunSession();
	RefreshPortrait();
}

void UPlayerStatusWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!SkillComp.IsValid()) return;
	UpdateSkillSlot(0, Icon_Skill0, CDText_Skill0);
	UpdateSkillSlot(1, Icon_Skill1, CDText_Skill1);
}

void UPlayerStatusWidget::UpdateSkillSlot(int32 SlotIndex, UImage* Icon, UTextBlock* CDText)
{
	if (!SkillComp.IsValid() || !Icon || !CDText) return;

	auto& Skills = SkillComp->ActiveSkills;
	if (!Skills.IsValidIndex(SlotIndex) || !Skills[SlotIndex])
	{
		Icon->SetOpacity(1.f);
		CDText->SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	USkill* SK = Skills[SlotIndex];

	if (SK->SkillIcon)
	{
		Icon->SetBrushFromTexture(SK->SkillIcon, true);
	}

	const float Now = GetWorld()->GetTimeSeconds();
	const float Elapsed = Now - SK->LastUseTime;
	const float Remain = SK->Cooldown - Elapsed;

	if (Remain > 0.f)
	{
		Icon->SetOpacity(0.5f);
		CDText->SetVisibility(ESlateVisibility::Visible);
		CDText->SetText(FText::AsNumber(FMath::CeilToInt(Remain)));
	}
	else
	{
		Icon->SetOpacity(1.f);
		CDText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPlayerStatusWidget::RefreshPortrait()
{
	if (!PortraitImage)       return;
	if (!OwnerPlayer.IsValid()) return;

	if (UTexture2D* Portrait = OwnerPlayer->GetPortraitTexture())
	{
		PortraitImage->SetBrushFromTexture(Portrait, true);
		PortraitImage->SetOpacity(1.f);
	}
	else
	{
		PortraitImage->SetOpacity(0.3f);
	}
}

void UPlayerStatusWidget::SetHealthPercent(float P)
{
	float Percent = P;
	if (OwnerPlayer.IsValid())
		if (auto* Attr = OwnerPlayer->Attributes)
		{
			Percent = P / FMath::Max(Attr->MaxHealth, 1.f);
			if (PB_Health) PB_Health->SetPercent(FMath::Clamp(Percent, 0.f, 1.f));
		}
}

void UPlayerStatusWidget::SetManaPercent(float P)
{
	float Percent = P;
	if (OwnerPlayer.IsValid())
		if (auto* Attr = OwnerPlayer->Attributes)
		{
			Percent = P / FMath::Max(Attr->MaxMana, 1.f);
			if (PB_Mana) PB_Mana->SetPercent(FMath::Clamp(Percent, 0.f, 1.f));
		}
}

void UPlayerStatusWidget::HandleXPChanged(int32 /*Level*/, float CurrentXP)
{
	if (!OwnerPlayer.IsValid() || !PB_XP) return;
	auto* Attr = OwnerPlayer->Attributes;
	const float Percent = CurrentXP / FMath::Max(Attr->XPPerLevel, 1.f);
	PB_XP->SetPercent(FMath::Clamp(Percent, 0.f, 1.f));
}

/* ----------------- 타이머 UI ----------------- */

void UPlayerStatusWidget::BindRunSession()
{
	if (!TimerText) return;

	if (UGameInstance* GI = GetGameInstance())
		if (URunSessionSubsystem* SS = GI->GetSubsystem<URunSessionSubsystem>())
		{
			// 최초 표시/숨김
			const bool bShow = (SS->Phase == ERunPhase::Dungeon || SS->Phase == ERunPhase::Boss);
			TimerText->SetVisibility(bShow ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
			UpdateTimerText(SS->GetRemainingSeconds());

			// 델리게이트 바인딩
			SS->OnPhaseChanged.AddDynamic(this, &UPlayerStatusWidget::OnRunPhaseChanged);
			SS->OnRunTimerTick.AddDynamic(this, &UPlayerStatusWidget::OnRunTimerTick);
		}
}

void UPlayerStatusWidget::OnRunPhaseChanged(ERunPhase NewPhase)
{
	if (!TimerText) return;

	const bool bShow = (NewPhase == ERunPhase::Dungeon || NewPhase == ERunPhase::Boss);
	TimerText->SetVisibility(bShow ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);

	// 색상은 취향껏
	if (NewPhase == ERunPhase::Dungeon)
	{
		TimerText->SetColorAndOpacity(FLinearColor(0.9f, 0.95f, 1.f)); // 밝은 하늘색
	}
	else if (NewPhase == ERunPhase::Boss)
	{
		TimerText->SetColorAndOpacity(FLinearColor(1.f, 0.75f, 0.2f));  // 황금색
	}
}

void UPlayerStatusWidget::OnRunTimerTick()
{
	if (UGameInstance* GI = GetGameInstance())
		if (URunSessionSubsystem* SS = GI->GetSubsystem<URunSessionSubsystem>())
		{
			UpdateTimerText(SS->GetRemainingSeconds());
		}
}

void UPlayerStatusWidget::UpdateTimerText(int32 RemainSec)
{
	if (!TimerText) return;
	TimerText->SetText(FText::FromString(FormatMMSS(RemainSec)));
}
