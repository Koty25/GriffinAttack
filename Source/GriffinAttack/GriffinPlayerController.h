// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GriffinPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GRIFFINATTACK_API AGriffinPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	// Game End Screen spawner
	virtual void GameHasEnded(AActor* EndGameFocus, bool bIsWinner) override;

private:
	
	// Player's HUD
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> HUDClass;

	// End game Screen
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> EndScreenClass;

	// Restart Timer
	UPROPERTY(EditAnywhere)
	float RestartDelay = 10.f;

	FTimerHandle RestartTimer;

	UPROPERTY()
	UUserWidget* HUD;
	
};
