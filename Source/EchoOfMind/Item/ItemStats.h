#pragma once
#include "CoreMinimal.h"
#include "ItemStats.generated.h"

USTRUCT(BlueprintType)
struct FItemStats
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Strength = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Agility = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Intelligence = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Vitality = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Defense = 0;
    /** +5% 는 5.0 으로 표기 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float MoveSpeedPct = 0.f;
};