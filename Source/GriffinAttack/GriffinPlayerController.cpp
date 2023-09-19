// Fill out your copyright notice in the Description page of Project Settings.


#include "GriffinPlayerController.h"

#include "Blueprint/UserWidget.h"

void AGriffinPlayerController::BeginPlay()
{
	Super::BeginPlay();

	HUD =  CreateWidget(this, HUDClass);
	if (HUD != nullptr)
	{
		HUD->AddToViewport();
	}
}

void AGriffinPlayerController::GameHasEnded(AActor* EndGameFocus, bool bIsWinner)
{
	Super::GameHasEnded(EndGameFocus, bIsWinner);

	HUD->RemoveFromParent();
	//if (bIsWinner)
	//{
		UUserWidget* EndScreen =  CreateWidget(this, EndScreenClass);
		if (EndScreen != nullptr)
		{
			EndScreen->AddToViewport();
		}
	//}
	//else
	//{
		// UUserWidget* LoseScreen =  CreateWidget(this, LoseScreenClass);
		// if (LoseScreen != nullptr)
		// {
		// 	LoseScreen->AddToViewport();
		// }	
	//}
	
	
	GetWorldTimerManager().SetTimer(RestartTimer, this, &APlayerController::RestartLevel, RestartDelay);
}
