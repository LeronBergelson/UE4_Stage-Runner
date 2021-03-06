// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoPlayerController.h"
#include "REST_API_DEMO/REST_API_DemoCharacter.h"
#include "REST_API_DEMO/REST_API_DemoGameMode.h"
#include "API_Info_GameInstance.h"
#include "JsonObjectConverter.h"
#include <Runtime/Engine/Public/Net/UnrealNetwork.h>
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>

ADemoPlayerController::ADemoPlayerController(){
	Http = &FHttpModule::Get();
    canSaveData = true;
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


void ADemoPlayerController::HandleServerEntry() {

    if (!HasAuthority()) {
        return;
    }

    UAPI_Info_GameInstance* GameInstanceRef = Cast<UAPI_Info_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

    FString tempUserEmail = GameInstanceRef->getUserEmail();
    FString tempUserPassword = GameInstanceRef->getUserPassword();

    UWorld* MyWorld = GetWorld();
    FString CurrentMapName = MyWorld->GetMapName(); // Retrieves map name
    CurrentMapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix); // Removes UEDPIE_0_ prefix on retrieved map name

    if (tempUserEmail != "" && tempUserPassword != "") {

        if (CurrentMapName == "LoginMap") // Makes sure pawn does not get spawned in the LoginMap level
        {
            UserSignIn(tempUserEmail, tempUserPassword);
        }
        else {
            CreatePlayer();
        }
    }

}

void ADemoPlayerController::SaveCourseData(int blueStageCompletionTime, int yellowStageCompletionTime, int redStageCompletionTime)
{
    if (!HasAuthority()) {
        return;
    }

    UAPI_Info_GameInstance* GameInstanceRef = Cast<UAPI_Info_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

    FString tempUserEmail = GameInstanceRef->getUserEmail();
    FString JWTValue = GameInstanceRef->GetJWT();

    FString AuthorizationValue = "Bearer " + JWTValue;

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

    //POST Request 
    Request->SetURL("http://localhost:8080/api/PostPlayerCourseData/");
    Request->SetVerb("POST"); // Post Request
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Authorization"), AuthorizationValue);

    // Post Request Code 
    FString JsonString;
    FCourseData FCourseData;
    FCourseData.email = tempUserEmail;
    FCourseData.bluestagecompletiontime = blueStageCompletionTime;
    FCourseData.yellowstagecompletiontime = yellowStageCompletionTime;
    FCourseData.redstagecompletiontime = redStageCompletionTime;

    FJsonObjectConverter::UStructToJsonObjectString(FCourseData, JsonString);
    Request->SetContentAsString(JsonString);
    UE_LOG(LogTemp, Warning, TEXT("Json String %s"), *JsonString);

    Request->ProcessRequest();
}

void ADemoPlayerController::OnProcessRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success)
{

    if (Success) {
        UE_LOG(LogTemp, Warning, TEXT("Json String %s"), *Response->GetContentAsString());
    }

    FVector Location;
    FPlayerData PlayerData;

    UWorld* MyWorld = GetWorld();
    FString CurrentMapName = MyWorld->GetMapName(); // Retrieves map name
    CurrentMapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix); // Removes UEDPIE_0_ prefix on retrieved map name

    UAPI_Info_GameInstance* GameInstanceRef = Cast<UAPI_Info_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld())); // Gets the API_Info_GameInstance

    if (Success)
    {
        //UE_LOG(LogTemp, Warning, TEXT("SUCCESS %s"), *Response->GetContentAsString());

        PlayerData = ConvertToPlayerData(Response->GetContentAsString()); // Converts Contents in Json string to PlayerData Struct

        // If is valid, proceed to get player location, health, stage attempts which are passed in from the database  
        if (PlayerData.isvalid) {

            //playerConnectEstablished = true; // If (PlayerData.isvalid) is true playerConnectEstablished is set to true

            if (CurrentMapName != "LoginMap") // Makes sure pawn does not get spawned in the LoginMap level
            {
                // Setup pawn location
                Location.X = PlayerData.xcoord;
                Location.Y = PlayerData.ycoord;
                Location.Z = PlayerData.zcoord;

                if (AREST_API_DemoGameMode* GM = GetWorld()->GetAuthGameMode<AREST_API_DemoGameMode>())
                {
                    FActorSpawnParameters SpawnParams;
                    SpawnParams.Owner = this;
                    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

                    // Spawn and possess pawn
                    if (AREST_API_DemoCharacter* NewPawn = GetWorld()->SpawnActor<AREST_API_DemoCharacter>(GM->DefaultPawnClass, Location, FRotator::ZeroRotator, SpawnParams))
                    {

                        NewPawn->SetHealth(PlayerData.health); // Sets character health
                        
                        // Saves retrieved current pawn/player stage attempts to the game instance
                        GameInstanceRef->SetBlueStageAttempts(PlayerData.bluestageattempts);         //NewPawn->SetBlueStageAttempts(PlayerData.bluestageattempts); 
                        GameInstanceRef->SetYellowStageAttempts(PlayerData.yellowstageattempts);     //NewPawn->SetYellowStageAttempts(PlayerData.yellowstageattempts); 
                        GameInstanceRef->SetRedStageAttempts(PlayerData.redstageattempts);           //NewPawn->SetRedStageAttempts(PlayerData.redstageattempts); 
                        
                        Possess(NewPawn);
                    }

                    SaveDataRepeater(true); 
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("FAILED"));
            //playerConnectEstablished = false; // If player is not found playerConnectEstablished is set to false
        }
    }
    
}

FPlayerData ADemoPlayerController::ConvertToPlayerData(const FString& ResponseString)
{
    FPlayerData PlayerData; // Creates empty PlayerData object

    if (!ResponseString.Contains("timestamp"))
    {
        FJsonObjectConverter::JsonObjectStringToUStruct(*ResponseString, &PlayerData, 0, 0); // Populates data retrieved from database into PlayerData
    }

    return PlayerData; // Either returns populated PlayerData object if result is successful or empty PlayerData object if result is NULL
}

void ADemoPlayerController::OnProcessSignInRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success) {

    FAuthenticationResponse JWT;


    if (Success)
    {
        UE_LOG(LogTemp, Warning, TEXT("Json String %s"), *Response->GetContentAsString());
        JWT = ConvertToJWT(Response->GetContentAsString()); // Converts Contents in Json string to SignInData Struct
        UAPI_Info_GameInstance* GameInstanceRef = Cast<UAPI_Info_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
        GameInstanceRef->SetJWT(JWT.jwt); // Saves JWT to the game instance
        playerConnectEstablished = true; // If (PlayerData.isvalid) is true playerConnectEstablished is set to true

    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("FAILED"));
        playerConnectEstablished = false; 
    }
}

FAuthenticationResponse ADemoPlayerController::ConvertToJWT(const FString& ResponseString) {

    FAuthenticationResponse JWT;


    if (!ResponseString.Contains("timestamp"))
    {
        FJsonObjectConverter::JsonObjectStringToUStruct(*ResponseString, &JWT, 0, 0); // Populates data retrieved from database into PlayerData
    }

    UE_LOG(LogTemp, Warning, TEXT("JWT String: %s"), *JWT.jwt);

    return JWT;

}

void ADemoPlayerController::UserSignIn(FString username, FString password) {

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

    Request->OnProcessRequestComplete().BindUObject(this, &ADemoPlayerController::OnProcessSignInRequestComplete);

    //POST Request 
    Request->SetURL("http://localhost:8080/authenticate");
    Request->SetVerb("POST"); // Post Request

    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    // Post Request Code 
    FString JsonString;
    FSignInData SignInData;
    SignInData.username = username;
    SignInData.password = password;

    FJsonObjectConverter::UStructToJsonObjectString(SignInData, JsonString);
    Request->SetContentAsString(JsonString);
    UE_LOG(LogTemp, Warning, TEXT("Json String %s"), *JsonString);

    Request->ProcessRequest();

}

void ADemoPlayerController::CreatePlayer() {


    UAPI_Info_GameInstance* GameInstanceRef = Cast<UAPI_Info_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    FString JWTValue = GameInstanceRef->GetJWT();
    FString email = GameInstanceRef->getUserEmail();

    FString AuthorizationValue = "Bearer " + JWTValue;
    
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

    Request->OnProcessRequestComplete().BindUObject(this, &ADemoPlayerController::OnProcessRequestComplete);

    // GET Request
    Request->SetURL("http://localhost:8080/api/getPlayerData/" + email);
    Request->SetVerb("GET"); //Get Request
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Authorization"), AuthorizationValue);
    Request->ProcessRequest();
}



void ADemoPlayerController::SaveData()
{
    if (canSaveData) {

        UE_LOG(LogTemp, Warning, TEXT("Saving"));
        AREST_API_DemoCharacter* ControlledCharacter = GetPawn<AREST_API_DemoCharacter>();

        // Cast to Game Instance
        UAPI_Info_GameInstance* GameInstanceRef = Cast<UAPI_Info_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld())); // Gets the API_Info_GameInstance
        //FString UserEmail = GameInstanceRef->getUserEmail(); // Retrieve user email 
        //FString UserPassword = GameInstanceRef->getUserPassword(); // Retrieve user password 

        if (ControlledCharacter)
        {
            FVector Location = ControlledCharacter->GetActorLocation();
            FPlayerData PlayerData;
            PlayerData.email = GameInstanceRef->getUserEmail(); // Retrieve user email 
            PlayerData.isvalid = true;
            PlayerData.health = ControlledCharacter->GetHealth();

            if (!ControlledCharacter->GetDidReachEnd()) {
                PlayerData.xcoord = Location.X;
                PlayerData.ycoord = Location.Y;
                PlayerData.zcoord = Location.Z;
            }
            else {
                PlayerData.xcoord = 208.425f;
                PlayerData.ycoord = 2567.03f;
                PlayerData.zcoord = 424.192f;
            }
            PlayerData.bluestageattempts = GameInstanceRef->GetBlueStageAttempts(); // ControlledCharacter->GetBlueStageAttempts();
            PlayerData.yellowstageattempts = GameInstanceRef->GetYellowStageAttempts(); // ControlledCharacter->GetYellowStageAttempts();
            PlayerData.redstageattempts = GameInstanceRef->GetRedStageAttempts(); // ControlledCharacter->GetRedStageAttempts();

            FString JWTValue = GameInstanceRef->GetJWT();
            FString AuthorizationValue = "Bearer " + JWTValue;

            TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

            //Request->OnProcessRequestComplete().BindUObject(this, &ADemoPlayerController::OnProcessRequestComplete);
            Request->SetURL("http://localhost:8080/api/updatePlayerData");
            Request->SetVerb("PUT"); // PUT Request
            Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
            Request->SetHeader(TEXT("Authorization"), AuthorizationValue);

            FString JsonString;
            FJsonObjectConverter::UStructToJsonObjectString(PlayerData, JsonString);
            Request->SetContentAsString(JsonString);
            UE_LOG(LogTemp, Warning, TEXT("Json String %s"), *JsonString);

            // Post Request through API passing in PID
            Request->ProcessRequest();
        }
    }
}

void ADemoPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ADemoPlayerController, userEmail);
}


bool ADemoPlayerController::Server_SetUserEmail_Validate(const FString& NewUserEmail)
{
    return true;
}

void ADemoPlayerController::Server_SetUserEmail_Implementation(const FString& NewUserEmail)
{
    SetUserEmail(NewUserEmail);
}

void ADemoPlayerController::SetUserEmail(FString NewUserEmail){

    if(HasAuthority()){ // Check if server
        userEmail = NewUserEmail; // set userEmail
        UAPI_Info_GameInstance* GameInstanceRef = Cast<UAPI_Info_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
        GameInstanceRef->SetUserEmail(NewUserEmail);
        //UE_LOG(LogTemp, Warning, TEXT("userEmail: %s"), *userEmail);
    }
    else{ // if we are the client
        Server_SetUserEmail(NewUserEmail); // call Server_SetUserEmail()
    }
}

bool ADemoPlayerController::Server_SetUserPassword_Validate(const FString& NewUserPassword)
{
    return true;
}

void ADemoPlayerController::Server_SetUserPassword_Implementation(const FString& NewUserPassword)
{
    SetUserPassword(NewUserPassword);
}

void ADemoPlayerController::SetUserPassword(FString NewUserPassword){

    if(HasAuthority()){ // Check if server
        userPassword = NewUserPassword; // set UserPassword
        //UE_LOG(LogTemp, Warning, TEXT("userPassword: %s"), *userPassword);
        UAPI_Info_GameInstance* GameInstanceRef = Cast<UAPI_Info_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
        GameInstanceRef->SetUserPassword(userPassword);
    }
    else{ // if we are the client
        Server_SetUserPassword(NewUserPassword); // call Server_SetUserPassword()
    }
}

bool ADemoPlayerController::SaveDataRepeater(bool activeState)
{
     canSaveData = activeState;

    // Calls SaveData every 5 seconds
    if (canSaveData) {
        //TimerDel.BindUFunction(this, FName("SaveData"), UserEmail, UserPassword);
        TimerDel.BindUFunction(this, FName("SaveData"));
        GetWorldTimerManager().SetTimer(TSaveHandle, TimerDel, 5.0f, true);
    }
    else {
        GetWorldTimerManager().PauseTimer(TSaveHandle);
        //GetWorldTimerManager().ClearTimer(TSaveHandle);
    }

    return canSaveData;
}


