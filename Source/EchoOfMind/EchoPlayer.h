// 플레이어 캐릭터 클래스 헤더

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EchoPlayer.generated.h"

class AEchoPlayerController;
class APickupItem;
class UAttributeComponent;
class USkillComponent;
class UPerkComponent;
class UTexture2D;

UCLASS()
class ECHOOFMIND_API AEchoPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// 생성자: 기본값 설정
	AEchoPlayer();

	// 상호작용(줍기/버튼/레버 등)
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void PerformAction();

	// 플레이어 컨트롤러 반환
	UFUNCTION(BlueprintCallable, Category = "Player")
	AEchoPlayerController* GetPlayerController() const;

	// 현재 들고 있는 아이템
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	APickupItem* HeldItem = nullptr;

	// 아이템 줍기 처리
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void PickupItem(APickupItem* Item);

	// 공격 처리
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void Attack();

	// 회피 동작 실행
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Dodge();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal")
	UAnimMontage* PortalFocusMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal")
	UAnimMontage* PortalTravelMontage;

	UFUNCTION(BlueprintCallable) void UseConsumableSlot(int32 Index);

	// 포탈 정신집중 시작/종료
	void PlayPortalFocusMontage();
	void StopPortalFocusMontage();
	void PlayPortalTravelMontage();
protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

	// 상호작용 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float InteractionDistance = 1000.f;

	// 카메라 및 스프링암
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* SpringArmComponent;

	// ✅ 회피 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* DodgeMontage;

	// ✅ 회피 쿨타임
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float DodgeCooldown = 2.0f;

	// ✅ 현재 회피 가능 여부
	bool bCanDodge = true;

	bool bIsDodging = false;

	// 내부용 타이머
	FTimerHandle DodgeCooldownTimerHandle;

	void ResetDodgeCooldown();

	// 회전 속도 (마우스 감도)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	// 공격 범위/반경
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float AttackRange = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float AttackRadius = 100.f;

	// 레벨업 처리
	UFUNCTION()
	void HandleLevelUp(int32 NewLevel);


public:
	// 속성 컴포넌트 (체력, 공격력 등)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	UAttributeComponent* Attributes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPerkComponent* PerkComp;

	// 플레이어 초상 이미지 (UI에서 사용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UTexture2D* PortraitTexture = nullptr;

	// 편의 Getter
	UFUNCTION(BlueprintPure, Category = "UI")
	UTexture2D* GetPortraitTexture() const { return PortraitTexture; }

	// 스킬/특성 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkillComponent* SkillComp;

};
