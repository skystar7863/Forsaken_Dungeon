#pragma once

#include "CoreMinimal.h"
#include "Item/ItemContainerComponent.h"
#include "LobbyStashComponent.generated.h"

/** 로비에서 사용하는 세션-외 보관함 (5x4 = 20칸) */
UCLASS(ClassGroup = (Inventory), meta = (BlueprintSpawnableComponent))
class ECHOOFMIND_API ULobbyStashComponent : public UItemContainerComponent
{
	GENERATED_BODY()
public:
	ULobbyStashComponent();
};
