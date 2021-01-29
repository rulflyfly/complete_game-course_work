// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosive.h"
#include "Main.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"

AExplosive::AExplosive()
{
    Damage = 15.f;
}

void AExplosive::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
    
    
    if (OtherActor)
    {
        AMain* Main = Cast<AMain>(OtherActor);
        AEnemy* Enemy = Cast<AEnemy>(OtherActor);
        if (Main || Enemy)
        {
            if (DamageTypeClass)
            {
                /** We can pass in nullptr for ivant instigator because Explosive is just an actor */
                UGameplayStatics::ApplyDamage(OtherActor, Damage, nullptr, this, DamageTypeClass);
            }
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

void AExplosive::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
    
}
