// Fill out your copyright notice in the Description page of Project Settings.


#include "Floater.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AFloater::AFloater()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CustomStaticMesh"));
    
    InitialLocation = FVector(0.f);
    PlaceLocation = FVector(0.f);
    WorldOrigin = FVector(0.f, 0.f, 0.f);
    InitialDirection = FVector(0.f, 0.f, 0.f);
    InitialForce = FVector(200000.f, 0.f, 0.f);
    InitialTorque = FVector(200000.f, 0.f, 0.f);
    RunningTime = 0.f;
    
    A = 0.f;
    B = 0.f;
    C = 0.f;
    D = 0.f;
    
    bInitializeFloaterLocations = false;
    bShouldFloat = false;
}

// Called when the game starts or when spawned
void AFloater::BeginPlay()
{
	Super::BeginPlay();
    
    float InitialX = FMath::FRand();
    float InitialY = FMath::FRand();
    float InitialZ = FMath::FRand();
    
    InitialLocation.X = InitialX;
    InitialLocation.Y = InitialY;
    InitialLocation.Z = InitialZ;
    
    InitialLocation *= 300.f;
    
    PlaceLocation = GetActorLocation();
    
    if (bInitializeFloaterLocations)
    {
        SetActorLocation(InitialLocation);
    }
    
    BaseZLocation = PlaceLocation.Z;
    
    //StaticMesh->AddForce(InitialForce);
    //StaticMesh->AddTorqueInRadians(InitialTorque);
	
}

// Called every frame
void AFloater::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
    if (bShouldFloat)
    {
        FVector NewLocation = GetActorLocation();
        NewLocation.Z = BaseZLocation + A * FMath::Sin(B * RunningTime - C) + D;
        
        SetActorLocation(NewLocation);
        RunningTime += DeltaTime;
    }

}

