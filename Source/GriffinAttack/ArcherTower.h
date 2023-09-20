// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GriffinCharacter.h"
#include "GameFramework/Pawn.h"
#include "ArcherTower.generated.h"

UCLASS()
class GRIFFINATTACK_API AArcherTower : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AArcherTower();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Archer Tower's Death
	UFUNCTION()
	void HandleDestruction();
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool IsArcherTowerDead = false;

	AGriffinCharacter* GriffinCharacter;

private:
	// Archer tower Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* CapsuleComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TowerMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* ProjectileSpawnPoint;

	FTimerHandle FireRateTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class AArrowProjectile> ProjectileClass;

	// Range of the Archer Tower
	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireRange = 500.f;

	// Fire rate of the Archer Tower
	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireRate = 2.f;

	// Damage of the Archer Tower
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Health, meta = (AllowPrivateAccess = "true"))
	float ContactDamage = 50.f;

	// Checks if in range and if Griffin is alive
	void CheckFireCondition();

	// Range check
	bool InFireRange();

	// Firing arrow
	void Fire();

	// OnHit function
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
