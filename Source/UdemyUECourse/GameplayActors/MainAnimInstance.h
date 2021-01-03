// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class UDEMYUECOURSE_API UMainAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
    
public:
    
    /** For UAnimInstance it's similar to bagin play of AActor */
    virtual void NativeInitializeAnimation() override;
    
    UFUNCTION(BlueprintCallable, Category = AnimationProperties)
    void UpdateAnimationProperties();
	
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
    float MovementSpeed;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
    bool bIsInAir;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
    class APawn* Pawn;
};
