#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemDragPayload.generated.h"

class UItemContainerComponent;

UCLASS()
class ECHOOFMIND_API UItemDragPayload : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY() UItemContainerComponent* SourceContainer = nullptr;
    UPROPERTY() int32 SourceIndex = INDEX_NONE;
};
