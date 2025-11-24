#include "LobbyStashComponent.h"

ULobbyStashComponent::ULobbyStashComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	// 프로젝트의 컨테이너 초기화 규약에 맞춰 사이즈 설정
	// (UItemContainerComponent에 InitializeGrid가 없다면, Columns/Rows UPROPERTY로 대체)
	InitializeGrid(4, 5);
}
