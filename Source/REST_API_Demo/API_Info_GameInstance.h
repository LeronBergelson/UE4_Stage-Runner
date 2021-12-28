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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInfo")
    int playerHealth;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInfo")
    int blueStageAttempts;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInfo")
    int yellowStageAttempts;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInfo")
    int redStageAttempts;

public:

    UAPI_Info_GameInstance();
    
    //Getter
    FString getUserEmail() {return userEmail;}
    //Setter
    void SetUserEmail(FString NewUserEmail);

    //Getter
    UFUNCTION(BlueprintCallable)
    FString getUserPassword() {return userPasswordVal;}
    //Setter
    UFUNCTION(BlueprintCallable)
    void SetUserPassword(FString NewUserPassword);

    //Getter
    UFUNCTION(BlueprintCallable)
    int GetPlayerHealth() {return playerHealth;}
    //Setter
    UFUNCTION(BlueprintCallable)
    void SetPlayerHealth(int newHealth);

    //Getter
    UFUNCTION(BlueprintCallable)
    int GetBlueStageAttempts() {return blueStageAttempts;}
    //Setter
    UFUNCTION(BlueprintCallable)
    void SetBlueStageAttempts(int attemptNum);

    //Getter
    UFUNCTION(BlueprintCallable)
    int GetYellowStageAttempts() {return yellowStageAttempts;}
    //Setter
    UFUNCTION(BlueprintCallable)
    void SetYellowStageAttempts(int attemptNum);

    //Getter
    UFUNCTION(BlueprintCallable)
    int GetRedStageAttempts() {return redStageAttempts;}
    //Setter
    UFUNCTION(BlueprintCallable)
    void SetRedStageAttempts(int attemptNum);





};
