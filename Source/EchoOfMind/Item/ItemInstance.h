#pragma once
#include "CoreMinimal.h"
#include "ItemEnums.h"
#include "ItemStats.h"
#include "ItemInstance.generated.h"

// 전방선언만으로도 되지만, DataAsset을 바로 쓰려면 포함해도 OK
class UItemDefinition;

USTRUCT(BlueprintType)
struct FItemInstance
{
    GENERATED_BODY()

    /** 정의(템 종류/아이콘/기본 스탯) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UItemDefinition* Def = nullptr;

    /** 희귀도 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EItemRarity Rarity = EItemRarity::Common;

    /** 소모품/열쇠 스택 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 StackCount = 1;

    /** 영웅:1개, 전설:2개 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<EExtraPerk> ExtraPerks;
};
