// Copyright Epic Games, Inc. All Rights Reserved.

#include "GettinItUpGameMode.h"
#include "GettinItUpCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGettinItUpGameMode::AGettinItUpGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
