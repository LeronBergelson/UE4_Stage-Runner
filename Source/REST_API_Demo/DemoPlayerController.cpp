// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoPlayerController.h"
#include "JsonObjectConverter.h"

ADemoPlayerController::ADemoPlayerController(){
	Http = &FHttpModule::Get();
}

void ADemoPlayerController::BeginPlay(){

    if(!HasAuthority())
    {
        UE_LOG(LogTemp, Warning, TEXT("RUNNING ON CLIENT"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("RUNNING ON SERVER"));
    }
}

void ADemoPlayerController::HandleServerEntry(){
    
    if(!HasAuthority()){
        return;
    }

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

    // Get Request
	Request->OnProcessRequestComplete().BindUObject(this, &ADemoPlayerController::OnProcessRequestComplete);
	Request->SetURL("http://localhost:8080/api/PlayerData");
	Request->SetVerb("POST"); // Post Request
	//	Request->SetVerb("GET"); //Get Request
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
   
    // Post Request Code {	
	FString JsonString;
	FPlayerData PlayerData;
	PlayerData.isvalid = true;
	PlayerData.Xcoord = 10.0f;
	PlayerData.Ycoord = 20.0f;
	PlayerData.Zcoord = 30.0f;
	
	FJsonObjectConverter::UStructToJsonObjectString(PlayerData, JsonString);
	Request->SetContentAsString(JsonString);
    UE_LOG(LogTemp, Warning, TEXT("Json String %s"), *JsonString);
    // }
    
	Request->ProcessRequest();
}

void ADemoPlayerController::OnProcessRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success)
{
    if(Success)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s"), *Response->GetContentAsString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("FAILED"));
    }
}