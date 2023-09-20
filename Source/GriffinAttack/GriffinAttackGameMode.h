// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GriffinAttackGameMode.generated.h"

UCLASS(minimalapi)
class AGriffinAttackGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	AGriffinAttackGameMode();

	// Pawn hit
	UFUNCTION()
	void PawnKilled(APawn* PawnKilled);

	// Actor hit
	UFUNCTION()
	void ActorHit(AActor* ActorHit);

	// EndGame Checker
	UFUNCTION()
	void EndGame(bool bIsPlayerWinner);

	// Spawn Platforms
	UFUNCTION(BlueprintCallable)
	void SpawnNextPlatforms();

	// Spawn Archer Tower
	UFUNCTION()
	void SpawnArcherTower(float SpaceBetweenPieces, FVector NextPieceLocation);


	// Platform spawning
	UPROPERTY(EditAnywhere, Category = "Platform")
	TSubclassOf<class APlatformsSpawner> PlatformClass;
	
	UPROPERTY(EditAnywhere, Category = "Platform")
	TSubclassOf<class AArcherTower> ArcherTowerClass;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Platform", meta = (AllowPrivateAccess = "true"))
	FVector PlatformSpawnPointLocation = FVector(0);
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Platform", meta = (AllowPrivateAccess = "true"))
	FRotator PlatformSpawnPointRotation = FRotator(0);

	// Scoring Parameters
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Score", meta = (AllowPrivateAccess = "true"))
	float Score = 0.f;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Score", meta = (AllowPrivateAccess = "true"))
	float ArcherTowerPoints = 50.f;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Score", meta = (AllowPrivateAccess = "true"))
	float VagalumePoints = 10.f;
};



