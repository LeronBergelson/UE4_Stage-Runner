// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Http.h"
#include "DemoPlayerController.generated.h"


USTRUCT(BlueprintType)
struct FPlayerData
{
	GENERATED_BODY()
	UPROPERTY()
    FString email = ""; 
    UPROPERTY()
    FString userpassword = ""; 
	UPROPERTY()
	bool isvalid = false; //requires lowercase
	UPROPERTY()
	int pid = -1;
	UPROPERTY()
	float Health = 100.0f;
	UPROPERTY()
	float Xcoord = 0.0f;
	UPROPERTY()
	float Ycoord = 0.0f;
	UPROPERTY()
	float Zcoord = 0.0f;
};

UCLASS()
class REST_API_DEMO_API ADemoPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
    ADemoPlayerController();
    virtual void BeginPlay() override;
    void HandleServerEntry();
    
protected:
	FHttpModule* Http;
	void OnProcessRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success);
	FPlayerData ConvertToPlayerData(const FString& ResponseString);
	void SaveData();
};
