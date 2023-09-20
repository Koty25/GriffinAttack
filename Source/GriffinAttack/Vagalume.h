// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Vagalume.generated.h"

UCLASS()
class GRIFFINATTACK_API AVagalume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVagalume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	class AGriffinCharacter* GriffinCharacter;

	// Vagalume components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* CapsuleComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* VagalumeMesh;

public:

	// Amount the Griffin is healed for
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Healing, meta = (AllowPrivateAccess = "true"))
	float VagalumeHeal = 10.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool IsVagalumeDead = false;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void HealGriffin();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void HandleDestruction();

};
