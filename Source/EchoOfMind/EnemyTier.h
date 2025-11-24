#pragma once
#include "EnemyTier.generated.h"

UENUM(BlueprintType)
enum class EEnemyTier : uint8
{
    Small  UMETA(DisplayName = "Small"),
    Medium UMETA(DisplayName = "Medium"),
    Boss   UMETA(DisplayName = "Boss")
};
