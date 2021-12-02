// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Http.h"
#include "REST_API_DemoGameMode.generated.h"

USTRUCT(BlueprintType)
struct FPlayerData
{
	GENERATED_BODY()
	float XCoord = 0.0f;
	float YCorrd = 0.0f;
	float ZCoord = 0.0f;
};

UCLASS(minimalapi)
class AREST_API_DemoGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AREST_API_DemoGameMode();
    virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
	FHttpModule* Http;
	void OnProcessRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success);
};



