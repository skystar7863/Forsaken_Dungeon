#pragma once
#include "CoreMinimal.h"
#include "ItemEnums.h"
#include "ItemStats.h"
#include "ItemInstance.h"

namespace ItemMath
{
    inline float GetRarityMultiplier(EItemRarity R)
    {
        switch (R)
        {
        case EItemRarity::Rare:      return 1.10f;
        case EItemRarity::Superior:  return 1.25f;
        case EItemRarity::Heroic:    return 1.30f;
        case EItemRarity::Legendary: return 1.50f;
        default:                     return 1.00f;
        }
    }

    FItemStats ComputeFinalStats(const FItemInstance& Inst);
}
