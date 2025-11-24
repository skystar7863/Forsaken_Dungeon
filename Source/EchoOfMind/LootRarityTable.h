#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Item/ItemEnums.h"
#include "LootRarityTable.generated.h"

USTRUCT(BlueprintType)
struct FWeightedRarity
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EItemRarity Rarity = EItemRarity::Common;

    /** 가중치(확률). 합계 100이 아니어도 비율로 처리 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Weight = 0.f;
};

UCLASS(BlueprintType)
class ECHOOFMIND_API ULootRarityTable : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    /** 희귀도 가중치 목록(예: Small/Medium/Boss용 테이블 각각 생성) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FWeightedRarity> Weights;
};
