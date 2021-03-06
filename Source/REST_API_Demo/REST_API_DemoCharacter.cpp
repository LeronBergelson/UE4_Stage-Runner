// Copyright Epic Games, Inc. All Rights Reserved.

#include "REST_API_DemoCharacter.h"
#include "API_Info_GameInstance.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
//////////////////////////////////////////////////////////////////////////
// AREST_API_DemoCharacter

AREST_API_DemoCharacter::AREST_API_DemoCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	
	//UAPI_Info_GameInstance* GameInstanceRef = Cast<UAPI_Info_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld())); // Gets the API_Info_GameInstance

	Health = 100; // GameInstanceRef->GetPlayerHealth();
	didReachEnd = false;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AREST_API_DemoCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AREST_API_DemoCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AREST_API_DemoCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AREST_API_DemoCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AREST_API_DemoCharacter::LookUpAtRate);
}

void AREST_API_DemoCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AREST_API_DemoCharacter, Health);
}

bool AREST_API_DemoCharacter::Server_SetHealth_Validate(float NewHealth)
{
    return true;
}

void AREST_API_DemoCharacter::Server_SetHealth_Implementation(float NewHealth)
{
    SetHealth(NewHealth);
}

void AREST_API_DemoCharacter::SetHealth(float NewHealth){
	UAPI_Info_GameInstance* GameInstanceRef = Cast<UAPI_Info_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld())); // Gets the API_Info_GameInstance

    if(HasAuthority()){ // Check if sever
        Health = NewHealth; // set health
		// Saves player health to the Game Instance so that if the player gets respawned saved health values can be retrieved.
		GameInstanceRef->SetPlayerHealth(Health);
    }
    else{ // if we are the client
        Server_SetHealth(NewHealth); // call Server_SetHealth()
    }
}

/*
void AREST_API_DemoCharacter::SetBlueStageAttempts(int blueAttempts)
{
	UAPI_Info_GameInstance* GameInstanceRef = Cast<UAPI_Info_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld())); // Gets the API_Info_GameInstance

	if (HasAuthority()) { // Check if sever
		blueStageAttempts = blueAttempts; // set health
	    // Saves blue stage attempts to the Game Instance so that if the player gets respawned the attempt value can be retrieved.
		GameInstanceRef->SetBlueStageAttempts(blueStageAttempts);
	}
}

void AREST_API_DemoCharacter::SetYellowStageAttempts(int yellowAttempts)
{
	UAPI_Info_GameInstance* GameInstanceRef = Cast<UAPI_Info_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld())); // Gets the API_Info_GameInstance

	if (HasAuthority()) { // Check if sever
		yellowStageAttempts = yellowAttempts; // set health
		// Saves yellow stage attempts to the Game Instance so that if the player gets respawned the attempt value can be retrieved.
		GameInstanceRef->SetYellowStageAttempts(yellowAttempts);
	}
}

void AREST_API_DemoCharacter::SetRedStageAttempts(int redAttempts)
{
	UAPI_Info_GameInstance* GameInstanceRef = Cast<UAPI_Info_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld())); // Gets the API_Info_GameInstance

	if (HasAuthority()) { // Check if sever
		redStageAttempts = redAttempts; // set health
		// Saves red stage attempts to the Game Instance so that if the player gets respawned the attempt value can be retrieved.
		GameInstanceRef->SetRedStageAttempts(redStageAttempts);
	}
}

*/

void AREST_API_DemoCharacter::SetDidReachEnd(bool didFinishCourse)
{
	didReachEnd = didFinishCourse;
}

void AREST_API_DemoCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AREST_API_DemoCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}



void AREST_API_DemoCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AREST_API_DemoCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

