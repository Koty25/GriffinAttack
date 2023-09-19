// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArrowProjectile.generated.h"

UCLASS()
class GRIFFINATTACK_API AArrowProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AArrowProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ProjectileMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovementComponent;
	// UPROPERTY(VisibleAnywhere, Category = "Combat")
	// class UParticleSystemComponent* ProjectileTrace;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void HandleDestruction();

	UPROPERTY(EditAnywhere)
	float Damage = 50.f;
	UPROPERTY(EditAnywhere)
	float TimeToDeleteArrow = 20.f;
	FTimerHandle DestroyArrowTimer;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UParticleSystem* HitParticles;
	UPROPERTY(EditAnywhere, Category = "Combat")
	class USoundBase* LaunchSound;
	UPROPERTY(EditAnywhere, Category = "Combat")
	USoundBase* HitSound;

};
