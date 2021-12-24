// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "REST_API_DemoCharacter.generated.h"

UCLASS(config=Game)
class AREST_API_DemoCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	AREST_API_DemoCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:
    
	//Needs to be replicated
    UPROPERTY(Replicated) 
    float Health;
	UPROPERTY(Replicated) 
	int blueStageAttempts;
	UPROPERTY(Replicated)
	int yellowStageAttempts;
	UPROPERTY(Replicated)
	int redStageAttempts;
	UPROPERTY(Replicated)
	bool didReachEnd;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_SetHealth(float NewHealth);
     
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
	// Setter and Getter for player Health
	UFUNCTION(BlueprintCallable)
	void SetHealth(float NewHealth);
	UFUNCTION(BlueprintCallable)
    float GetHealth() const {return Health;}

	// Setter and Getter for player blue stage attempts
	UFUNCTION(BlueprintCallable)
	void SetBlueStageAttempts(int blueAttempts);
	UFUNCTION(BlueprintCallable)
	int GetBlueStageAttempts() const {return blueStageAttempts;}

	// Setter and Getter for player yellow stage attempts
	UFUNCTION(BlueprintCallable)
	void SetYellowStageAttempts(int yellowAttempts);
	UFUNCTION(BlueprintCallable)
	int GetYellowStageAttempts() const {return yellowStageAttempts;}

	// Setter and Getter for player red stage attempts
	UFUNCTION(BlueprintCallable)
	void SetRedStageAttempts(int redAttempts);
	UFUNCTION(BlueprintCallable)
	int GetRedStageAttempts() const {return redStageAttempts;}

	// Setter and Getter for player red stage attempts
	UFUNCTION(BlueprintCallable)
	void SetDidReachEnd(bool didFinishCourse);
	UFUNCTION(BlueprintCallable)
	bool GetDidReachEnd() const {return didReachEnd;}

};

