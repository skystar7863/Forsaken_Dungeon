#include "Skill.h"
#include "EchoPlayer.h"

USkill::USkill()
{
	// 생성 시 UnlockLevel이 1 이하이면 자동 해금
	//bIsUnlocked = (UnlockLevel <= 1);
}

// 실제 스킬 실행 로직 (BP 또는 C++에서 구현)
void USkill::Execute_Implementation(AEchoPlayer* Player)
{
	// 기본 구현은 없음
}
