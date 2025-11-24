#include "ItemMath.h"
#include "ItemDefinition.h"

using namespace ItemMath;

FItemStats ItemMath::ComputeFinalStats(const FItemInstance& Inst)
{
    FItemStats S{};
    if (!Inst.Def) return S;

    const FItemStats& B = Inst.Def->BaseStats;
    const float M = GetRarityMultiplier(Inst.Rarity);

    S.Strength = FMath::FloorToInt(B.Strength * M);
    S.Agility = FMath::FloorToInt(B.Agility * M);
    S.Intelligence = FMath::FloorToInt(B.Intelligence * M);
    S.Vitality = FMath::FloorToInt(B.Vitality * M);
    S.Defense = FMath::FloorToInt(B.Defense * M);
    S.MoveSpeedPct = B.MoveSpeedPct * M;

    for (EExtraPerk P : Inst.ExtraPerks)
    {
        switch (P)
        {
        case EExtraPerk::Plus5Pct_Str:       S.Strength = FMath::FloorToInt(S.Strength * 1.05f); break;
        case EExtraPerk::Plus5Pct_Agi:       S.Agility = FMath::FloorToInt(S.Agility * 1.05f); break;
        case EExtraPerk::Plus5Pct_Int:       S.Intelligence = FMath::FloorToInt(S.Intelligence * 1.05f); break;
        case EExtraPerk::Plus5Pct_Vit:       S.Vitality = FMath::FloorToInt(S.Vitality * 1.05f); break;
        case EExtraPerk::Plus3Pct_MoveSpeed: S.MoveSpeedPct += 3.f; break;
        case EExtraPerk::Plus5_Defense:      S.Defense += 5;   break;
        case EExtraPerk::Bleed5for5s_NonWeapon: /* 전투 처리로 넘김 */ break;
        }
    }
    return S;
}
