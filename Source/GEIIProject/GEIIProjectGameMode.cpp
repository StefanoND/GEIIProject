// Copyright Epic Games, Inc. All Rights Reserved.

#include "GEIIProjectGameMode.h"
#include "GEIIProjectHUD.h"
#include "GEIIProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGEIIProjectGameMode::AGEIIProjectGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AGEIIProjectHUD::StaticClass();
}
