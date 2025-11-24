// 스킬의 기본 베이스 클래스 (Blueprint 확장 가능)

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Skill.generated.h"

class AEchoPlayer;

UCLASS(Abstract, Blueprintable)
class ECHOOFMIND_API USkill : public UObject
{
	GENERATED_BODY()

public:
	USkill();

	/** 스킬 아이콘 (Widget 에서 브러시로 사용) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	UTexture2D* SkillIcon;

	// 스킬 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	FName SkillName;

	

	// 쿨다운 시간 (초 단위)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	float Cooldown = 1.f;

	

	// 마지막 사용 시간
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	float LastUseTime = -1000.f;

	// 스킬 실행 함수 (BP나 C++에서 오버라이드)
	UFUNCTION(BlueprintNativeEvent, Category = "Skill")
	void Execute(AEchoPlayer* Player);

	virtual void Execute_Implementation(AEchoPlayer* Player);
};
