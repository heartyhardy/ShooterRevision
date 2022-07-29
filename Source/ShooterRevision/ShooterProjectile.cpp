// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AShooterProjectile::AShooterProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!ProjectileCollision)
	{
		ProjectileCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCollisionComponent"));

		ProjectileCollision->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));

		ProjectileCollision->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);

		ProjectileCollision->InitSphereRadius(15.f);
		RootComponent = ProjectileCollision;
	}

	if (!ProjectileMovement)
	{
		ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

		ProjectileMovement->SetUpdatedComponent(ProjectileCollision);
		ProjectileMovement->InitialSpeed = 5000.f;
		ProjectileMovement->MaxSpeed = 5000.f;
		ProjectileMovement->bRotationFollowsVelocity = true;
		ProjectileMovement->bShouldBounce = true;
		ProjectileMovement->Bounciness = 0.3f;
		ProjectileMovement->ProjectileGravityScale = 0.f;
	}

	if (!ProjectileMesh)
	{
		ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		//static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("StaticMesh'/Game/ParagonLtBelica/FX/Meshes/Heroes/SM_PlasmaShot_Shell.SM_PlasmaShot_Shell'"));

		//if (Mesh.Succeeded())
		//{
		//	ProjectileMesh->SetStaticMesh(Mesh.Object);
		//}

		//static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("Material'/Game/_Game/Assets/Materials/Projectiles/MAT_DefaultProjectile.MAT_DefaultProjectile'"));

		//if (Material.Succeeded())
		//{
		//	ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, ProjectileMesh);
		//}

		//ProjectileMesh->SetMaterial(0, ProjectileMaterialInstance);

		if (ProjectileMesh)
		{
			ProjectileMesh->SetRelativeScale3D(FVector(0.05f, 0.05f, 0.05f));
			ProjectileMesh->SetupAttachment(RootComponent);
		}
	}

	InitialLifeSpan = 3.0f;

}

// Called when the game starts or when spawned
void AShooterProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void AShooterProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor) return;
	if (!OtherComponent) return;
	if (!ProjectileMovement) return;

	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ImpactParticles,
			Hit.ImpactPoint
		);
	}

	if (OtherActor != this && OtherComponent->IsSimulatingPhysics())
	{
		OtherComponent->AddImpulseAtLocation(ProjectileMovement->Velocity * 100.f, Hit.ImpactPoint);
	}

	Destroy();
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

