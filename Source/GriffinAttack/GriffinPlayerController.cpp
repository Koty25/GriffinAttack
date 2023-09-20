// Fill out your copyright notice in the Description page of Project Settings.


#include "GriffinPlayerController.h"

#include "Blueprint/UserWidget.h"

// Adds Player's HUD with Health, Dash Charges and Score
void AGriffinPlayerController::BeginPlay()
{
	Super::BeginPlay();

	HUD =  CreateWidget(this, HUDClass);
	if (HUD != nullptr)
	{
		HUD->AddToViewport();
	}
}

// Deletes Players HUD and adds a Game over screen. Restart the level.
void AGriffinPlayerController::GameHasEnded(AActor* EndGameFocus, bool bIsWinner)
{
	Super::GameHasEnded(EndGameFocus, bIsWinner);

	HUD->RemoveFromParent();
		UUserWidget* EndScreen =  CreateWidget(this, EndScreenClass);
		if (EndScreen != nullptr)
		{
			EndScreen->AddToViewport();
		}
	GetWorldTimerManager().SetTimer(RestartTimer, this, &APlayerController::RestartLevel, RestartDelay);
}
