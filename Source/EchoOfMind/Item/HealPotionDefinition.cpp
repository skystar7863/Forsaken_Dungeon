#include "HealPotionDefinition.h"
#include "EchoPlayer.h"
#include "AttributeComponent.h"
#include "TimerManager.h"

bool UHealPotionDefinition::Consume(AEchoPlayer* Player) const
{
    if (!Player) return false;

    UAttributeComponent* Attr = Player->FindComponentByClass<UAttributeComponent>();
    if (!Attr) return false;

    // 틱 간격/총 틱 수
    const float TickSec = 0.5f;
    const int32 TotalTicks = FMath::Max(1, FMath::RoundToInt(HealDuration / TickSec));
    const float PerTickHeal = HealTotal / float(TotalTicks);

    // 안전한 약포인터
    TWeakObjectPtr<AEchoPlayer>         WeakPlayer = Player;
    TWeakObjectPtr<UAttributeComponent> WeakAttr = Attr;

    // 기존 HoT 덮어쓰기(중첩 원하면 이 줄 제거)
    Player->GetWorldTimerManager().ClearTimer(Attr->PotionHoTHandle);
    Attr->PotionHoT_TicksLeft = TotalTicks;

    Player->GetWorldTimerManager().SetTimer(
        Attr->PotionHoTHandle,
        [WeakPlayer, WeakAttr, PerTickHeal]()
        {
            if (!WeakPlayer.IsValid() || !WeakAttr.IsValid())
            {
                if (WeakPlayer.IsValid())
                {
                    WeakPlayer->GetWorldTimerManager().ClearTimer(WeakAttr->PotionHoTHandle);
                }
                return;
            }

            WeakAttr->AddHealth(PerTickHeal);

            WeakAttr->PotionHoT_TicksLeft = FMath::Max(0, WeakAttr->PotionHoT_TicksLeft - 1);
            if (WeakAttr->PotionHoT_TicksLeft <= 0)
            {
                WeakPlayer->GetWorldTimerManager().ClearTimer(WeakAttr->PotionHoTHandle);
            }
        },
        TickSec, true
    );

    return true;
}
