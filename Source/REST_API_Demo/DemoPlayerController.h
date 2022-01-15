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
	bool isvalid = false; //requires lowercase
	UPROPERTY()
	float health = 100.0f;
	UPROPERTY()
	float xcoord = 0.0f;
	UPROPERTY()
	float ycoord = 0.0f;
	UPROPERTY()
	float zcoord = 0.0f;
	UPROPERTY()
	int bluestageattempts = 0;
	UPROPERTY()
	int yellowstageattempts = 0;
	UPROPERTY()
	int redstageattempts = 0;
};

USTRUCT(BlueprintType)
struct FCourseData
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

USTRUCT(BlueprintType)
struct FSignInData
{
	GENERATED_BODY()
	UPROPERTY()
	FString username = "";
	UPROPERTY()
	FString password = "";
};

USTRUCT(BlueprintType)
struct FAuthenticationResponse
{
	GENERATED_BODY()
	UPROPERTY()
	FString jwt = "";
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
	bool canSaveData;
	
	UPROPERTY(Replicated)
	FTimerHandle TSaveHandle;
	
	FTimerDelegate TimerDel;
	
	void OnProcessRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success);
	FPlayerData ConvertToPlayerData(const FString& ResponseString);
	
	void OnProcessSignInRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success);
	FAuthenticationResponse ConvertToJWT(const FString& ResponseString);

	UFUNCTION(BlueprintCallable)
	void SaveData();
	
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
	void SaveCourseData(int blueStageCompletionTime, int yellowStageCompletionTime, int redStageCompletionTime);
	
	UFUNCTION(BlueprintCallable)
	bool GetPlayerConnectEstablished() {return playerConnectEstablished;}
	
	UFUNCTION(BlueprintCallable)
	bool SetCanSaveData(bool CanSave) {return canSaveData = CanSave;}

	UFUNCTION(BlueprintCallable)
    void SetUserEmail(FString NewUserEmail);
	
	UFUNCTION(BlueprintCallable)
	FString GetUserEmail() const {return userEmail;}
        
    UFUNCTION(BlueprintCallable)
    void SetUserPassword(FString NewUserPassword);
	
	UFUNCTION(BlueprintCallable)
	FString GetUserPassword() const {return userPassword;}
	
	UFUNCTION(BlueprintCallable)
	void UserSignIn(FString username, FString password);

	UFUNCTION(BlueprintCallable)
	void CreatePlayer();

	UFUNCTION(BlueprintCallable)
	bool SaveDataRepeater(bool activeState);
};
