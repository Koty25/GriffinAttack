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
	// set default pawn class to our Blueprinted character
	// static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/GriffinAttack/Blueprints/BP_GriffinCharacter"));
	// if (PlayerPawnBPClass.Class != NULL)
	// {
	// 	DefaultPawnClass = PlayerPawnBPClass.Class;
	// }
}

void AGriffinAttackGameMode::PawnKilled(APawn* PawnKilled)
{
	APlayerController* PlayerController = Cast<APlayerController>(PawnKilled->GetController());
	if (PlayerController != nullptr)
	{
		EndGame(false);
	}

	if (PawnKilled->IsA(AArcherTower::StaticClass()))
	{
		Score += ArcherTowerPoints;
		AArcherTower* ArcherTower = Cast<AArcherTower>(PawnKilled);
		ArcherTower->HandleDestruction();
	}
}

void AGriffinAttackGameMode::ActorHit(AActor* ActorHit)
{
	if (ActorHit->IsA(AVagalume::StaticClass()))
	{
		Score += VagalumePoints;
		AVagalume* Vagalume = Cast<AVagalume>(ActorHit);
		Vagalume->HealGriffin();
		Vagalume->HandleDestruction();
	}
}

void AGriffinAttackGameMode::EndGame(bool bIsPlayerWinner)
{
	for (AController* Controller : TActorRange<AController>(GetWorld()))
	{
		bool bIsWinner = Controller->IsPlayerController() == bIsPlayerWinner;
		Controller->GameHasEnded(Controller->GetPawn(), bIsWinner);
	}
}

void AGriffinAttackGameMode::SpawnNextPlatforms()
{
	UE_LOG(LogTemp,Warning, TEXT("Vector: %s"), *PlatformSpawnPointLocation.ToString());
	
	APlatformsSpawner* PlatformsSpawner = GetWorld()->SpawnActor<APlatformsSpawner>(PlatformClass, PlatformSpawnPointLocation, PlatformSpawnPointRotation);

	PlatformSpawnPointLocation = PlatformsSpawner->GetNextPlatformLocation();

	UE_LOG(LogTemp,Warning, TEXT("Vector: %s"), *PlatformSpawnPointLocation.ToString());
	
}

void AGriffinAttackGameMode::SpawnArcherTower(float SpaceBetweenPieces, FVector NextPieceLocation)
{
	FVector ArcherTowerLocation = NextPieceLocation + FVector(FMath::RandRange(-SpaceBetweenPieces/4, SpaceBetweenPieces/4), 0, 33);				
	AArcherTower* ArcherTowerPiece = GetWorld()->SpawnActor<AArcherTower>(ArcherTowerClass, ArcherTowerLocation, FRotator(0));
	if (ArcherTowerPiece)
	{
		ArcherTowerPiece->SpawnDefaultController();	
	}
	//ArcherTowerPiece->AttachToActor(PlatformPiecesVector[i+1], FAttachmentTransformRules::KeepWorldTransform);
	UE_LOG(LogTemp,Warning, TEXT("Spawn ArcherTower!"));
}
