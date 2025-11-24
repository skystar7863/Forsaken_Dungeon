#pragma once

#include "CoreMinimal.h"
#include "EchoPlayer.h"
#include "WarriorCharacter.generated.h"

class USkeletalMeshComponent;
class UAnimMontage;
class UAttributeComponent;
class UBoxComponent;
class UCameraShakeBase;

UCLASS()
class ECHOOFMIND_API AWarriorCharacter : public AEchoPlayer
{
    GENERATED_BODY()

public:
    AWarriorCharacter();
    virtual void Attack() override;

    UPROPERTY(EditDefaultsOnly, Category = "Sound")
    USoundBase* SwordSwingSound; // 휘두를 때

    UPROPERTY(EditDefaultsOnly, Category = "Sound")
    USoundBase* SwordHitSound;   // 타격 시


protected:
    virtual void BeginPlay() override;

    /** 오른손 칼 메시 */
    UPROPERTY(VisibleAnywhere, Category = "Weapon")
    USkeletalMeshComponent* SwordMesh;

    /** 칼 히트박스(Overlap으로 타격 판정) */
    UPROPERTY(VisibleAnywhere, Category = "Weapon")
    UBoxComponent* SwordHitBox;

    /** 콤보용 몽타주들 */
    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    TArray<UAnimMontage*> AttackMontages;

    /** 히트 시 카메라 흔들림 */
    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    TSubclassOf<UCameraShakeBase> HitCameraShake;

    /** 콤보 인덱스 */
    int32 CurrentAttackIndex;

    /** 몽타주 실행 중인지 */
    bool bIsAttacking;

    /** 현재 재생 중인 몽타주 */
    UAnimMontage* ActiveAttackMontage;

    /** 히트스톱 타이머(우리 몽타주 재개) */
    FTimerHandle HitPauseTimer;

    /** 콤보 초기화 타이머 */
    FTimerHandle ComboResetTimer;

    /** 이 공격 윈도우 동안 이미 맞춘 액터들(중복타격 방지) */
    TSet<TWeakObjectPtr<AActor>> HitActorsThisWindow;

    /** 공격 윈도우 ON/OFF */
    bool bHitWindowActive = false;

    /** 공격 윈도우 켜기/끄기 (기본: 몽타주 전체 구간 ON, 필요하면 AnimNotify로 좁힐 수 있음) */
    void EnableHitWindow();
    void DisableHitWindow();

    /** 콤보 인덱스 초기화 */
    void ResetCombo();

    /** 몽타주 종료 콜백 */
    UFUNCTION()
    void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    /** 일시정지된 몽타주 다시 재생 */
    void ResumeAttackMontage();

    /** 칼 히트박스 오버랩 시작 */
    UFUNCTION()
    void OnSwordBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
        const FHitResult& SweepResult);

    /** 히트 이펙트(히트스톱, 카메라 흔들림, 적 경직) */
    void ApplyHitEffects(AActor* Victim);

    /** (선택) 애님 노티파이로 세밀하게 컨트롤하고 싶다면 아래 두 개를 몽타주에 붙이면 됨 */
    UFUNCTION() void AnimNotify_HitStart() { EnableHitWindow(); }
    UFUNCTION() void AnimNotify_HitEnd() { DisableHitWindow(); }
};
