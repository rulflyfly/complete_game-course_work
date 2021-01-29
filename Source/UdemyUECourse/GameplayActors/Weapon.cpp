// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Main.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"


AWeapon::AWeapon()
{
    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    SkeletalMesh->SetupAttachment(GetRootComponent());
    
    CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
    CombatCollision->SetupAttachment(GetRootComponent());
    
    bWeaponParticles = false;
    
    WeaponState = EWeaponState::EWS_Pickup;
    
    Damage = 25.f;
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();
    
    /** That was we make sure it's set to NOT use physics  and only generate overlap events */
    CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
    
    CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapBegin);
    CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapEnd);
}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
    
    if (WeaponState == EWeaponState::EWS_Pickup && OtherActor)
    {
        /** we basically can check what we overlap with this way */
        AMain* Main = Cast<AMain>(OtherActor);
        
        if (Main && Main->EquippedWeapon != this)
        {
            Main->SetActiveOverlappingItem(this);
        }
    }
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
    
    if (OtherActor)
    {
        /** we basically can check what we overlap with this way */
        AMain* Main = Cast<AMain>(OtherActor);
        
        if (Main && Main->EquippedWeapon != this)
        {
            Main->SetActiveOverlappingItem(nullptr);
        }
    }
}


void AWeapon::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    if (OtherActor)
    {
        AEnemy* Enemy = Cast<AEnemy>(OtherActor);
        
        if (Enemy)
        {
            const USkeletalMeshSocket* WeaponSocket = SkeletalMesh->GetSocketByName("WeaponSocket");
            
            if (WeaponSocket)
            {
                FVector SocketLocation = WeaponSocket->GetSocketLocation(SkeletalMesh);
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, SocketLocation, FRotator(0.f), false);
            }
            
            if (Enemy->HitSound)
            {
                UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
            }
            
            if (DamageTypeClass)
            {
                UGameplayStatics::ApplyDamage(Enemy, Damage, WeaponInstigator, this, DamageTypeClass);
            }
        }
    }
}


void AWeapon::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor)
    {
        AEnemy* Enemy = Cast<AEnemy>(OtherActor);
        
        if (Enemy)
        {
            
        }
    }
}

void AWeapon::Equip(AMain* Char)
{
    if (Char)
    {
        SetInstigator(Char->GetController());
        
        AMain* Main = Char;
        /** This will prevent camera zooming in on a  player if the weapon gets between camera and a player */
        SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
        
        /** We don't want any collision happening to the pawn itself */
        SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
        
        /** To prevent any physics simulation, we just want to attach the weapon to socket without any additional behavior */
        SkeletalMesh->SetSimulatePhysics(false);
        
        /** Getting the socket we need to attach the weapon to */
        const USkeletalMeshSocket* RightHandSocket = Char->GetMesh()->GetSocketByName("RightHandSocket");
        
        if (RightHandSocket)
        {
            /** That's how actors get attached to sockets */
            RightHandSocket->AttachActor(this, Char->GetMesh());
            bRotate = false;
            Char->SetEquippedWeapon(this);
        }
        if (OnEquipSound) { UGameplayStatics::PlaySound2D(this, OnEquipSound); }
        
        if (!bWeaponParticles)
        {
            IdleParticlesComponent->Deactivate();
        }
    }
}


void AWeapon::ActivateCollision()
{
    CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::DeactivateCollision()
{
    CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
