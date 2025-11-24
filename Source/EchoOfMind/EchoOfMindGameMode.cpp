// Copyright Epic Games, Inc. All Rights Reserved.

#include "EchoOfMindGameMode.h"
#include "EchoPlayer.h"
#include "EchoPlayerController.h"
#include "UObject/ConstructorHelpers.h"

AEchoOfMindGameMode::AEchoOfMindGameMode()
{
	// set default pawn class to our Blueprinted character
//	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	
	
	PlayerControllerClass = AEchoPlayerController::StaticClass();
	
	DefaultPawnClass = AEchoPlayer::StaticClass();
	
}
