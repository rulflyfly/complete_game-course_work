// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "Enemy.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
    /** for some reasons we don't need super here */
    
    if (Pawn == nullptr)
    {
        Pawn = TryGetPawnOwner();
        Enemy = Cast<AEnemy>(Pawn);
    }
}


void UEnemyAnimInstance::UpdateAnimationProperties()
{
    if (Pawn == nullptr)
    {
        Pawn = TryGetPawnOwner();
    }
    
    if (Pawn)
    {
        FVector Speed = Pawn->GetVelocity();
        FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
        MovementSpeed = LateralSpeed.Size(); // returns vector magnitude
        
        if (!Enemy)
        {
            Enemy = Cast<AEnemy>(Pawn);
        }
    }
}
