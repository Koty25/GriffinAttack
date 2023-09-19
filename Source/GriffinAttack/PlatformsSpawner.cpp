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
	
	// static ConstructorHelpers::FClassFinder<AActor> EdgePieceBPClass(TEXT("/Game/GriffinAttack/Blueprints/BP_EdgePiece"));
	// static ConstructorHelpers::FClassFinder<AActor> MiddlePieceBPClass(TEXT("/Game/GriffinAttack/Blueprints/BP_MiddlePiece"));

}

// Called when the game starts or when spawned
void APlatformsSpawner::BeginPlay()
{
	Super::BeginPlay();

	FVector SpawnLocation = PlatformPositions->GetComponentLocation();
	FVector BoxExtent = PlatformPositions->GetScaledBoxExtent();

	int32 PlatformAmount = FMath::RandRange(3, PlatformAmountMax);
	UE_LOG(LogTemp,Warning, TEXT("PlatformAmount: %i"), PlatformAmount);
	
	SpawnPlatforms(SpawnLocation, BoxExtent, PlatformAmount);
	
}

// Called every frame
void APlatformsSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlatformsSpawner::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == Cast<AGriffinCharacter>(UGameplayStatics::GetPlayerPawn(this,0)))
	{
		UE_LOG(LogTemp,Warning,TEXT("Spawn"))
		GriffinAttackGameMode = Cast<AGriffinAttackGameMode>(UGameplayStatics::GetGameMode(this));
		GriffinAttackGameMode->SpawnNextPlatforms();

		this->SetLifeSpan(TimeToDeletePreviousPlatform);
		//GetWorldTimerManager().SetTimer(DestroyPlatformTimer, this, &APlatformsSpawner::HandleDestruction, TimeToDeletePreviousPlatform);	
	}
}

void APlatformsSpawner::HandleDestruction()
{
	Destroy();
}

FVector APlatformsSpawner::GetNextPlatformLocation()
{
	return NextPlatformLocation->GetComponentLocation();
}

void APlatformsSpawner::SpawnPlatforms(FVector BoxPosition, FVector BoxExtent, int32 Amount)
{
	if (EdgePieceClass == nullptr && MiddlePieceClass == nullptr)
	{
		return;
	}

	UE_LOG(LogTemp,Warning, TEXT("SpawnPlatform"));
	
	//TArray<AActor*> PlatformPiecesVector;
	TArray<FVector> SpawnedPoints;
	int32 MaxPlatforms;
	TArray<int32> MaxPlatformsVector;

	for (int32 ii = 0; ii < Amount; ++ii)
	{
		TArray<AActor*> PlatformPiecesVector;
		FVector PositionInBox = UKismetMathLibrary::RandomPointInBoundingBox(BoxPosition,BoxExtent);

		if (FMath::RandRange(0,4) == 0)
		{
			MaxPlatformsVector.Add(1);
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
			MaxPlatforms = FMath::RandRange(1, MaxMiddlePlatforms);
			MaxPlatformsVector.Add(MaxPlatforms);
			UE_LOG(LogTemp,Warning, TEXT("MaxPlatforms: %i"), MaxPlatforms);
	
		

			while (!IsPointFarEnough(PositionInBox, SpawnedPoints, MinimalDistance, MaxPlatformsVector))
			{
				PositionInBox = UKismetMathLibrary::RandomPointInBoundingBox(BoxPosition,BoxExtent);
			}

			SpawnedPoints.Add(PositionInBox);
	
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
				
				if (FMath::RandRange(0,3) == 0)
				{
					// FActorSpawnParameters SpawnInfo;
					// SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					GriffinAttackGameMode = Cast<AGriffinAttackGameMode>(UGameplayStatics::GetGameMode(this));
					GriffinAttackGameMode->SpawnArcherTower(SpaceBetweenPieces,NextPieceLocation);					
					// FVector ArcherTowerLocation = NextPieceLocation + FVector(FMath::RandRange(-SpaceBetweenPieces/3, SpaceBetweenPieces/3), 0, 40);				
					// AArcherTower* ArcherTowerPiece = GetWorld()->SpawnActor<AArcherTower>(ArcherTowerClass, ArcherTowerLocation, FRotator(0));
					// //ArcherTowerPiece->AttachToActor(PlatformPiecesVector[i+1], FAttachmentTransformRules::KeepWorldTransform);
					// UE_LOG(LogTemp,Warning, TEXT("Spawn ArcherTower!"));
				}
		
		
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


		
		//int32 MaxPlatforms = FMath::RandRange(1, MaxMiddlePlatforms);
		
	}
	
	// int32 MaxPlatforms = FMath::RandRange(1, MaxMiddlePlatforms);
	//
	// FVector PositionInBox = UKismetMathLibrary::RandomPointInBoundingBox(BoxPosition,BoxExtent);
	//
	// while (!IsPointFarEnough(PositionInBox, SpawnedPoints, MinimalDistance, MaxPlatforms))
	// {
	// 	PositionInBox = UKismetMathLibrary::RandomPointInBoundingBox(BoxPosition,BoxExtent);
	// }
	//
	// SpawnedPoints.Add(PositionInBox);
	//
	// AActor* PlatformPiece = GetWorld()->SpawnActor<AActor>(EdgePieceClass, PositionInBox, FRotator(0));
	// PlatformPiecesVector.Add(PlatformPiece);
	// PlatformPiecesVector[0]->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	// PlatformPiecesVector[0]->SetLifeSpan(TimeToDeletePreviousPlatform);		
	//
	// FVector NextPieceLocation = PlatformPiecesVector[0]->GetActorLocation();
	// for (int32 i = 0; i < MaxPlatforms; ++i)
	// {
	// 	NextPieceLocation += FVector(SpaceBetweenPieces,0,0);
	// 	PlatformPiece = GetWorld()->SpawnActor<AActor>(MiddlePieceClass, NextPieceLocation, FRotator(0));
	// 	PlatformPiecesVector.Add(PlatformPiece);
	// 	PlatformPiecesVector[i+1]->AttachToActor(PlatformPiecesVector[0], FAttachmentTransformRules::KeepWorldTransform);
	// 	PlatformPiecesVector[i+1]->SetLifeSpan(TimeToDeletePreviousPlatform);
	// 	
	// 	
	// 	if (i == (MaxPlatforms-1))
	// 	{
	// 		NextPieceLocation += FVector(SpaceBetweenPieces,0,0);
	// 		PlatformPiece = GetWorld()->SpawnActor<AActor>(EdgePieceClass, NextPieceLocation, FRotator(0,180,0));
	// 		PlatformPiecesVector.Add(PlatformPiece);
	// 		PlatformPiecesVector[i+2]->AttachToActor(PlatformPiecesVector[i+1], FAttachmentTransformRules::KeepWorldTransform);
	// 		PlatformPiecesVector[i+2]->SetLifeSpan(TimeToDeletePreviousPlatform);
	// 	}
	// }
	
	// FVector NextPieceLocation = EdgePiece1->GetActorLocation();
	// NextPieceLocation += FVector(SpaceBetweenPieces,0,0);
	// AActor* MiddlePiece = GetWorld()->SpawnActor<AActor>(MiddlePieceClass, NextPieceLocation, FRotator(0));
	// MiddlePiece->AttachToActor(EdgePiece1, FAttachmentTransformRules::KeepWorldTransform);

	// NextPieceLocation += FVector(SpaceBetweenPieces,0,0);
	// AActor* EdgePiece2 = GetWorld()->SpawnActor<AActor>(EdgePieceClass, NextPieceLocation, FRotator(0,180,0));
	// EdgePiece2->AttachToActor(MiddlePiece, FAttachmentTransformRules::KeepWorldTransform);
	
}

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

