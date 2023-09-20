// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformsSpawner.h"

#include "ArcherTower.h"
#include "GriffinAttackGameMode.h"
#include "GriffinCharacter.h"
#include "Vagalume.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APlatformsSpawner::APlatformsSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the components and root
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	
	NextPlatformLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("Next Platform Location"));
	NextPlatformLocation->SetupAttachment(RootComponent);

	InvisibleWallTop = CreateDefaultSubobject<UBoxComponent>(TEXT("Invisible Wall Top"));
	InvisibleWallTop->SetupAttachment(RootComponent);
	
	InvisibleWallBottom = CreateDefaultSubobject<UBoxComponent>(TEXT("Invisible Wall Bottom"));
	InvisibleWallBottom->SetupAttachment(RootComponent);
	
	NewPlatformsTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("New Platforms Trigger"));
	NewPlatformsTrigger->SetupAttachment(RootComponent);
	NewPlatformsTrigger->OnComponentBeginOverlap.AddDynamic(this, &APlatformsSpawner::OnBeginOverlap);

	PlatformPositions = CreateDefaultSubobject<UBoxComponent>(TEXT("Platform Positions"));
	PlatformPositions->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APlatformsSpawner::BeginPlay()
{
	Super::BeginPlay();

	// Box component location and extent
	FVector SpawnLocation = PlatformPositions->GetComponentLocation();
	FVector BoxExtent = PlatformPositions->GetScaledBoxExtent();

	// Amount of platforms to spawn
	int32 PlatformAmount = FMath::RandRange(3, PlatformAmountMax);

	// Spawn X platforms inside the box component
	SpawnPlatforms(SpawnLocation, BoxExtent, PlatformAmount);
}

// Called every frame
void APlatformsSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// When actor overlaps, spawn more platforms
void APlatformsSpawner::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == Cast<AGriffinCharacter>(UGameplayStatics::GetPlayerPawn(this,0)))
	{
		GriffinAttackGameMode = Cast<AGriffinAttackGameMode>(UGameplayStatics::GetGameMode(this));
		GriffinAttackGameMode->SpawnNextPlatforms();

		this->SetLifeSpan(TimeToDeletePreviousPlatform);
	}
}

// Platform Spawner Destruction
void APlatformsSpawner::HandleDestruction()
{
	Destroy();
}

// Returns next platform location
FVector APlatformsSpawner::GetNextPlatformLocation()
{
	return NextPlatformLocation->GetComponentLocation();
}

/*	Spawn Platform function. It Spawns the ground and Archer Towers or Vagalumes inside the box component.
	When a Platform spawns, it is composed of two edge pieces and a random amount of middle pieces.
	Archer towers can only spawn on top of those middle pieces. It spawns at a set rate (25% by default)
	This value could be elevated to Blueprint in a subsequent iteration
*/
void APlatformsSpawner::SpawnPlatforms(FVector BoxPosition, FVector BoxExtent, int32 Amount)
{
	if (EdgePieceClass == nullptr && MiddlePieceClass == nullptr)
	{
		return;
	}
		
	TArray<FVector> SpawnedPoints;
	int32 MaxPlatforms;
	TArray<int32> MaxPlatformsVector;

	for (int32 ii = 0; ii < Amount; ++ii)
	{
		TArray<AActor*> PlatformPiecesVector;
		FVector PositionInBox = UKismetMathLibrary::RandomPointInBoundingBox(BoxPosition,BoxExtent);

		// Spawn Vagalume or the Platform with Archer Towers
		if (FMath::RandRange(0,4) == 0)
		{
			MaxPlatformsVector.Add(1);
			// Checks if the PositionInBox is far enough of other spawned elements
			while (!IsPointFarEnough(PositionInBox, SpawnedPoints, MinimalDistance, MaxPlatformsVector))
			{
				PositionInBox = UKismetMathLibrary::RandomPointInBoundingBox(BoxPosition,BoxExtent);
			}
			
			AVagalume* VagalumePiece = GetWorld()->SpawnActor<AVagalume>(VagalumeClass, PositionInBox, FRotator(0));
			VagalumePiece->SetLifeSpan(TimeToDeletePreviousPlatform);
			SpawnedPoints.Add(PositionInBox);
		}
		else
		{
			// Spawns Platforms and Archer towers
			
			MaxPlatforms = FMath::RandRange(1, MaxMiddlePlatforms);
			MaxPlatformsVector.Add(MaxPlatforms);

			// Checks if the PositionInBox is far enough of other spawned elements
			while (!IsPointFarEnough(PositionInBox, SpawnedPoints, MinimalDistance, MaxPlatformsVector))
			{
				PositionInBox = UKismetMathLibrary::RandomPointInBoundingBox(BoxPosition,BoxExtent);
			}

			SpawnedPoints.Add(PositionInBox);

			// Spawn Edge piece
			AActor* PlatformPiece = GetWorld()->SpawnActor<AActor>(EdgePieceClass, PositionInBox, FRotator(0));
			PlatformPiecesVector.Add(PlatformPiece);
			PlatformPiecesVector[0]->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			PlatformPiecesVector[0]->SetLifeSpan(TimeToDeletePreviousPlatform);		

			FVector NextPieceLocation = PlatformPiecesVector[0]->GetActorLocation();
			for (int32 i = 0; i < MaxPlatforms; ++i)
			{
				NextPieceLocation += FVector(SpaceBetweenPieces,0,0);
				PlatformPiece = GetWorld()->SpawnActor<AActor>(MiddlePieceClass, NextPieceLocation, FRotator(0));
				PlatformPiecesVector.Add(PlatformPiece);
				PlatformPiecesVector[i+1]->AttachToActor(PlatformPiecesVector[0], FAttachmentTransformRules::KeepWorldTransform);
				PlatformPiecesVector[i+1]->SetLifeSpan(TimeToDeletePreviousPlatform);

				// Spawn Archer Tower with 25% chance, by default
				if (FMath::RandRange(0,3) == 0)
				{
					GriffinAttackGameMode = Cast<AGriffinAttackGameMode>(UGameplayStatics::GetGameMode(this));
					GriffinAttackGameMode->SpawnArcherTower(SpaceBetweenPieces,NextPieceLocation);
				}

				// Last Edge piece
				if (i == (MaxPlatforms-1))
				{
					NextPieceLocation += FVector(SpaceBetweenPieces,0,0);
					PlatformPiece = GetWorld()->SpawnActor<AActor>(EdgePieceClass, NextPieceLocation, FRotator(0,180,0));
					PlatformPiecesVector.Add(PlatformPiece);
					PlatformPiecesVector[i+2]->AttachToActor(PlatformPiecesVector[i+1], FAttachmentTransformRules::KeepWorldTransform);
					PlatformPiecesVector[i+2]->SetLifeSpan(TimeToDeletePreviousPlatform);
				}
			}
		}
	}	
}

// Checks if the NewPoint if far enough of other spawned objects (Needs some polish)
bool APlatformsSpawner::IsPointFarEnough(FVector NewPoint, TArray<FVector> SpawnedPoints, FVector MinimalDistanceVector, TArray<int32> MaxPlatformsVector)
{
	//for (FVector &Point : SpawnedPoints)
	for (int i = 0; i < SpawnedPoints.Num(); ++i)
	{
		if (UKismetMathLibrary::Abs(SpawnedPoints[i].Z-NewPoint.Z) < MinimalDistanceVector.Z && UKismetMathLibrary::Abs(SpawnedPoints[i].X-NewPoint.X) < MinimalDistanceVector.X)
		{
			return false;
		}
		if (UKismetMathLibrary::Abs(SpawnedPoints[i].Z-NewPoint.Z) < MinimalDistanceVector.Z && UKismetMathLibrary::Abs(SpawnedPoints[i].X-NewPoint.X) < 400 * (MaxPlatformsVector[i]+2))
		{
			return false;
		}
	}
	return true;
}

