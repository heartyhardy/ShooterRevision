// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AShooterProjectile::AShooterProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	}

	if (!ProjectileCollision)
	{
		ProjectileCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCollisionComponent"));

		ProjectileCollision->InitSphereRadius(15.f);
		RootComponent = ProjectileCollision;
	}

	if (!ProjectileMovement)
	{
		ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

		ProjectileMovement->SetUpdatedComponent(ProjectileCollision);
		ProjectileMovement->InitialSpeed = 3000.f;
		ProjectileMovement->MaxSpeed = 3000.f;
		ProjectileMovement->bRotationFollowsVelocity = true;
		ProjectileMovement->bShouldBounce = true;
		ProjectileMovement->Bounciness = 0.3f;
		ProjectileMovement->ProjectileGravityScale = 0.f;
	}

}

// Called when the game starts or when spawned
void AShooterProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShooterProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShooterProjectile::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
}

