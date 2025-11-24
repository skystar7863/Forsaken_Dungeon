#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnemyTier.h"
#include "EnemyAttackPattern.h"
#include "LootRarityTable.h"
#include "Item/ItemDefinition.h"
#include "EnemyArchetype.generated.h"

UCLASS(BlueprintType)
class ECHOOFMIND_API UEnemyArchetype : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
    EEnemyTier Tier = EEnemyTier::Small;

    // 기본 능력치(이 값은 적 스폰 시 AttributeComponent에 적용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    float MaxHealth = 200.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    float AttackPower = 30.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    float MoveSpeed = 200.f;

    // 인지 세팅(필요시)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perception")
    float SightRadius = 1200.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perception")
    float LoseSightRadius = 1500.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perception")
    float PeripheralAngle = 90.f;

    // 공격 패턴 세트(소형 1, 중형 2, 보스 3 추천)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    TArray<FEnemyAttackPattern> AttackPatterns;

    // 드롭 관련
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot")
    int32 DropCount = 2; // Small=2, Medium=2, Boss=3

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot")
    ULootRarityTable* RarityTable = nullptr;

    /** 장비 풀(검/헬멧/갑옷/각반/신발 등) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot")
    TArray<UItemDefinition*> EquipmentPool;

    /** 소모품 풀(물약/열쇠 등) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot")
    TArray<UItemDefinition*> ConsumablePool;

    // === 추가 : XP 보상 ===
    /** 적 처치 시 지급되는 기본 XP 보상 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reward")
    float XPReward = 25.f;
};
