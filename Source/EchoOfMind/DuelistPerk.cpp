#include "DuelistPerk.h"
#include "EchoPlayer.h"
#include "AttributeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"

UDuelistPerk::UDuelistPerk()
{
    PerkName = FName("Duelist");
    Description = FText::FromString(
        "이동 속도 +10%, 가하는 피해 +10%, 받는 피해 +10%. "
        "피해 받으면 3초간 이동 속도 -20%.");
}

void UDuelistPerk::Apply_Implementation(AEchoPlayer* Player)
{
    if (!Player) return;
    OwnerPlayer = Player;

    // 1) AttributeComponent 찾아서 DamageMultiplier 조정
    if (UAttributeComponent* Attr = Player->FindComponentByClass<UAttributeComponent>())
    {
        Attr->OutgoingDamageMultiplier += 0.10f;
        Attr->IncomingDamageMultiplier += 0.10f;

        // 데미지 받을 때 이벤트 바인딩
        Attr->OnTakeDamage.AddDynamic(this, &UDuelistPerk::OnOwnerTakeDamage);
    }

    // 2) 캐릭터 이동 속도 +10%
    if (auto* MoveComp = Player->GetCharacterMovement())
    {
        BuffedSpeed = MoveComp->MaxWalkSpeed * 1.10f;
        MoveComp->MaxWalkSpeed = BuffedSpeed;
    }
}

void UDuelistPerk::OnOwnerTakeDamage(float DamageAmount)
{
    if (!OwnerPlayer.IsValid()) return;
    auto* MoveComp = OwnerPlayer->GetCharacterMovement();
    if (!MoveComp) return;

    // 기존 디버프 타이머 리셋
    OwnerPlayer->GetWorldTimerManager().ClearTimer(TimerHandle_RemoveDebuff);

    // 이동 속도 20% 감소
    MoveComp->MaxWalkSpeed = BuffedSpeed * 0.80f;

    // 3초 후 복구
    OwnerPlayer->GetWorldTimerManager().SetTimer(
        TimerHandle_RemoveDebuff,
        this,
        &UDuelistPerk::RestoreSpeed,
        3.f,
        false
    );
}

void UDuelistPerk::RestoreSpeed()
{
    if (!OwnerPlayer.IsValid()) return;
    if (auto* MoveComp = OwnerPlayer->GetCharacterMovement())
    {
        MoveComp->MaxWalkSpeed = BuffedSpeed;
    }
}
