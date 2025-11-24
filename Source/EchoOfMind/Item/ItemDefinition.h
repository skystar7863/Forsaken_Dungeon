#pragma once
#include "Engine/DataAsset.h"
#include "Item/ItemEnums.h"
#include "Item/ItemStats.h"
#include "ItemDefinition.generated.h"

UCLASS(BlueprintType)
class UItemDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName ItemId;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FText DisplayName;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) EItemType ItemType;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) UTexture2D* Icon;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FItemStats BaseStats; // “일반” 기준

    /** 등급 (테두리/효과용) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rarity")
    EItemRarity Rarity = EItemRarity::Common;
    // 예: 검 = 힘+10, 갑옷 = 활력+10, 헬멧=방어+5, 각반=활력+5, 신발=이속+5%
};
