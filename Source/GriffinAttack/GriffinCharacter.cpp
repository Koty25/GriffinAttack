// Fill out your copyright notice in the Description page of Project Settings.


#include "GriffinCharacter.h"

// #include "ArcherTower.h"
// #include "ArrowProjectile.h"
#include "CameraGriffin.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GriffinAttackCharacter.h"
#include "GriffinAttackGameMode.h"
// #include "GriffinPlayerController.h"
#include "InputTriggers.h"
// #include "Vagalume.h"
// #include "Camera/CameraComponent.h"
// #include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
// #include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

// Sets default values
AGriffinCharacter::AGriffinCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setup Character movement
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCharacterMovement()->BrakingDecelerationFlying = 1000.f;
	GetCharacterMovement()->MaxFlySpeed = FlyingSpeed;

	// Camera is now separate and spawned on begin play
	// Mesh and animation are on the derived blueprint.
}

// Called when the game starts or when spawned
void AGriffinCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Speed buildup timer 
	if (!GetWorldTimerManager().TimerExists(SpeedBuildUpTimer))
	{
		GetWorldTimerManager().SetTimer(SpeedBuildUpTimer, this, &AGriffinCharacter::SpeedBuildUp, BuildUpTimeDelta, true);
	}

	// Health
	Health = MaxHealth;

	// Camera
	ACameraGriffin* CameraFollow = GetWorld()->SpawnActor<ACameraGriffin>(CameraClass, GetActorLocation(), GetActorRotation().Add(0,-90,0));
	if (CameraFollow == nullptr)
	{
		return;
	}
	CameraFollow->SetOwner(this);
	CameraFollow->FollowCharacter = this;
	APlayerController* ThisPlayerController = UGameplayStatics::GetPlayerController(this,0);
	ThisPlayerController->SetViewTarget(CameraFollow);
}

// Called every frame
void AGriffinCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Constant x axis speed
	XAxisSpeedBuildup();

	// Dash speed increase
	if (IsDashing())
	{
		DashSpeedIncrease();	
	}
}

// Called to bind functionality to input
void AGriffinCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Dashing and Movement
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &AGriffinCharacter::Dash);
		// EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Completed, this, &AGriffinCharacter::StopDash);
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AGriffinCharacter::Movement);

	}
}

// Gives constant speed on the X axis, which is a factor of the max speed.
void AGriffinCharacter::XAxisSpeedBuildup()
{
	const FVector XAxis(1.f,0,0);
	AddMovementInput(XAxis, InitialSpeedFactor, true);
}

// Griffin Movement. The griffin can only move on the Z axis.
void AGriffinCharacter::Movement(const FInputActionValue& Value)
{	
	if (Controller != nullptr)
	{
		// Amount of desired movement
		float MovementAmount = Value.GetMagnitude() * InitialSpeedFactor*2;

		// Vector of movement
		const FVector ZAxis(0,0,1.f);
		
		// Add movement
		AddMovementInput(ZAxis, MovementAmount, true);
	}
}

// Dash happens in a DashInterval second interval. It adds 4/5 of the maximum speed to the character.
void AGriffinCharacter::Dash(const FInputActionValue& Value)
{
	
	if (DashUsed != DashMaximum)
	{
		// Setup timer to be used in Tick()
		GetWorldTimerManager().SetTimer(DashTimer, this, &AGriffinCharacter::ClearDashTimer, DashInterval);

		// Dash Line Niagara system
		if (DashLines)
		{
			DashLinesComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, DashLines, GetActorLocation());
			DashLinesComponent->Activate();
		}
		
		// Dashes used and delay timer
		DashUsed++;
		if (!GetWorldTimerManager().TimerExists(DashResetTimer))
		{
			GetWorldTimerManager().SetTimer(DashResetTimer, this, &AGriffinCharacter::ResetDashCharges, DashRechargeDelay);
		}
	}
}

// Reset the Dash Charges
void AGriffinCharacter::ResetDashCharges()
{
	DashUsed = 0;
	GetWorldTimerManager().ClearTimer(DashResetTimer);
}

// Delete dash timer
void AGriffinCharacter::ClearDashTimer()
{
	GetWorldTimerManager().ClearTimer(DashTimer);
	if (DashLines)
	{
		DashLinesComponent->Deactivate();
	}
}

// X axis speed build up. Increases the speed on the X axis by Y (SpeedIncrease) amount each Z (BuildUpTimeDelta) seconds.
void AGriffinCharacter::SpeedBuildUp()
{
	GetCharacterMovement()->MaxFlySpeed += SpeedIncrease;
}

// Speed increase due to Dashing
void AGriffinCharacter::DashSpeedIncrease()
{
	const FVector XAxis(1.f,0,0);
	AddMovementInput(XAxis, InitialSpeedFactor*DashVelocityMultiplier, true);
}

// Checks if Griffin is dead
bool AGriffinCharacter::IsDead()
{
	return Health <= 0.f;
}

// Checks if Griffin is Dashing
bool AGriffinCharacter::IsDashing()
{
	return GetWorldTimerManager().TimerExists(DashTimer);
}

// Return Health percentage
float AGriffinCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}

// Take Damage function and kills this pawn if IsDead() is true
float AGriffinCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                    AActor* DamageCauser)
{
	float DamageToApply =  Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	DamageToApply = FMath::Min(Health,DamageToApply);
	Health -= DamageToApply;

	if (IsDead())
	{
		AGriffinAttackGameMode* GameMode = GetWorld()->GetAuthGameMode<AGriffinAttackGameMode>();
		if (GameMode != nullptr)
		{
			GameMode->PawnKilled(this);
		}
		DetachFromControllerPendingDestroy();
		// GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	return DamageToApply;
}

