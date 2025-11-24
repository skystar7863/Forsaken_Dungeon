#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemGridWidget.generated.h"

class UUniformGridPanel;
class UItemContainerComponent;
class UItemSlotWidget;

/** 그리드 클릭 델리게이트(외부에서 바인딩 → ex. 퀵바 클릭 시 사용) */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGridSlotClicked, int32 /*Index*/);

UCLASS()
class ECHOOFMIND_API UItemGridWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** 컨테이너 연결 및 그리드 구성 */
    UFUNCTION(BlueprintCallable) void InitializeWithContainer(UItemContainerComponent* InContainer);

    /** 슬롯 위젯 클래스 (디자인 가능한) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UItemSlotWidget> ItemSlotWidgetClass;

    /** 읽기 전용(드래그/드롭/스왑 비활성) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    bool bReadOnly = false;

    /** 슬롯 좌상단 라벨(예: {"1","2","3","4"}) */
    UFUNCTION(BlueprintCallable) void SetKeyLabels(const TArray<FText>& InLabels);

    /** 슬롯 클릭 이벤트(읽기 전용 모드에서 사용) */
    FOnGridSlotClicked OnSlotClicked;

protected:
    UPROPERTY(meta = (BindWidget)) UUniformGridPanel* Grid;

    UPROPERTY() UItemContainerComponent* Container;

    /** 키 라벨 보관 */
    UPROPERTY() TArray<FText> KeyLabels;

    virtual void NativeConstruct() override;

    UFUNCTION() void Rebuild();
    void ClearGrid();

    /** 슬롯 위젯에서 넘어오는 클릭을 중계 */
    void HandleSlotClicked(int32 Index);
};
