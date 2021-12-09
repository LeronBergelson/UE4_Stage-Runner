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
        FTimerHandle TSaveHandle;
        GetWorldTimerManager().SetTimer(TSaveHandle, this, &ADemoPlayerController::SaveData, 5.0f, true);
    }
}

void ADemoPlayerController::HandleServerEntry(){
    
    if(!HasAuthority()){
        return;
    }
    
    //FString PID = "1234";

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	
	Request->OnProcessRequestComplete().BindUObject(this, &ADemoPlayerController::OnProcessRequestComplete);
	
	// GET Request 
	//Request->SetURL("http://localhost:8080/api/PlayerData/" + PID);
	//Request->SetVerb("GET"); //Get Request

	//POST Request 
	Request->SetURL("http://localhost:8080/api/PlayerData/login");
	Request->SetVerb("POST"); // Post Request
	
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    // Post Request Code 
	FString JsonString;
	FPlayerData PlayerData;
	PlayerData.email = "leronbergelson@gmail.com";
    PlayerData.userpassword = "mypass";
	
	FJsonObjectConverter::UStructToJsonObjectString(PlayerData, JsonString);
	Request->SetContentAsString(JsonString);
    UE_LOG(LogTemp, Warning, TEXT("Json String %s"), *JsonString);

    
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
    // spawn new pawn at default location
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

void ADemoPlayerController::SaveData()
{
    UE_LOG(LogTemp, Warning, TEXT("Saving"));
    AREST_API_DemoCharacter* ControlledCharacter = GetPawn<AREST_API_DemoCharacter>();
    
    if(ControlledCharacter)
    {
        FVector Location = ControlledCharacter->GetActorLocation();
        FPlayerData PlayerData;
        PlayerData.email = "leronbergelson@gmail.com";
        PlayerData.userpassword = "mypass";
        PlayerData.isvalid = true;
        PlayerData.pid = 1234;    
        PlayerData.Health = ControlledCharacter->GetHealth();
        PlayerData.Xcoord = Location.X;
        PlayerData.Ycoord = Location.Y;
        PlayerData.Zcoord = Location.Z;
        
        TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
        
        //Request->OnProcessRequestComplete().BindUObject(this, &ADemoPlayerController::OnProcessRequestComplete);
        Request->SetURL("http://localhost:8080/api/PlayerData/updatePlayerData");
        Request->SetVerb("PUT"); // PUT Request
        Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

        FString JsonString;
        FJsonObjectConverter::UStructToJsonObjectString(PlayerData, JsonString);
        Request->SetContentAsString(JsonString);
        UE_LOG(LogTemp, Warning, TEXT("Json String %s"), *JsonString);
        
        // Post Request through API passing in PID
        Request->ProcessRequest();

    }

}