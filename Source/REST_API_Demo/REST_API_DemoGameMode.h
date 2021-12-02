// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Http.h"
#include "REST_API_DemoGameMode.generated.h"

UCLASS(minimalapi)
class AREST_API_DemoGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AREST_API_DemoGameMode();
    virtual void PostLogin(APlayerController* NewPlayer) override;
};



