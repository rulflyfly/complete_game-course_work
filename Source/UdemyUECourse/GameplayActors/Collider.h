// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Collider.generated.h"

UCLASS()
class UDEMYUECOURSE_API ACollider : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACollider();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    
    UPROPERTY(VisibleAnywhere, Category = "Mesh")
    class UStaticMeshComponent* MeshComponent;
    
    UPROPERTY(VisibleAnywhere, Category = "Mesh")
    class USphereComponent* SphereComponent;
    
    UPROPERTY(VisibleAnywhere, Category = "Mesh")
    class UCameraComponent* Camera;
    
    UPROPERTY(VisibleAnywhere, Category = "Mesh")
    class USpringArmComponent* SpringArm;
    
    virtual UPawnMovementComponent* GetMovementComponent() const override; 
    
    UPROPERTY(VisibleAnywhere, Category = "Movement")
    class UColliderMovementComponent* OurMovementComponent;
    
    FORCEINLINE UStaticMeshComponent* GetMeshComponent() { return MeshComponent; }
    FORCEINLINE void SetMeshComponent(UStaticMeshComponent* Mesh) { MeshComponent = Mesh; }
    
    FORCEINLINE USphereComponent* GetSphereComponent() { return SphereComponent; }
    FORCEINLINE void SetMeshComponent(USphereComponent* Sphere) { SphereComponent = Sphere; }
    
    FORCEINLINE UCameraComponent* GetCamera() { return Camera; }
    FORCEINLINE void SetCamera(UCameraComponent* InCamera) { Camera = InCamera; }
    
    FORCEINLINE USpringArmComponent* GetSpringArm() { return SpringArm; }
    FORCEINLINE void SetSpringArm(USpringArmComponent* InSpringArm) { SpringArm = InSpringArm; }
    
    
private:
    void MoveForward(float Value);
    void MoveRight(float Value);
    
    void PitchCamera(float AxisValue);
    void YawCamera(float AxisValue);
    
    FVector2D CameraInput;
    
    float Speed;

};
