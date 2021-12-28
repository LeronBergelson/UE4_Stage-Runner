// Fill out your copyright notice in the Description page of Project Settings.


#include "API_Info_GameInstance.h"

UAPI_Info_GameInstance::UAPI_Info_GameInstance() {
    userEmail = "";
    userPasswordVal = "";
}

void UAPI_Info_GameInstance::SetUserEmail(FString NewUserEmail) {

    userEmail = NewUserEmail; // set User Email
    //UE_LOG(LogTemp, Warning, TEXT("userEmail: %s"), *userEmail);

}

void UAPI_Info_GameInstance::SetUserPassword(FString NewUserPassword) {

    userPasswordVal = NewUserPassword; // set UserPassword
    //UE_LOG(LogTemp, Warning, TEXT("userPasswordVal: %s"), *userPasswordVal);

}

void UAPI_Info_GameInstance::SetPlayerHealth(int newHealth)
{
    playerHealth = newHealth;
}

void UAPI_Info_GameInstance::SetBlueStageAttempts(int attemptNum)
{
    blueStageAttempts = attemptNum;
}

void UAPI_Info_GameInstance::SetYellowStageAttempts(int attemptNum)
{
    yellowStageAttempts = attemptNum;
}

void UAPI_Info_GameInstance::SetRedStageAttempts(int attemptNum)
{
    redStageAttempts = attemptNum;
}
