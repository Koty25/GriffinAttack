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

	UFUNCTION()
	void HandleDestruction();
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool IsArcherTowerDead = false;

	AGriffinCharacter* GriffinCharacter;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* CapsuleComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TowerMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* ProjectileSpawnPoint;
	
	FTimerHandle FireRateTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class AArrowProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireRange = 500.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireRate = 2.f;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Health, meta = (AllowPrivateAccess = "true"))
	float ContactDamage = 50.f;
	
	void CheckFireCondition();

	bool InFireRange();

	void Fire();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
