// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "GriffinCharacter.generated.h"

struct FInputActionValue;

UCLASS()
class GRIFFINATTACK_API AGriffinCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGriffinCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Movement and Dash Function
	void Movement(const FInputActionValue& Value);

	void Dash(const FInputActionValue& Value);

	// void StopDash(const FInputActionValue& Value);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	/** ==========Variables========== */
	

	// CameraClass
	UPROPERTY(EditAnywhere, Category = "Camera")
	TSubclassOf<class ACameraGriffin> CameraClass;
	
	
	// Input Mapping Context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	// Dash Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DashAction;

	// Movement Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MovementAction;

	// Flying Properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Flying, meta = (AllowPrivateAccess = "true"))
	float FlyingSpeed = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Flying, meta = (AllowPrivateAccess = "true"))
	float InitialSpeedFactor = 1.f / 5.f;

	// Dash Ability Variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dash, meta = (AllowPrivateAccess = "true"))
	int32 DashMaximum = 3; // 3 Maximum dashes. Resets back to 0 every 10 seconds (not in use)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dash, meta = (AllowPrivateAccess = "true"))
	int32 DashUsed = 0; // Amount of Dashes used

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dash, meta = (AllowPrivateAccess = "true"))
	int32 DashVelocityMultiplier = 3; // Multiplies current X-axis velocity

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dash, meta = (AllowPrivateAccess = "true"))
	float DashRechargeDelay = 5.f; // How many seconds to recharge after first Dash use

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dash, meta = (AllowPrivateAccess = "true"))
	float DashInterval = 0.5f; // Dash time interval	

	FTimerHandle DashTimer;
	FTimerHandle DashResetTimer;

	// X axis speed buildup
	float BuildUpTimeDelta = 1.f;	// [s]
	FTimerHandle SpeedBuildUpTimer;
	float SpeedIncrease = 5.f;

	// Health
	bool bGriffinAlive = true;
	float Health;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Health, meta = (AllowPrivateAccess = "true"))	
	float MaxHealth = 100.f;

	// Dash Lines
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* DashLines;
	
	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* DashLinesComponent;

	FTimerHandle DashLineTimer;
	

	/** ==========Functions========== */

	// Constant speed on the X axis
	UFUNCTION()
	void XAxisSpeedBuildup();

	// Resets Dash charges
	UFUNCTION()
	void ResetDashCharges();

	// Clears Dash timer
	UFUNCTION()
	void ClearDashTimer();

	// X axis buildup
	UFUNCTION()
	void SpeedBuildUp();

	// Speed increase due to Dashing
	UFUNCTION()
	void DashSpeedIncrease();

	// Checks if Griffin is dead
	UFUNCTION(BlueprintPure)
	bool IsDead();

	// Checks if Griffin is Dashing
	UFUNCTION(BlueprintPure)
	bool IsDashing();

	// Returns Health percentage
	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

	// Take Damage function
	UFUNCTION()
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

};
