#pragma once
#include "CoreMinimal.h"
#include "EnemyAttackPattern.generated.h"

class UAnimMontage;

USTRUCT(BlueprintType)
struct FEnemyAttackPattern
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
    FName Name = TEXT("Attack");

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
    float Range = 200.f;          // 실행 거리

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
    float Cooldown = 3.f;         // 쿨다운(초)

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
    float DamageMultiplier = 1.f; // 기본 공격력에 곱

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
    bool bRootMotionLock = false; // 공격 동안 이동 잠금?

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
    UAnimMontage* Montage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
    FName HitNotifyName = TEXT("Hit"); // AnimNotify로 타이밍 잡기
};
