#include "WarriorCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Animation/AnimInstance.h"
#include "DrawDebugHelpers.h"
#include "AttributeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Damage.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "AEnemyBase.h"

AWarriorCharacter::AWarriorCharacter()
{
    // 칼 메시
    SwordMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SwordMesh"));
    SwordMesh->SetupAttachment(GetMesh(), TEXT("RightHandSocket"));

    // 칼 히트박스 (칼 자식으로 붙임)
    SwordHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SwordHitBox"));
    SwordHitBox->SetupAttachment(SwordMesh);
    SwordHitBox->SetBoxExtent(FVector(6.f, 40.f, 4.f)); // 대충 칼날 두께/길이 감
    SwordHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 평소 비활성
    SwordHitBox->SetCollisionObjectType(ECC_WorldDynamic);
    SwordHitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    SwordHitBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 적 캡슐과 겹치면 히트
    SwordHitBox->SetGenerateOverlapEvents(true);

    CurrentAttackIndex = 0;
    bIsAttacking = false;
    ActiveAttackMontage = nullptr;
}

void AWarriorCharacter::BeginPlay()
{
    Super::BeginPlay();

    // 오버랩 바인딩
    if (SwordHitBox)
    {
        SwordHitBox->OnComponentBeginOverlap.AddDynamic(this, &AWarriorCharacter::OnSwordBeginOverlap);
        SwordHitBox->IgnoreActorWhenMoving(this, true); // 자기 자신 무시
    }
}

void AWarriorCharacter::EnableHitWindow()
{
    bHitWindowActive = true;
    HitActorsThisWindow.Empty();
    if (SwordHitBox)
    {
        SwordHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }
}

void AWarriorCharacter::DisableHitWindow()
{
    bHitWindowActive = false;
    if (SwordHitBox)
    {
        SwordHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    HitActorsThisWindow.Empty();
}

void AWarriorCharacter::Attack()
{
    if (bIsDodging) return;
    // 이미 공격 중이면 무시
    if (bIsAttacking) return;
    bIsAttacking = true;

    // 콤보 리셋 타이머 초기화
    GetWorldTimerManager().ClearTimer(ComboResetTimer);

    // 1) 몽타주 재생
    if (AttackMontages.IsValidIndex(CurrentAttackIndex))
    {
        UAnimMontage* MontageToPlay = AttackMontages[CurrentAttackIndex];
        if (MontageToPlay)
        {
            if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
            {
                ActiveAttackMontage = MontageToPlay;
                AnimInst->Montage_Play(MontageToPlay);

                FOnMontageEnded EndDel;
                EndDel.BindUObject(this, &AWarriorCharacter::OnAttackMontageEnded);
                AnimInst->Montage_SetEndDelegate(EndDel, MontageToPlay);

                // ✅ 공격 시작 시 휘두르는 소리 재생
                if (SwordSwingSound && SwordMesh)
                {
                    UGameplayStatics::SpawnSoundAttached(
                        SwordSwingSound,
                        SwordMesh,
                        NAME_None,
                        FVector::ZeroVector,
                        EAttachLocation::KeepRelativeOffset,
                        true);
                }
            }
        }

        // 다음 콤보 인덱스
        CurrentAttackIndex = (CurrentAttackIndex + 1) % FMath::Max(1, AttackMontages.Num());
    }

    // 2) 히트 윈도우 ON (기본: 몽타주 전체 / 세밀히 하려면 AnimNotify_HitStart/End 사용)
    EnableHitWindow();

    // 3) 3초 후 콤보 리셋 예약
    GetWorldTimerManager().SetTimer(ComboResetTimer, this, &AWarriorCharacter::ResetCombo, 3.f, false);
}

void AWarriorCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage == ActiveAttackMontage)
    {
        bIsAttacking = false;
        ActiveAttackMontage = nullptr;
        DisableHitWindow();
    }
}

void AWarriorCharacter::ResumeAttackMontage()
{
    if (ActiveAttackMontage)
    {
        if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
        {
            AnimInst->Montage_Resume(ActiveAttackMontage);
        }
    }
}

void AWarriorCharacter::ResetCombo()
{
    CurrentAttackIndex = 0;
}

void AWarriorCharacter::OnSwordBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!bHitWindowActive) return;
    if (!OtherActor || OtherActor == this) return;

    // 적만 타격
    AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor);
    if (!Enemy) return;

    // 한 윈도우에 1회만
    if (HitActorsThisWindow.Contains(Enemy)) return;

    // 데미지 계산
    const UAttributeComponent* MyAttr = Attributes;
    const float Base = MyAttr ? MyAttr->AttackPower : 20.f;
    const float Dmg = MyAttr ? (Base * MyAttr->OutgoingDamageMultiplier) : Base;

    UGameplayStatics::ApplyDamage(
        /* DamagedActor   */ Enemy,
        /* BaseDamage     */ Dmg,
        /* EventInstigator*/ GetController(),
        /* DamageCauser   */ this,
        /* DamageType     */ nullptr
    );

    // ✅ 검이 적과 부딪힐 때 타격음 재생
    if (SwordHitSound && SwordMesh)
    {
        UGameplayStatics::SpawnSoundAttached(
            SwordHitSound,
            SwordMesh,
            NAME_None,
            FVector::ZeroVector,
            EAttachLocation::KeepRelativeOffset,
            true);
    }

    // 히트 이펙트: 히트스톱 + 카메라 흔들림 + 적 경직
    ApplyHitEffects(Enemy);

    HitActorsThisWindow.Add(Enemy);
}

void AWarriorCharacter::ApplyHitEffects(AActor* Victim)
{
    // 1) 카메라 흔들림
    if (APlayerController* PC = GetController<APlayerController>())
    {
        if (PC->PlayerCameraManager && HitCameraShake)
        {
            PC->PlayerCameraManager->StartCameraShake(HitCameraShake, 1.f);
        }
    }

    // 2) 히트스톱(우리 공격 몽타주 0.2초 정지)
    if (ActiveAttackMontage)
    {
        if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
        {
            AnimInst->Montage_Pause(ActiveAttackMontage);
            GetWorldTimerManager().ClearTimer(HitPauseTimer);
            GetWorldTimerManager().SetTimer(HitPauseTimer, this, &AWarriorCharacter::ResumeAttackMontage, 0.2f, false);
        }
    }

    // 3) 적 경직(0.2초 동안 타임딜레이션 0.1)
    if (ACharacter* EnemyChar = Cast<ACharacter>(Victim))
    {
        EnemyChar->CustomTimeDilation = 0.1f;
        // 로컬 타이머로 0.2초 후 복구
        FTimerHandle Tmp;
        GetWorldTimerManager().SetTimer(Tmp, [EnemyChar]()
            {
                if (IsValid(EnemyChar))
                {
                    EnemyChar->CustomTimeDilation = 1.0f;
                }
            }, 0.2f, false);
    }
}
