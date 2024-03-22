// Copyright Epic Games, Inc. All Rights Reserved.

#include "PSBGameMode.h"
#include "PSBCharacter.h"
#include "UObject/ConstructorHelpers.h"

APSBGameMode::APSBGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
