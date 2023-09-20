// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcherTower.h"

#include "ArrowProjectile.h"
#include "GriffinAttackGameMode.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AArcherTower::AArcherTower()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Component Creation
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	RootComponent = CapsuleComp;
	TowerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tower Mesh"));
	TowerMesh->SetupAttachment(CapsuleComp);
	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(TowerMesh);

}

// Called when the game starts or when spawned
void AArcherTower::BeginPlay()
{
	Super::BeginPlay();

	GriffinCharacter = Cast<AGriffinCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	// Timer to set the firing rate
	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &AArcherTower::CheckFireCondition, FireRate, true);

	// OnHit event
	TowerMesh->OnComponentHit.AddDynamic(this, &AArcherTower::OnHit);
	
}

// Called every frame
void AArcherTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AArcherTower::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// Checks if Griffin is in range and alive
void AArcherTower::CheckFireCondition()
{
	if (InFireRange() && GriffinCharacter->bGriffinAlive)
	{
		Fire();
	}
}

// In range check
bool AArcherTower::InFireRange()
{
	if (GriffinCharacter)
	{
		float Distance = FVector::Dist(GetActorLocation(), GriffinCharacter->GetActorLocation());

		if (Distance <= FireRange)
		{
			return true;
		}
	}
	return false;
}

// Fire the arrow, spawning it at the projectile spawn point
void AArcherTower::Fire()
{
	FVector ProjectileSpawnPointLocation = ProjectileSpawnPoint->GetComponentLocation();
	// Might add a rotation to give the arrow more distance and an arch
	FRotator ProjectileSpawnPointRotation = ProjectileSpawnPoint->GetComponentRotation();

	AArrowProjectile* Projectile = GetWorld()->SpawnActor<AArrowProjectile>(ProjectileClass, ProjectileSpawnPointLocation, ProjectileSpawnPointRotation);
	Projectile->SetOwner(this);
}

// On Hit function. If the Griffin is dashing, the Archer Tower dies, if not, the Griffin dies.
void AArcherTower::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	AActor* MyOwner = GetOwner();
	if (MyOwner == nullptr) return;
	GriffinCharacter = Cast<AGriffinCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (GriffinCharacter == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("Empty character"));
		return;
	}

	if (GetWorldTimerManager().TimerExists(GriffinCharacter->DashTimer) && OtherActor->IsA(AGriffinCharacter::StaticClass()))
	{
		AGriffinAttackGameMode* GameMode = GetWorld()->GetAuthGameMode<AGriffinAttackGameMode>();
		if (GameMode != nullptr)
		{
			GameMode->PawnKilled(this);
		}
	}
	else
	{
		AController* MyOwnerInstigator = MyOwner->GetInstigatorController();
		UClass* DamageTypeClass = UDamageType::StaticClass();

		if (OtherActor && OtherActor != this && OtherActor != MyOwner)
		{
			UGameplayStatics::ApplyDamage(OtherActor, ContactDamage, MyOwnerInstigator, this, DamageTypeClass);
		}
	}
}

// Archer Tower Destruction
void AArcherTower::HandleDestruction()
{
	Destroy();
	IsArcherTowerDead = true;
}

