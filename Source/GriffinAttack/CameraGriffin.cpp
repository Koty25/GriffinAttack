// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraGriffin.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ACameraGriffin::ACameraGriffin()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setup Character components (Camera Spring Arm, Camera...)
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));	
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Spring Arm"));
	CameraSpringArm->SetupAttachment(RootComponent);
	CameraSpringArm->TargetArmLength = 1000.f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraSpringArm,USpringArmComponent::SocketName);

}

// Called when the game starts or when spawned
void ACameraGriffin::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACameraGriffin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCameraPosition();
}

void ACameraGriffin::UpdateCameraPosition()
{
	FVector NewLocation = FMath::VInterpTo(GetActorLocation(), FollowCharacter->GetActorLocation(), GetWorld()->GetDeltaSeconds(), 5.f);
	NewLocation.Z = GetActorLocation().Z;
	this->SetActorLocation(NewLocation);
}

