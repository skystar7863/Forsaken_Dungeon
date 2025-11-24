#include "BloodResurgenceSkill.h"
#include "EchoPlayer.h"
#include "AttributeComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"

UBloodResurgenceSkill::UBloodResurgenceSkill()
{
    SkillName = FName("Blood Resurgence");
    Cooldown = 300.f;   // 60초 쿨다운
    Duration = 20.f;
    TotalHealRatio = 0.3f;   // 최대체력의 40%
    LastUseTime = -1000.f;
}

void UBloodResurgenceSkill::Execute_Implementation(AEchoPlayer* Player)
{
    if (!Player) return;

    // 1) 현재 체력의 5%를 즉시 감소
    UAttributeComponent* AC = Player->FindComponentByClass<UAttributeComponent>();
    if (AC)
    {
        float current = AC->Health;
        float damage = current * 0.05f;
        // Defense 무시하고 직접 체력 감소
        AC->Health = FMath::Clamp(current - damage, 0.f, AC->MaxHealth);
        AC->BroadcastHealth();
    }

    // 2) HealOverTime 초기화
    OwnerPlayer = Player;
    AttrComp = AC;
    if (!AttrComp.IsValid()) return;

    // Tick 설정 (1초 간격)
    TotalTicks = FMath::Max(1, FMath::FloorToInt(Duration));
    TicksDone = 0;
    HealPerTick = AttrComp->MaxHealth * TotalHealRatio / float(TotalTicks);

    Player->GetWorld()->GetTimerManager().SetTimer(
        TimerHandle_Heal,
        this,
        &UBloodResurgenceSkill::DoHealTick,
        Duration / float(TotalTicks),
        true
    );
}

void UBloodResurgenceSkill::DoHealTick()
{
    if (!AttrComp.IsValid() || !OwnerPlayer.IsValid())
    {
        if (OwnerPlayer.IsValid())
            OwnerPlayer->GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Heal);
        return;
    }

    // 실제 회복
    AttrComp->Heal(HealPerTick);
    TicksDone++;

    // 끝나면 타이머 종료
    if (TicksDone >= TotalTicks)
    {
        OwnerPlayer->GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Heal);
    }
}
