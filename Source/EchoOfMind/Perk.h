// Perk 클래스 베이스. Perk는 캐릭터에 부가 능력을 부여하는 시스템

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Perk.generated.h"

class AEchoPlayer;

UCLASS(Abstract, Blueprintable)
class ECHOOFMIND_API UPerk : public UObject
{
	GENERATED_BODY()

public:
	UPerk();

	// Perk 이름 (Key로도 사용 가능)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perk")
	FName PerkName;

	// Perk 설명 (UI용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perk")
	FText Description;

	// 해금 레벨 조건
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perk")
	int32 UnlockLevel = 1;

	// 효과 적용 함수 (BP Native Event로 확장 가능)
	UFUNCTION(BlueprintNativeEvent, Category = "Perk")
	void Apply(AEchoPlayer* Player);

	virtual void Apply_Implementation(AEchoPlayer* Player);
};
