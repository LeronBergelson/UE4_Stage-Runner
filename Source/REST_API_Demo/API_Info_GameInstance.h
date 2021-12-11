// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "API_Info_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class REST_API_DEMO_API UAPI_Info_GameInstance : public UGameInstance
{
	GENERATED_BODY()
protected:

    UPROPERTY()
        FString userEmail;
    UPROPERTY()
        FString userPasswordVal;
    	
public:

    UAPI_Info_GameInstance();
    
    FString getUserEmail() { return userEmail;}
    void SetUserEmail(FString NewUserEmail);

    FString getUserPassword() { return userPasswordVal; }
    void SetUserPassword(FString NewUserPassword);

};
