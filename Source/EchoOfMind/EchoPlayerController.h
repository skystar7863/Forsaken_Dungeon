// 플레이어 컨트롤러 클래스 헤더

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EchoPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
class UUserWidget;
class UInventoryWidget;
class UPlayerStatusWidget;
class UInventoryRootWidget;

UCLASS()
class ECHOOFMIND_API AEchoPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AEchoPlayerController();

	// 인벤토리(옛) 위젯 접근(필요 시)
	UFUNCTION(BlueprintCallable, Category = "UI")
	UInventoryWidget* GetInventoryWidget() const { return InventoryWidget; }

	// 플레이어 상태 위젯 접근
	UFUNCTION(BlueprintCallable, Category = "UI")
	UPlayerStatusWidget* GetPlayerStatusWidget() const { return PlayerStatusWidget; }

	// 인벤토리 Root 열기/닫기
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowInventory(bool bShow = true);

	// Root 위젯 포인터
	UFUNCTION(BlueprintCallable, Category = "UI")
	UInventoryRootWidget* GetInventoryRoot() const { return InventoryRoot; }

	UFUNCTION()
	void OnCloseInventoryRequested();

	// 입력 상태 관리
	bool bInventoryOpen = false;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	// 입력 매핑 컨텍스트 및 액션
	UPROPERTY(EditAnywhere)
	UInputMappingContext* IMC_Player;

	UPROPERTY(EditAnywhere) UInputAction* IA_Move;
	UPROPERTY(EditAnywhere) UInputAction* IA_Look; 
	UPROPERTY(EditAnywhere) UInputAction* IA_Dodge;

	UPROPERTY(EditAnywhere) UInputAction* IA_Action;
	UPROPERTY(EditAnywhere) UInputAction* IA_Attack;
	UPROPERTY(EditAnywhere) UInputAction* IA_Skill1;
	UPROPERTY(EditAnywhere) UInputAction* IA_Skill2;
	UPROPERTY(EditAnywhere) UInputAction* IA_UseQuick1;
	UPROPERTY(EditAnywhere) UInputAction* IA_UseQuick2;
	UPROPERTY(EditAnywhere) UInputAction* IA_UseQuick3;
	UPROPERTY(EditAnywhere) UInputAction* IA_UseQuick4;

	// 인벤토리 토글 액션(I)
	UPROPERTY(EditAnywhere) UInputAction* IA_ToggleInventory;
	UPROPERTY(EditAnywhere) UInputAction* IA_CloseInventory; // (Esc 등)

	UFUNCTION()
	void OnToggleInventory(const FInputActionValue& value);

	// Root 위젯 클래스/인스턴스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UInventoryRootWidget> InventoryRootClass; // BP_InventoryRoot 지정

	UPROPERTY() UInventoryRootWidget* InventoryRoot = nullptr;

	// 기타 UI 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> CrosshairWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> InventoryWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UPlayerStatusWidget> PlayerStatusWidgetClass;


	bool IsInventoryOpen() const { return bInventoryOpen; }

private:
	// 입력 콜백
	UFUNCTION() void OnMoveTriggered(const FInputActionValue& Value);
	UFUNCTION() void OnLookTriggered(const FInputActionValue& Value);
	//UFUNCTION() void OnJumpTriggered(const FInputActionValue& Value);
	UFUNCTION() void OnDodgeTriggered(const FInputActionValue& Value);
	UFUNCTION() void OnActionTriggered(const FInputActionValue& Value);
	UFUNCTION() void OnAttackTriggered(const FInputActionValue& Value);
	UFUNCTION() void OnSkill1Triggered(const FInputActionValue& Value);
	UFUNCTION() void OnSkill2Triggered(const FInputActionValue& Value);
	UFUNCTION() void OnUseQuick1Triggered(const FInputActionValue& Value);
	UFUNCTION() void OnUseQuick2Triggered(const FInputActionValue& Value);
	UFUNCTION() void OnUseQuick3Triggered(const FInputActionValue& Value);
	UFUNCTION() void OnUseQuick4Triggered(const FInputActionValue& Value);

	// 위젯 인스턴스
	UPROPERTY() UUserWidget* CrosshairWidget = nullptr;
	UPROPERTY() UInventoryWidget* InventoryWidget = nullptr; // (이전 인벤토리 — 필요시 사용)
	UPROPERTY() UPlayerStatusWidget* PlayerStatusWidget = nullptr;

	// ✅ Root 위젯 생성 보장
	void EnsureInventoryRoot();

	bool bRecentlyClosedInventory = false;
	FTimerHandle RecentlyClosedTimerHandle;

	//void BeginInventoryDebounce()
	//{
	//	bRecentlyClosedInventory = true;
	//	// 0.15s 후 해제 (한 프레임만으로 부족할 때 대비)
	//	GetWorldTimerManager().SetTimer(RecentlyClosedTimerHandle, [this]()
	//		{
	//			bRecentlyClosedInventory = false;
	//		}, 0.15f, false);
	//	// 키 눌림 버퍼도 비움
	//	if (PlayerInput) { PlayerInput->FlushPressedKeys(0.f); }
	//}
};
