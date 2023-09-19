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
	
	UFUNCTION()
	void PawnKilled(APawn* PawnKilled);

	UFUNCTION()
	void ActorHit(AActor* ActorHit);

	UFUNCTION()
	void EndGame(bool bIsPlayerWinner);

	UFUNCTION(BlueprintCallable)
	void SpawnNextPlatforms();

	UFUNCTION()
	void SpawnArcherTower(float SpaceBetweenPieces, FVector NextPieceLocation);

	UPROPERTY(EditAnywhere, Category = "Platform")
	TSubclassOf<class APlatformsSpawner> PlatformClass;
	
	UPROPERTY(EditAnywhere, Category = "Platform")
	TSubclassOf<class AArcherTower> ArcherTowerClass;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Platform", meta = (AllowPrivateAccess = "true"))
	FVector PlatformSpawnPointLocation = FVector(0);
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Platform", meta = (AllowPrivateAccess = "true"))
	FRotator PlatformSpawnPointRotation = FRotator(0);

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Score", meta = (AllowPrivateAccess = "true"))
	float Score = 0.f;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Score", meta = (AllowPrivateAccess = "true"))
	float ArcherTowerPoints = 50.f;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Score", meta = (AllowPrivateAccess = "true"))
	float VagalumePoints = 10.f;
};



