// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformsSpawner.generated.h"


UCLASS()
class GRIFFINATTACK_API APlatformsSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlatformsSpawner();

private:

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* NextPlatformLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* InvisibleWallTop;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* InvisibleWallBottom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* NewPlatformsTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* PlatformPositions;

	class AGriffinCharacter* GriffinCharacter;
	class AGriffinAttackGameMode* GriffinAttackGameMode;

	FTimerHandle DestroyPlatformTimer;

	// Platform lifespan
	UPROPERTY(EditAnywhere)
	float TimeToDeletePreviousPlatform= 60.f;

	// Max amount of middle platforms. The total size of the platform will always be MaxMiddlePlatforms + 2
	UPROPERTY(EditAnywhere)
	int32 MaxMiddlePlatforms = 3;

	// Classes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AActor> EdgePieceClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AActor> MiddlePieceClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AVagalume> VagalumeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AArcherTower> ArcherTowerClass;

	// Minimal distance from another spawned element
	UPROPERTY(EditAnywhere)
	FVector MinimalDistance = FVector(150,0, 200);

	// Maximum amount of Elements spawned by the SpawnPlatform function. (Might need some refactoring for clarity)
	UPROPERTY(EditAnywhere)
	int32 PlatformAmountMax = 5;

	// Space Between each Platform piece
	UPROPERTY(EditAnywhere)
	float SpaceBetweenPieces = 190.f;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleDestruction();

	UFUNCTION()
	FVector GetNextPlatformLocation();

	UFUNCTION()
	void SpawnPlatforms(FVector BoxPosition, FVector BoxExtent, int32 Amount);

	UFUNCTION()
	bool IsPointFarEnough(FVector NewPoint, TArray<FVector> SpawnedPoints, FVector MinimalDistanceVector, TArray<int32> MaxPlatformsVector);

};
