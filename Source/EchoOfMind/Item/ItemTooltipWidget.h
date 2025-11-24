#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemTooltipWidget.generated.h"

class UTextBlock;
class UBorder;
struct FItemInstance;
enum class EItemRarity : uint8;

/** 아이템 능력치/희귀도 툴팁 */
UCLASS()
class ECHOOFMIND_API UItemTooltipWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** 툴팁 채우기 (nullptr면 숨김 처리 권장) */
    UFUNCTION(BlueprintCallable)
    void SetItem(const FItemInstance& Instance);


    /** 희귀도 텍스트/색상 계산만 필요하면 호출 */
    UFUNCTION(BlueprintPure)
    static FLinearColor GetRarityColor(EItemRarity Rarity);

protected:
    // BP 바인딩 (디자인은 자유)
    UPROPERTY(meta = (BindWidget)) UTextBlock* NameText = nullptr;
    UPROPERTY(meta = (BindWidget)) UTextBlock* RarityText = nullptr;
    UPROPERTY(meta = (BindWidget)) UBorder* RarityBorder = nullptr;

    // 스탯 텍스트들(값 0이면 Hidden 처리)
    UPROPERTY(meta = (BindWidget)) UTextBlock* StrengthText = nullptr;
    UPROPERTY(meta = (BindWidget)) UTextBlock* AgilityText = nullptr;
    UPROPERTY(meta = (BindWidget)) UTextBlock* IntelligenceText = nullptr;
    UPROPERTY(meta = (BindWidget)) UTextBlock* VitalityText = nullptr;
    UPROPERTY(meta = (BindWidget)) UTextBlock* DefenseText = nullptr;
    UPROPERTY(meta = (BindWidget)) UTextBlock* MoveSpeedText = nullptr;
    // 추가 라인
UPROPERTY(meta = (BindWidgetOptional))
UTextBlock* ExtraPerkText = nullptr;


private:
    // 기존 SetLine에 옵션 하나 추가: bHideZero (기본값 false로 두면 0도 표시)
    void SetLine(class UTextBlock* Line, float Value, bool bPercent = false, bool bHideZero = false);

};
