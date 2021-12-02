// Copyright Epic Games, Inc. All Rights Reserved.

#include "REST_API_DemoGameMode.h"
#include "REST_API_DemoCharacter.h"
#include "UObject/ConstructorHelpers.h"

AREST_API_DemoGameMode::AREST_API_DemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	
	Http = &FHttpModule::Get();
}

void AREST_API_DemoGameMode::PostLogin(APlayerController* NewPlayer) {
	Super::PostLogin(NewPlayer);

	UE_LOG(LogTemp, Warning, TEXT("Post Login Running"));

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

	Request->OnProcessRequestComplete().BindUObject(this, &AREST_API_DemoGameMode::OnProcessRequestComplete);
	Request->SetURL("http://localhost:8080/api/PlayerData");
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->ProcessRequest();
	
	//Get Request throught API passing in player ID
}

void AREST_API_DemoGameMode::OnProcessRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success)
{
	if (Success) {
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Response->GetContentAsString());

	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("FAILED"));
	}
}