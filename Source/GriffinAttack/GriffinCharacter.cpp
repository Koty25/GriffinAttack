// Fill out your copyright notice in the Description page of Project Settings.


#include "GriffinCharacter.h"

#include "ArcherTower.h"
#include "ArrowProjectile.h"
#include "CameraGriffin.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GriffinAttackCharacter.h"
#include "GriffinAttackGameMode.h"
#include "GriffinPlayerController.h"
#include "InputTriggers.h"
#include "Vagalume.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
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

	// Camera is now seperate and spawned on begin play
	// // Setup Character components (Camera Spring Arm, Camera...)
	// CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Spring Arm"));
	// CameraSpringArm->SetupAttachment(RootComponent);
	// CameraSpringArm->TargetArmLength = 1000.f;

	// Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	// Camera->SetupAttachment(CameraSpringArm,USpringArmComponent::SocketName);

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

	XAxisSpeedBuildup();

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
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Completed, this, &AGriffinCharacter::StopDash);
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AGriffinCharacter::Movement);

	}
}

void AGriffinCharacter::XAxisSpeedBuildup()
{
	const FVector XAxis(1.f,0,0);
	AddMovementInput(XAxis, InitialSpeedFactor, true);
}

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
		// // Desired distance for the dash
		// float DashDistance = Value.GetMagnitude() * GetCharacterMovement()->MaxFlySpeed * DashVelocityMultiplier;
		//
		// // The delta movement in delta seconds
		// FVector DeltaLocation = FVector::ZeroVector;
		// DeltaLocation.X = DashDistance * UGameplayStatics::GetWorldDeltaSeconds(this);
		//
		// // Add movement as an offset to not interfere with X axis movement
		// AddActorLocalOffset(DeltaLocation, true);

		// Setup timer to be used in Tick()

		GetWorldTimerManager().SetTimer(DashTimer, this, &AGriffinCharacter::ClearDashTimer, DashInterval);

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
		// FVector LaunchVelocity = this->GetVelocity();
		// float DashAmount = Value.GetMagnitude() * this->GetVelocity().X * DashVelocityMultiplier;
		// UE_LOG(LogTemp, Warning, TEXT("dash amount is %f and launch velocity is %f, %f, %f"), DashAmount, LaunchVelocity.X, LaunchVelocity.Y, LaunchVelocity.Z);
		// GetCharacterMovement()->MaxFlySpeed += 100.f;
		// AddMovementInput(LaunchVelocity, DashAmount);
		//LaunchCharacter(LaunchVelocity,0,0);
		//DashUsed++; Not used for now, might change later		
	}
}

void AGriffinCharacter::ResetDashCharges()
{
	DashUsed = 0;
	GetWorldTimerManager().ClearTimer(DashResetTimer);
}

void AGriffinCharacter::ClearDashTimer()
{
	GetWorldTimerManager().ClearTimer(DashTimer);
	if (DashLines)
	{
		DashLinesComponent->Deactivate();
	}
}

void AGriffinCharacter::SpeedBuildUp()
{
	GetCharacterMovement()->MaxFlySpeed += SpeedIncrease;
	//GetWorldTimerManager().ClearTimer(SpeedBuildUpTimer);
	UE_LOG(LogTemp, Warning, TEXT("speed increased to %f"), GetCharacterMovement()->MaxFlySpeed);
}

void AGriffinCharacter::DashSpeedIncrease()
{
	const FVector XAxis(1.f,0,0);
	AddMovementInput(XAxis, InitialSpeedFactor*DashVelocityMultiplier, true);
}

void AGriffinCharacter::StopDash(const FInputActionValue& Value)
{
	
	//GetCharacterMovement()->MaxFlySpeed = GetCharacterMovement()->MaxFlySpeed - 200.f;
	// FVector LaunchVelocity = this->GetVelocity();
	// GetCharacterMovement()->MaxFlySpeed -= 100.f;
	// float DashAmount = Value.GetMagnitude() * this->GetVelocity().X * DashVelocityMultiplier *(-1);
	// AddMovementInput(LaunchVelocity, DashAmount);
	// UE_LOG(LogTemp, Warning, TEXT("max fly speed is %f launch velocity is %f, %f, %f"), GetCharacterMovement()->MaxFlySpeed, LaunchVelocity.X, LaunchVelocity.Y, LaunchVelocity.Z);
}

bool AGriffinCharacter::IsDead()
{
	return Health <= 0.f;
}

bool AGriffinCharacter::IsDashing()
{
	return GetWorldTimerManager().TimerExists(DashTimer);
}

float AGriffinCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;

}

float AGriffinCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                    AActor* DamageCauser)
{
	float DamageToApply =  Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	DamageToApply = FMath::Min(Health,DamageToApply);
	Health -= DamageToApply;
	UE_LOG(LogTemp, Warning, TEXT("Health: %f"), Health);

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

