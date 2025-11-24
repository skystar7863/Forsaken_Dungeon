#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RunSessionSubsystem.h"  // ERunPhase
#include "PlayerStatusWidget.generated.h"


class UProgressBar;
class UImage;
class UTextBlock;
class AEchoPlayer;
class USkillComponent;
class UItemGridWidget;

UCLASS()
class ECHOOFMIND_API UPlayerStatusWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable) void SetHealthPercent(float P);
	UFUNCTION(BlueprintCallable) void SetManaPercent(float P);
	UFUNCTION() void HandleXPChanged(int32 Level, float CurrentXP);

protected:
	UPROPERTY(meta = (BindWidget)) UProgressBar* PB_Health;
	UPROPERTY(meta = (BindWidget)) UProgressBar* PB_Mana;
	UPROPERTY(meta = (BindWidget)) UProgressBar* PB_XP;

	UPROPERTY(meta = (BindWidget)) UImage* PortraitImage;

	// Skill slot widgets
	UPROPERTY(meta = (BindWidget)) UImage* Icon_Skill0;
	UPROPERTY(meta = (BindWidget)) UTextBlock* CDText_Skill0;
	UPROPERTY(meta = (BindWidget)) UImage* Icon_Skill1;
	UPROPERTY(meta = (BindWidget)) UTextBlock* CDText_Skill1;

	// 퀵바(읽기 전용)
	UPROPERTY(meta = (BindWidget)) UItemGridWidget* QuickbarGrid;

	// ===== 타이머 표시 =====
	UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* TimerText;
	UFUNCTION() void OnRunPhaseChanged(ERunPhase NewPhase);
	UFUNCTION() void OnRunTimerTick();

	void BindRunSession();
	void UpdateTimerText(int32 RemainSec);

private:
	TWeakObjectPtr<AEchoPlayer>     OwnerPlayer;
	TWeakObjectPtr<USkillComponent> SkillComp;

	// 슬롯마다 아이콘/쿨다운 업데이트
	void UpdateSkillSlot(int32 SlotIndex, UImage* Icon, UTextBlock* CDText);
	void RefreshPortrait();
};
