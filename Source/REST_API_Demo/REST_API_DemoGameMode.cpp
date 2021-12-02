// Copyright Epic Games, Inc. All Rights Reserved.

#include "REST_API_DemoGameMode.h"
#include "REST_API_DemoCharacter.h"
#include "DemoPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "JsonObjectConverter.h"


AREST_API_DemoGameMode::AREST_API_DemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AREST_API_DemoGameMode::PostLogin(APlayerController* NewPlayer) {
	Super::PostLogin(NewPlayer);
    
    if(ADemoPlayerController* PC = Cast<ADemoPlayerController>(NewPlayer))
    {
        PC->HandleServerEntry();
    }
}