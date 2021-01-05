// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

/** A macro to make the enum show in blueprints */
UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
    /** If you make an enum blueprint tyoe you need to use umeta marker to give it a display name */
    EMS_Normal UMETA(DisplayName = "Normal"), // we use comas rather than semicolon inside enums
    EMS_Sprinting UMETA(DisplayName = "Sprinting"),
    
    /** in enums there often is the last max constant
     this isn't ment to be used */
    EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
    ESS_Normal UMETA(DisplayName = "Normal"),
    ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
    ESS_Exhausted UMETA(DisplayName = "Exhausted"),
    ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),
    
    ESS_MAX UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class UDEMYUECOURSE_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();
    
    TArray<FVector> PickupLocations;
    
    void ShowPickupLocations();
    
    /** We need a variable to hold the enum properties inside the class so that we can use those in blueprint scropting */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Enums)
    EMovementStatus MovementStatus;
    
    /** Sets movement status and running steed */
    void SetMovementStatus(EMovementStatus Status);
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Running)
    float RunningSpeed;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Running)
    float SprintingSpeed;
    
    bool bShiftKeyDown;
    
    /** Pressed down to enable sprinting */
    void ShiftKeyDown();
    
    /** Released to stop sprinting */
    void ShiftKeyUp();
    
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Enums)
    EStaminaStatus StaminaStatus;
    
    FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
    float StaminaDrainRate;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
    float MinSprintStamina;
    
    /** Camera Boom positioning the camera behind the player */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;
    
    /** Follow Camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* FollowCamera;
    
    /** Base turn rates to scale turning functions for the camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    float BaseTurnRate;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    float BaseLookUpRate;
    
    /**
                    Player Stats
     */
    
    /** BlueprintReadWrite makes in possible to edit variables by scripting from blueprints
     otherwise you can only change them from side menu bar */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
    float MaxHealth;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
    float Health;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
    float MaxStamina;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
    float Stamina;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
    int32 Coins;
   
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    
    /** Called for forwards/backwards movement */
    void MoveForward(float Value);

    /** Called for side to side unput */
    void MoveRight(float Value);
    
    /** Called via input to turn at a given rate
        @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired turn rate;
     */
    void TurnAtRate(float Rate);
    
    /** Called via input to look up/down  at a given rate
        @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired look up/down rate;
     */
    void LookUpAtRate(float Rate);
    
    void DecrementHealth(float Amount);
    
    void IncrementCoins(int32 Amount);
    
    void Die();
    
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
