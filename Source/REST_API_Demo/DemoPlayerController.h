// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Engine/EngineTypes.h"
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

USTRUCT(BlueprintType)
struct FStageData
{
	GENERATED_BODY()
	UPROPERTY()
	FString email = "";
	UPROPERTY()
	int bluestagecompletiontime = 0;
	UPROPERTY()
	int yellowstagecompletiontime = 0;
	UPROPERTY()
	int redstagecompletiontime = 0;
};

UCLASS()
class REST_API_DEMO_API ADemoPlayerController : public APlayerController
{
	GENERATED_BODY()
    
protected:
	FHttpModule* Http;
	
	UPROPERTY(Replicated)
	FString userEmail;

	UPROPERTY(Replicated)
    FString userPassword;

	UPROPERTY(Replicated)
	bool playerConnectEstablished;

	UPROPERTY(Replicated)
	bool canSaveData = true;
	
	UPROPERTY(Replicated)
	FTimerHandle TSaveHandle;
	
	FTimerDelegate TimerDel;
	
	void OnProcessRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success);
	FPlayerData ConvertToPlayerData(const FString& ResponseString);
	
	UFUNCTION()
	void SaveData(FString PlayerEmail, FString Password);
	
	UFUNCTION(Server, Reliable, WithValidation)
    void Server_SetUserEmail(const FString& NewUserEmail);
        
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_SetUserPassword(const FString& NewUserPassword); 

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    
public:
    ADemoPlayerController();
    virtual void BeginPlay() override;
    
	UFUNCTION(BlueprintCallable)
    void HandleServerEntry();    
	
	UFUNCTION(BlueprintCallable)
	bool GetPlayerConnectEstablished() {return playerConnectEstablished;}
	
	UFUNCTION(BlueprintCallable)
    void SetUserEmail(FString NewUserEmail);
	
	UFUNCTION(BlueprintCallable)
	FString GetUserEmail() const { return userEmail;}
        
    UFUNCTION(BlueprintCallable)
    void SetUserPassword(FString NewUserPassword);
	
	UFUNCTION(BlueprintCallable)
	FString GetUserPassword() const { return userPassword;}
	
	UFUNCTION(BlueprintCallable)
	bool SaveDataRepeater(bool activeState);
};
