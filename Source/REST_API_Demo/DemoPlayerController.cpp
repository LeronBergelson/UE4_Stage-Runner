// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoPlayerController.h"
#include "REST_API_DEMO/REST_API_DemoCharacter.h"
#include "REST_API_DEMO/REST_API_DemoGameMode.h"
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
    
    FString PID = "1234";

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

    // Get Request
	Request->OnProcessRequestComplete().BindUObject(this, &ADemoPlayerController::OnProcessRequestComplete);
	Request->SetURL("http://localhost:8080/api/PlayerData/" + PID);
	//Request->SetVerb("POST"); // Post Request
	Request->SetVerb("GET"); //Get Request
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
   
    /*
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
    
    */
    
	Request->ProcessRequest();
}

void ADemoPlayerController::OnProcessRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success)
{
    //Sets Middle map location
    FVector Location = FVector::ZeroVector;
    Location.Z = 400.0f;
    FPlayerData PlayerData;
   
    if(Success)
    {
        UE_LOG(LogTemp, Warning, TEXT("SUCCESS %s"), *Response->GetContentAsString());
        // setup pawn
        
        PlayerData = ConvertToPlayerData(Response->GetContentAsString()); // Converts Contents in Json string to PlayerData Struct
        
        // if is valid, sets player location based on retrieved values  
        if(PlayerData.isvalid){
           UE_LOG(LogTemp, Warning, TEXT("SUCCESS %f"), PlayerData.Zcoord);
           Location.X = PlayerData.Xcoord;
           Location.Y = PlayerData.Ycoord;
           Location.Z = PlayerData.Zcoord;
        }
    }
    else
    {
    // spawn new pawnn at dafual location

        UE_LOG(LogTemp, Warning, TEXT("FAILED"));
    }
    
    if(AREST_API_DemoGameMode* GM = GetWorld()->GetAuthGameMode<AREST_API_DemoGameMode>())
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
                
        if(AREST_API_DemoCharacter* NewPawn = GetWorld()->SpawnActor<AREST_API_DemoCharacter>(GM->DefaultPawnClass, Location, FRotator::ZeroRotator, SpawnParams))
        {
            NewPawn->SetHealth(PlayerData.Health);
            Possess(NewPawn);
        }
    }
}

FPlayerData ADemoPlayerController::ConvertToPlayerData(const FString& ResponseString)
{
    FPlayerData PlayerData;
    
    if(!ResponseString.Contains("timestamp"))
    {
        FJsonObjectConverter::JsonObjectStringToUStruct(*ResponseString, &PlayerData, 0, 0);
    }
    
    return PlayerData;
}