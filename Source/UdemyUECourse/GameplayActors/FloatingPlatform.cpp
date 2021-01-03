// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// Sets default values
AFloatingPlatform::AFloatingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;
    
    StartPoint = FVector(0.f);
    EndPoint = FVector(0.f);
    
    InterpSpeed = 1.f;
    InterpTime = 1.f;
    
    bInterping = true;
    
    
}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
    StartPoint = GetActorLocation();
    
    /** EndPoint Location is presented in relative to the platform coordinates, we fix that by adding world coordinated of the StartPoint to it*/
    EndPoint += StartPoint;
    Distance = (EndPoint - StartPoint).Size();
    //GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterping, InterpTime);
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
    if (bInterping)
    {
        FVector CurrentLocation = GetActorLocation();
        /** A function for interpolating vectors to different points in 3D space */
        FVector Interp = FMath::VInterpTo(CurrentLocation, EndPoint, DeltaTime, InterpSpeed);
        SetActorLocation(Interp);

        float DistanceTraveled = (GetActorLocation() - StartPoint).Size();
        if (Distance - DistanceTraveled <= 1.f)
        {
            ToggleInterping();
            GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterping, InterpTime);
            SwapVectors(StartPoint, EndPoint);
        }
    }
}

void AFloatingPlatform::ToggleInterping()
{
    bInterping = !bInterping;
}

void AFloatingPlatform::SwapVectors(FVector& VecOne, FVector& VecTwo)
{
    FVector Temp = VecOne;
    VecOne = VecTwo;
    VecTwo = Temp;
}
