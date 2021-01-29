// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMainPlayerController::BeginPlay()
{
    Super::BeginPlay();
    
    if (HUDOverlayAsset)
    {
        HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
    }
    
    HUDOverlay->AddToViewport();
    HUDOverlay->SetVisibility(ESlateVisibility::Visible);
    
    if (WEnemyHealthBar)
    {
        EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);
        
        if (EnemyHealthBar)
        {
            EnemyHealthBar->AddToViewport();
            EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
            
            FVector2D Alignment(0.f, 0.f);
            EnemyHealthBar->SetAlignmentInViewport(Alignment);
        }
    }
    
    if (WPauseMenu)
    {
        PauseMenu = CreateWidget<UUserWidget>(this, WPauseMenu);
        
        if (PauseMenu)
        {
            PauseMenu->AddToViewport();
            PauseMenu->SetVisibility(ESlateVisibility::Hidden);
        }
    }
    
}

void AMainPlayerController::DisplayEnemyHealthBar()
{
    if (EnemyHealthBar)
    {
        bEnemyHealthBarVisible = true;
        EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
    }
}

void AMainPlayerController::RemoveEnemyHealthBar()
{
    if (EnemyHealthBar)
    {
        bEnemyHealthBarVisible = false;
        EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
    }
}


void AMainPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (EnemyHealthBar)
    {
        FVector2D PositionInViewport;
        
        /** this function will take a 3d location and turn it into a 2d location on the screen */
        ProjectWorldLocationToScreen(EnemyLocation, PositionInViewport);
        
        FVector2D SizeInViewport(200.f, 25.f);
        
        EnemyHealthBar->SetPositionInViewport(PositionInViewport);
        EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
    }
}


void AMainPlayerController::DisplayPauseMenu_Implementation()
{
    if (PauseMenu)
    {
        bPauseMenuVisible = true;
        PauseMenu->SetVisibility(ESlateVisibility::Visible);
        
        FInputModeGameAndUI InputModeGameAndUI;
        
        /** We we just set it like that we won't be able to see the mouse cursor */
        SetInputMode(InputModeGameAndUI);
        
        /** we also need this to see the cursor */
        bShowMouseCursor = true;
    }
}


void AMainPlayerController::RemovePauseMenu_Implementation()
{
    if (PauseMenu)
    {
        bPauseMenuVisible = false;
        
        GameModeOnly();
        
        bShowMouseCursor = false;
    }
}


void AMainPlayerController::TogglePauseMenu()
{
    if (!bPauseMenuVisible)
    {
        DisplayPauseMenu();
    }
    else
    {
        RemovePauseMenu();
    }
}


void AMainPlayerController::GameModeOnly()
{
    FInputModeGameOnly InputModeGameOnly;
    
    SetInputMode(InputModeGameOnly);
}
