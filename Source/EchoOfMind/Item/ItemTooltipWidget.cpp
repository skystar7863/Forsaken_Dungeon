#include "ItemTooltipWidget.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

#include "ItemInstance.h"     // FItemInstance
#include "ItemDefinition.h"   // UItemDefinition, EItemRarity
#include "ItemStats.h"        // FItemStats (Strength/Agility/Intelligence/Vitality/Defense/MoveSpeedPct)

void UItemTooltipWidget::SetItem(const FItemInstance& Instance)
{
    if (!Instance.Def)
    {
        SetVisibility(ESlateVisibility::Collapsed);
        return;
    }

    const UItemDefinition* Def = Instance.Def;


    // ✅ 안전장치 추가
    FText NameTextValue = Def->DisplayName.IsEmpty() ? FText::FromString(TEXT("Unknown Item")) : Def->DisplayName;
    EItemRarity SafeRarity = Instance.Rarity != EItemRarity::Common ? Instance.Rarity : Def->Rarity;
    //EItemRarity SafeRarity = Def->Rarity; // 이미 기본값 Common 설정돼 있음 
    
    if (NameText)
        NameText->SetText(NameTextValue);

    /*if (RarityText)
    {
        FString RarityName = StaticEnum<EItemRarity>()->GetNameStringByValue((int64)Def->Rarity);
        RarityText->SetText(FText::FromString(RarityName));
    }*/

    if (RarityText)
    {
        FString RarityName = StaticEnum<EItemRarity>()->GetNameStringByValue((int64)SafeRarity);
        RarityText->SetText(FText::FromString(RarityName));
    }

    if (RarityBorder)
        RarityBorder->SetBrushColor(GetRarityColor(SafeRarity));

    // Base Stats 표기 (희귀도 배율 반영)
    const FItemStats& Base = Def->BaseStats;
    FItemStats FinalStats = Base;

    // 🔹 희귀도별 배율 계산
    float Multiplier = 1.f;
    switch (Instance.Rarity)
    {
    case EItemRarity::Rare:      Multiplier = 1.10f; break;
    case EItemRarity::Superior:  Multiplier = 1.25f; break;
    case EItemRarity::Heroic:    Multiplier = 1.30f; break;
    case EItemRarity::Legendary: Multiplier = 1.50f; break;
    default: break;
    }

    // 🔹 실제 계산
    FinalStats.Strength = Base.Strength * Multiplier;
    FinalStats.Agility = Base.Agility * Multiplier;
    FinalStats.Intelligence = Base.Intelligence * Multiplier;
    FinalStats.Vitality = Base.Vitality * Multiplier;
    FinalStats.Defense = Base.Defense * Multiplier;
    FinalStats.MoveSpeedPct = Base.MoveSpeedPct * Multiplier; // 이속은 퍼센트이므로 그대로 유지

    // 🔹 툴팁 표기
    SetLine(StrengthText, FinalStats.Strength, false, /*bHideZero=*/false);
    SetLine(AgilityText, FinalStats.Agility, false, false);
    SetLine(IntelligenceText, FinalStats.Intelligence, false, false);
    SetLine(VitalityText, FinalStats.Vitality, false, false);
    SetLine(DefenseText, FinalStats.Defense, false, false);
    SetLine(MoveSpeedText, FinalStats.MoveSpeedPct, true, false);



    FString ExtraLine;
    if (Instance.Rarity == EItemRarity::Heroic || Instance.Rarity == EItemRarity::Legendary)
    {
        for (EExtraPerk Perk : Instance.ExtraPerks)
        {
            switch (Perk)
            {
            case EExtraPerk::Plus5Pct_Str:         ExtraLine += TEXT("+5% Strength\n"); break;
            case EExtraPerk::Plus5Pct_Agi:         ExtraLine += TEXT("+5% Agility\n"); break;
            case EExtraPerk::Plus5Pct_Int:         ExtraLine += TEXT("+5% Intelligence\n"); break;
            case EExtraPerk::Plus5Pct_Vit:         ExtraLine += TEXT("+5% Vitality\n"); break;
            case EExtraPerk::Plus3Pct_MoveSpeed:   ExtraLine += TEXT("+3% Move Speed\n"); break;
            case EExtraPerk::Plus5_Defense:        ExtraLine += TEXT("+5 Defense\n"); break;
            case EExtraPerk::Bleed5for5s_NonWeapon:ExtraLine += TEXT("Bleed Damage \n"); break;
            default: break;
            }
        }
    }

    if (ExtraPerkText)
    {
        if (!ExtraLine.IsEmpty())
        {
            ExtraPerkText->SetText(FText::FromString(ExtraLine.TrimEnd()));
            ExtraPerkText->SetVisibility(ESlateVisibility::HitTestInvisible);
        }
        else
        {
            ExtraPerkText->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
    SetVisibility(ESlateVisibility::HitTestInvisible);
}

FLinearColor UItemTooltipWidget::GetRarityColor(EItemRarity Rarity)
{
    switch (Rarity)
    {
    case EItemRarity::Rare:       return FLinearColor(0.15f, 0.8f, 0.2f, 1.f);  // 초록
    case EItemRarity::Superior:   return FLinearColor(0.25f, 0.45f, 1.0f, 1.f); // 파랑
    case EItemRarity::Heroic:     return FLinearColor(0.7f, 0.3f, 0.9f, 1.f); // 보라
    case EItemRarity::Legendary:  return FLinearColor(1.0f, 0.3f, 0.2f, 1.f); // 주황/빨강
    default:                      return FLinearColor(0.4f, 0.4f, 0.4f, 1.f);   // 회색
    }
}

void UItemTooltipWidget::SetLine(UTextBlock* Line, float Value, bool bPercent, bool bHideZero)
{
    if (!Line) return;

    if (FMath::IsNearlyZero(Value))
    {
        if (bHideZero)
        {
            Line->SetVisibility(ESlateVisibility::Collapsed);
            return;
        }
        // 0도 보여주기
        Line->SetVisibility(ESlateVisibility::Visible);
        Line->SetText(FText::FromString(bPercent ? TEXT("0.0%") : TEXT("0")));
        return;
    }
    Line->SetVisibility(ESlateVisibility::Visible);

    const TCHAR* Sign = Value > 0.f ? TEXT("+") : TEXT("");
    FString Out;

    FString S = bPercent
        ? FString::Printf(TEXT("+%.1f%%"), Value)
        : FString::Printf(TEXT("+%.1f"), Value);

    Line->SetText(FText::FromString(S));
    //Line->SetVisibility(ESlateVisibility::HitTestInvisible);
}

