// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Main.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Particles/ParticleSystem.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SkeletalMeshComponent.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "MainPlayerController.h"


// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    
    AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
    AgroSphere->SetupAttachment(GetRootComponent());
    AgroSphere->InitSphereRadius(600.f);
    AgroSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
    
    CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
    CombatSphere->SetupAttachment(GetRootComponent());
    CombatSphere->InitSphereRadius(75.f);
    
    CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
    
    /** this function take a USceneComponent and Mesh IS  a USceneComponent, so we can get it by GetMesh()
     the last argument here is a socket name^ so we're attaching the box component to the socket like this */
    //CombatCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("EnemySocket"));
    
    /** but we better do this to avoid errors on new enemy instanses where the mashes havn't got the specified socket names */
    CombatCollision->SetupAttachment(GetMesh(), FName("EnemySocket"));
    
    bOverlappingCombatSphere = false;
    
    Health = 100.f;
    MaxHealth = 100.f;
    Damage = 10.f;
    
    bAttacking = false;
    
    auto MovementComponent = GetCharacterMovement();
    if (MovementComponent)
    {
        MovementComponent->RotationRate = FRotator(0.f, 540.f, 0.f);
    }
    
    AttackMinTime = .5f;
    AttackMaxTime = 3.5f;
    
    EnemyMovementStatus = EEnemyMovementStatus::EMS_Idle;
    
    DeathDelay = 3.f;
    
    bHasValidTarget = false;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
    
    AIController = Cast<AAIController>(GetController());
    
    AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapBegin);
    AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapEnd);
    
    CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapBegin);
    CombatSphere->OnComponentEndOverlap.AddDynamic(this,&AEnemy::CombatSphereOnOverlapEnd);
    
    /** That was we make sure it's set to NOT use physics  and only generate overlap events */
    CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
    
    
    CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
    CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);

    /** Apparently we need to call it on both Mesh and Capsule so that the camra ignored the Enemy */
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    if (OtherActor && Alive())
    {
        AMain* Main = Cast<AMain>(OtherActor);
         if (Main)
         {
             MoveToTarget(Main);
             ReferenceToMain = Main;
         }
    }
}


void AEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && Alive())
    {
        AMain* Main = Cast<AMain>(OtherActor);
        
        if (Main)
        {
            SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
            
            Main->UpdateCombatTarget();
            
            if (AIController)
            {
                AIController->StopMovement();
            }
        }
    }
}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    if (OtherActor && Alive())
    {
        AMain* Main = Cast<AMain>(OtherActor);
        
        if (Main)
        {
            bHasValidTarget = true;
            Main->SetCombatTarget(this);
            
            Main->UpdateCombatTarget();
            
            Main->bHasCombatTarget = true;
            CombatTarget = Main;
            bOverlappingCombatSphere = true;
            Attack();
        }
    }
}


void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && Alive())
    {
        AMain* Main = Cast<AMain>(OtherActor);
        
        if (Main)
        {
            bHasValidTarget = false;
            bOverlappingCombatSphere = false;
            if (EnemyMovementStatus != EEnemyMovementStatus::EMS_Attacking)
            {
                MoveToTarget(Main);
                CombatTarget = nullptr;
                
                if (Main->CombatTarget == this)
                {
                    Main->SetCombatTarget(nullptr);
                    Main->bHasCombatTarget = false;
                    Main->UpdateCombatTarget();
                }
                
                if (Main->CombatTarget == this)
                {
                    Main->SetCombatTarget(nullptr);
                    Main->bHasCombatTarget = false;
                }
                
                if (Main->MainPlayerController)
                {
                    USkeletalMeshComponent* MainMesh = Cast<USkeletalMeshComponent>(OtherComp);
                    
                    if (MainMesh) Main->MainPlayerController->RemoveEnemyHealthBar();
                }
            }
            
            GetWorldTimerManager().ClearTimer(AttackTimer);
        }
    }
}

void AEnemy::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
  
    if (OtherActor && Alive())
    {
        AMain* Main = Cast<AMain>(OtherActor);
        
        if (Main)
        {
            const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName("TipSocket");
            
            if (TipSocket)
            {
                FVector SocketLocation = TipSocket->GetSocketLocation(GetMesh());
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->HitParticles, SocketLocation, FRotator(0.f), false);
            }
            
            if (Main->HitSound)
            {
                UGameplayStatics::PlaySound2D(this, Main->HitSound);
            }
            
            if (DamageTypeClass)
            {
                UGameplayStatics::ApplyDamage(Main, Damage, AIController, this, DamageTypeClass);
            }
        }
    }
}

void AEnemy::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && Alive())
    {
        AMain* Main = Cast<AMain>(OtherActor);
        
        if (Main)
        {
            
        }
    }
}


void AEnemy::MoveToTarget(AMain* Target)
{
    SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
    
    if (AIController && Alive())
    {
        FAIMoveRequest MoveRequest;
        MoveRequest.SetGoalActor(Target);
        MoveRequest.SetAcceptanceRadius(5.f);
        
        FNavPathSharedPtr NavPath;
        
        AIController->MoveTo(MoveRequest, &NavPath);
        
        /** Draws debug spheres to show the path that AI follows to get to target */
        /**
        TArray<FNavPathPoint> PathPoints = NavPath->GetPathPoints();
        
        for (auto PathPoint : PathPoints)
        {
            FVector Location = PathPoint.Location;
            UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 12, FLinearColor::Red, 10.f, 2.f);
        }
         */
    }
}

void AEnemy::ActivateCollision()
{
    CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateCollision()
{
    CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::Attack()
{
    if (Alive() && bHasValidTarget)
    {
        if (AIController)
        {
            AIController->StopMovement();
            SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
            if (!bAttacking) {
                bAttacking = true;
                UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
                    
                if (AnimInstance && CombatMontage)
                {
                    AnimInstance->Montage_Play(CombatMontage, 1.35f);
                    AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage);
                }
                if (SwingSound)
                {
                    UGameplayStatics::PlaySound2D(this, SwingSound);
                }
            }
        }
    }
    
}


void AEnemy::AttackEnd()
{
    bAttacking = false;
    
    if (bOverlappingCombatSphere)
    {
        GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, FMath::RandRange(AttackMinTime, AttackMaxTime));
    }
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (Health - DamageAmount <= 0.f)
    {
        Health -= DamageAmount;
        Die(DamageCauser);
    }
    else
    {
        Health -= DamageAmount;
    }
    return DamageAmount;
}

void AEnemy::Die(AActor* Causer)
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    
    if (AnimInstance && CombatMontage)
    {
        AnimInstance->Montage_Play(CombatMontage, 0.7f);
        AnimInstance->Montage_JumpToSection(FName("Death"), CombatMontage);
    }
    SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);
    
    CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    if (ReferenceToMain)
    {
        if (ReferenceToMain->MainPlayerController)
        {
            ReferenceToMain->MainPlayerController->RemoveEnemyHealthBar();
        }
    }
    
    AMain* Main = Cast<AMain>(Causer);
    
    if (Main)
    {
        Main->UpdateCombatTarget();
    }
}

void AEnemy::DeathEnd()
{
    GetMesh()->bPauseAnims = true;
    GetMesh()->bNoSkeletonUpdate = true;
    
    GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDelay);
}

bool AEnemy::Alive()
{
    return GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_Dead;
}


void AEnemy::Disappear()
{
    Destroy();
}
