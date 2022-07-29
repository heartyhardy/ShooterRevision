// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystem.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ShooterProjectile.h"
#include "Sound/SoundCue.h"
#include "ShooterCharacter.h"

// Sets default values
AShooterCharacter::AShooterCharacter():
	BaseTurnRate(45.f),
	BaseLookupRate(45.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Don't Rotate When Controller Rotates. Let the controller only affect camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Orient Character to Movment
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// Rotation Rate of the Character when orienting
	GetCharacterMovement()->RotationRate = FRotator{ 0.f, 540.f, 0.f };
	// Jump Z velocity
	GetCharacterMovement()->JumpZVelocity = 600.f;
	// Air Control
	GetCharacterMovement()->AirControl = 0.2f;
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AShooterCharacter::MoveForward(float Value)
{
	if (Controller && Value != 0.f)
	{
		// Find Foward
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0.f, Rotation.Yaw, 0.f };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if (Controller && Value != 0.f)
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0.f, Rotation.Yaw, 0.f };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookupAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookupRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::FireWeapon()
{
	if (ProjectileClass)
	{
		// Play Shooting Sound
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				GetWorld(),
				FireSound,
				GetActorLocation()
			);
		}

		// Check for Weapon_L Muzzle Socket
		const USkeletalMeshSocket* LeftBarrelSocket = GetMesh()->GetSocketByName("weapon_muzzle_l");
		const USkeletalMeshSocket* RightBarrelSocket = GetMesh()->GetSocketByName("weapon_muzzle_r");

		// Left Weapon MuzzleFlash
		if (LeftBarrelSocket)
		{
			const FTransform BarrelSocketTransform = LeftBarrelSocket->GetSocketTransform(GetMesh());

			if (MuzzleFlash)
			{
				UGameplayStatics::SpawnEmitterAttached(
					MuzzleFlash,
					GetMesh(),
					NAME_None,
					BarrelSocketTransform.GetLocation(),
					BarrelSocketTransform.GetRotation().Rotator(),
					EAttachLocation::KeepWorldPosition
				);
			}
		}

		// Right Weapon MuzzleFlash
		if (RightBarrelSocket)
		{
			const FTransform BarrelSocketTransform = RightBarrelSocket->GetSocketTransform(GetMesh());

			if (MuzzleFlash)
			{
				UGameplayStatics::SpawnEmitterAttached(
					MuzzleFlash,
					GetMesh(),
					NAME_None,
					BarrelSocketTransform.GetLocation(),
					BarrelSocketTransform.GetRotation().Rotator(),
					EAttachLocation::KeepWorldPosition
				);
			}
		}

		// Spawn Projectile
		UWorld* World = GetWorld();

		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			const FTransform BarrelLeftSocketTransform = LeftBarrelSocket->GetSocketTransform(GetMesh());
			const FTransform BarrelRightSocketTransform = RightBarrelSocket->GetSocketTransform(GetMesh());

			// Spawn Left Projectile
			AShooterProjectile* LeftProjectile = World->SpawnActor<AShooterProjectile>(
				ProjectileClass,
				BarrelLeftSocketTransform.GetLocation(),
				BarrelLeftSocketTransform.GetRotation().Rotator(),
				SpawnParams
			);

			if (LeftProjectile)
			{
				FVector LaunchDirection = BarrelLeftSocketTransform.GetRotation().Vector();
				LeftProjectile->FireInDirection(LaunchDirection);
			}

			// Spawn Right Projectile
			AShooterProjectile* RightProjectile = World->SpawnActor<AShooterProjectile>(
				ProjectileClass,
				BarrelRightSocketTransform.GetLocation(),
				BarrelRightSocketTransform.GetRotation().Rotator(),
				SpawnParams
				);

			if (RightProjectile)
			{
				FVector LaunchDirection = BarrelRightSocketTransform.GetRotation().Vector();
				RightProjectile->FireInDirection(LaunchDirection);
			}
		}
	}
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ThisClass::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ThisClass::MoveRight);

	PlayerInputComponent->BindAxis("TurnRate", this, &ThisClass::TurnAtRate);
	PlayerInputComponent->BindAxis("LookupRate", this, &ThisClass::LookupAtRate);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Lookup", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ThisClass::Jump);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ThisClass::StopJumping);

	PlayerInputComponent->BindAction("FireButton", EInputEvent::IE_Pressed, this, &ThisClass::FireWeapon);
}

