// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AMain::AMain()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Create CameraBoom (pulls towards the player if there's a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetRootComponent());
    CameraBoom->TargetArmLength = 600.f; // Camera follows at this distance
    CameraBoom->bUsePawnControlRotation = true; // Rotate arm based on controller
    
    // Set size for collition capsule
    GetCapsuleComponent()->SetCapsuleSize(34.f, 88.f);
    
    //Create FollowCamera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    // Attach camera to the end of the boom and let the boom adjust
    // to match the controller orientation 
    FollowCamera->bUsePawnControlRotation = false;
    
    // Set our turn rates for input
    BaseTurnRate = 65.f;
    BaseLookUpRate = 65.f;
    
    
    /** Dont't rotate when controller rotates, let this just effect the camera*/
    
    // the character rotation will no longer change with mouse move along X axis
    bUseControllerRotationYaw = false;
    
    // this are usually false by default
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    
    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // ... at this rotation rate
    GetCharacterMovement()->JumpZVelocity = 650.f;
    GetCharacterMovement()->AirControl = 0.1f;
    
    

    MaxHealth = 100.f;
    Health = 65.f;
    MaxStamina = 150.f;
    Stamina = 120.f;
    Coins = 0.f;
    
    RunningSpeed = 650.f;
    SprintingSpeed = 950.f;
    
    bShiftKeyDown = false;
    
    MovementStatus = EMovementStatus::EMS_Normal;
    StaminaStatus = EStaminaStatus::ESS_Normal;
    
    StaminaDrainRate = 25.f;
    MinSprintStamina = 50.f;
}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();
	
    UKismetSystemLibrary::DrawDebugSphere(this, GetActorLocation() + FVector(0.f, 0.f, 75.f), 25.f, 12, FLinearColor::Red, 10.f, 2.f);
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    ShowPickupLocations();

    float DeltaStamina = StaminaDrainRate * DeltaTime;
    
    switch (StaminaStatus)
    {
        case EStaminaStatus::ESS_Normal:
            if (bShiftKeyDown)
            {
                if (Stamina - DeltaStamina <= MinSprintStamina)
                {
                    SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
                    Stamina -= DeltaStamina;
                }
                else
                {
                    Stamina -= DeltaStamina;
                }
                SetMovementStatus(EMovementStatus::EMS_Sprinting);
            }
            else // shift key up
            {
                if (Stamina + DeltaStamina >= MaxStamina)
                {
                    Stamina = MaxStamina;
                }
                else
                {
                    Stamina += DeltaStamina;
                }
                SetMovementStatus(EMovementStatus::EMS_Normal);
            }
            break;
        case EStaminaStatus::ESS_BelowMinimum:
            if (bShiftKeyDown)
            {
                if (Stamina - DeltaStamina <= 0)
                {
                    SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
                    SetMovementStatus(EMovementStatus::EMS_Normal);
                    Stamina = 0;
                }
                else
                {
                    Stamina -=DeltaStamina;
                    SetMovementStatus(EMovementStatus::EMS_Sprinting);
                }
            }
            else //shift key up
            {
                if (Stamina + DeltaStamina >= MinSprintStamina)
                {
                    SetStaminaStatus(EStaminaStatus::ESS_Normal);
                }
                Stamina += DeltaStamina;
                SetMovementStatus(EMovementStatus::EMS_Normal);
            }
            break;
        case EStaminaStatus::ESS_Exhausted:
            if (bShiftKeyDown)
            {
                Stamina = 0;
            }
            else // shift key up
            {
                SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
                Stamina += DeltaStamina;
            }
            SetMovementStatus(EMovementStatus::EMS_Normal);
            break;
        case EStaminaStatus::ESS_ExhaustedRecovering:
            if (Stamina + DeltaStamina >= MinSprintStamina)
            {
                SetStaminaStatus(EStaminaStatus::ESS_Normal);
            }
            Stamina += DeltaStamina;
            SetMovementStatus(EMovementStatus::EMS_Normal);
            break;
        default:
            ;
    }
}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    check(PlayerInputComponent);
    
    /** Here we're using a Jump funtion inherited from character. We can do that if we don't write our own funtion for that */
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    
    /** Here we're using a StopJu,pint Function inherited from character */
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
    
    PlayerInputComponent->BindAction("Sprinting", IE_Pressed, this, &AMain::ShiftKeyDown);
    PlayerInputComponent->BindAction("Sprinting", IE_Released, this, &AMain::ShiftKeyUp);
    
    PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);
    
    /** Here we're using AddControllerYawInput function inherited from pawn that will turn our character to sides */
    PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    
    /** Here we're using AddControllerPitchInput funtion inherited from pawn that will let our character look up and down */
    PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
    
    PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
    PlayerInputComponent->BindAxis("LookUpRate", this, &AMain::LookUpAtRate);
}

void AMain::MoveForward(float Value)
{
    if ((Controller != nullptr) && (Value != 0.f))
    {
        // find out which way is farward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
        
        // getting a forward vector based of a particular rotator
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void AMain::MoveRight(float Value)
{
    if ((Controller != nullptr) && (Value != 0.f))
    {
        // find out which way is farward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
        
        // getting a forward vector based of a particular rotator
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
    }
}


void AMain::TurnAtRate(float Rate)
{
    AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMain::LookUpAtRate(float Rate)
{
    AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMain::DecrementHealth(float Amount)
{
    if (Health - Amount <= 0)
    {
        Health -= Amount;
        Die();
    }
    else
    {
        Health -= Amount;
    }
}

void AMain::IncrementCoins(int32 Amount)
{
    Coins += Amount;
}

void AMain::Die()
{
    
}


void AMain::SetMovementStatus(EMovementStatus Status)
{
    MovementStatus = Status;
    if (MovementStatus == EMovementStatus::EMS_Sprinting)
    {
        /** Here's how you  can change movement speed of a character */
        GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
    }
    else
    {
        GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
    }
}

void AMain::ShiftKeyDown()
{
    bShiftKeyDown = true;
    UE_LOG(LogTemp, Warning, TEXT("Shift Pressed"));
}

void AMain::ShiftKeyUp()
{
    bShiftKeyDown = false;
    UE_LOG(LogTemp, Warning, TEXT("Shift Released"));
}

void AMain::ShowPickupLocations()
{
    for (FVector Location : PickupLocations)
    {
        UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 12, FLinearColor::Red, 10.f, 2.f);
    }
}
