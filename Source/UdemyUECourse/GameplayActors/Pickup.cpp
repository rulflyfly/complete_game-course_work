// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "Main.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

APickup::APickup()
{
    
}

void APickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
    
    
    /** Other Actor is the actor that overlapped wit h this actor */
    if (OtherActor)
    {
        AMain* Main = Cast<AMain>(OtherActor);
        
        if (Main)
        {
            OnPickupBP(Main);
            Main->PickupLocations.Add(GetActorLocation());
            
            if (OverlapSound)
            {
                UGameplayStatics::PlaySound2D(this, OverlapSound);
            }
            if (OverlapParticles)
            {
                /** We created UParticleSystem and not UParticleSystemComponent because this function requires a system not a component
                 We don't need to initialize UParticleSystem with CreateDefaultSubobject */
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.f), true);
            }
            
            Destroy();
        }
    }
}

void APickup::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
    
    UE_LOG(LogTemp, Warning, TEXT("Pickup::OnOverlapEnd()"));
}
