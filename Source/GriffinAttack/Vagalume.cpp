// Fill out your copyright notice in the Description page of Project Settings.


#include "Vagalume.h"

#include "GriffinAttackGameMode.h"
#include "GriffinCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AVagalume::AVagalume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	RootComponent = CapsuleComp;
	VagalumeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Vagalume Mesh"));
	VagalumeMesh->SetupAttachment(CapsuleComp);

}

// Called when the game starts or when spawned
void AVagalume::BeginPlay()
{
	Super::BeginPlay();

	// OnHit event
	VagalumeMesh->OnComponentHit.AddDynamic(this, &AVagalume::OnHit);
	
}

// Called every frame
void AVagalume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// When the Griffin gets a Vagalume, heal the Griffin
void AVagalume::HealGriffin()
{
	GriffinCharacter = Cast<AGriffinCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (GriffinCharacter->Health != GriffinCharacter->MaxHealth)
	{
		GriffinCharacter->Health += VagalumeHeal;
	}
}

// On hit function
void AVagalume::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->IsA(AGriffinCharacter::StaticClass()))
	{
		AGriffinAttackGameMode* GameMode = GetWorld()->GetAuthGameMode<AGriffinAttackGameMode>();
		if (GameMode != nullptr)
		{
			GameMode->ActorHit(this);
		}
	}
}

// Vagalume's Death
void AVagalume::HandleDestruction()
{
	Destroy();
	IsVagalumeDead = true;
}



