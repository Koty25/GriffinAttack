// Copyright Epic Games, Inc. All Rights Reserved.

#include "GriffinAttackGameMode.h"

#include "ArcherTower.h"
#include "EngineUtils.h"
#include "GriffinAttackCharacter.h"
#include "PlatformsSpawner.h"
#include "Vagalume.h"
#include "UObject/ConstructorHelpers.h"

void AGriffinAttackGameMode::BeginPlay()
{
	Super::BeginPlay();

	SpawnNextPlatforms();
}

AGriffinAttackGameMode::AGriffinAttackGameMode()
{
}

// When a pawn is killed
void AGriffinAttackGameMode::PawnKilled(APawn* PawnKilled)
{
	APlayerController* PlayerController = Cast<APlayerController>(PawnKilled->GetController());
	if (PlayerController != nullptr)
	{
		EndGame(false);
	}

	// Archer Tower Death. Also adds score
	if (PawnKilled->IsA(AArcherTower::StaticClass()))
	{
		Score += ArcherTowerPoints;
		AArcherTower* ArcherTower = Cast<AArcherTower>(PawnKilled);
		ArcherTower->HandleDestruction();
	}
}

// When an actor is killed
void AGriffinAttackGameMode::ActorHit(AActor* ActorHit)
{
	// When the Vagalume is killed and adds score
	if (ActorHit->IsA(AVagalume::StaticClass()))
	{
		Score += VagalumePoints;
		AVagalume* Vagalume = Cast<AVagalume>(ActorHit);
		Vagalume->HealGriffin();
		Vagalume->HandleDestruction();
	}
}

// End game state checker
void AGriffinAttackGameMode::EndGame(bool bIsPlayerWinner)
{
	for (AController* Controller : TActorRange<AController>(GetWorld()))
	{
		bool bIsWinner = Controller->IsPlayerController() == bIsPlayerWinner;
		Controller->GameHasEnded(Controller->GetPawn(), bIsWinner);
	}
}

// Platform spawning function. It spawns in a random location.
void AGriffinAttackGameMode::SpawnNextPlatforms()
{	
	APlatformsSpawner* PlatformsSpawner = GetWorld()->SpawnActor<APlatformsSpawner>(PlatformClass, PlatformSpawnPointLocation, PlatformSpawnPointRotation);

	// Gives the location for the next platform
	PlatformSpawnPointLocation = PlatformsSpawner->GetNextPlatformLocation();
}

// Spawns the Archer Tower in a Random location of a platform.
void AGriffinAttackGameMode::SpawnArcherTower(float SpaceBetweenPieces, FVector NextPieceLocation)
{
	// Archer tower spawn location. It spawn in a random location of the platform.
	FVector ArcherTowerLocation = NextPieceLocation + FVector(FMath::RandRange(-SpaceBetweenPieces/4, SpaceBetweenPieces/4), 0, 33);				
	AArcherTower* ArcherTowerPiece = GetWorld()->SpawnActor<AArcherTower>(ArcherTowerClass, ArcherTowerLocation, FRotator(0));
	
	if (ArcherTowerPiece)
	{
		ArcherTowerPiece->SpawnDefaultController();	
	}
}
