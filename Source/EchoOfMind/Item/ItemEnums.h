#pragma once
#include "CoreMinimal.h"
#include "ItemEnums.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
    Weapon_Sword,
    Weapon_Staff,
    Helmet,
    Chest,
    Leg,
    Boots,
    Consumable_HP,
    Key_Small,
    Key_Medium,
    Key_Large
};

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
    Consumable, // ¼Ò¸ðÇ°
    Common,     // ÀÏ¹Ý
    Rare,       // Èñ±Í
    Superior,   // °í±Þ
    Heroic,     // ¿µ¿õ
    Legendary   // Àü¼³
};

UENUM(BlueprintType)
enum class EExtraPerk : uint8
{
    Plus5Pct_Str,
    Plus5Pct_Agi,
    Plus5Pct_Int,
    Plus5Pct_Vit,
    Plus3Pct_MoveSpeed,
    Plus5_Defense,
    Bleed5for5s_NonWeapon
};
