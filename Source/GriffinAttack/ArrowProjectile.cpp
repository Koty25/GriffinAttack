// Fill out your copyright notice in the Description page of Project Settings.


#include "ArrowProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AArrowProjectile::AArrowProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	RootComponent = ProjectileMesh;
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	// ProjectileTrace = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Projectile Trace"));
	// ProjectileTrace->SetupAttachment(RootComponent);
	
	ProjectileMovementComponent->InitialSpeed = 600.f;
	ProjectileMovementComponent->MaxSpeed = 700.f;

}

// Called when the game starts or when spawned
void AArrowProjectile::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMesh->OnComponentHit.AddDynamic(this, &AArrowProjectile::OnHit);
	if (LaunchSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, LaunchSound, GetActorLocation());
	}
	GetWorldTimerManager().SetTimer(DestroyArrowTimer, this, &AArrowProjectile::HandleDestruction, TimeToDeleteArrow);
}

// Called every frame
void AArrowProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AArrowProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	AActor* MyOwner = GetOwner();
	if (MyOwner == nullptr) return;

	AController* MyOwnerInstigator = MyOwner->GetInstigatorController();
	UClass* DamageTypeClass = UDamageType::StaticClass();

	if (OtherActor && OtherActor != this && OtherActor != MyOwner)
	{
		UGameplayStatics::ApplyDamage(OtherActor, Damage, MyOwnerInstigator, this, DamageTypeClass);
		if (HitParticles) 
		{
			UGameplayStatics::SpawnEmitterAtLocation(this, HitParticles, GetActorLocation(), GetActorRotation());
		}
		Destroy();
	}
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
	}
	
}

void AArrowProjectile::HandleDestruction()
{
	Destroy();
}

