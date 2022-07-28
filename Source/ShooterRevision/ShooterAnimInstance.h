// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERREVISION_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	// Similar to Begin play
	virtual void NativeInitializeAnimation() override;

	// Similar to Tick
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "Movement", meta = (AllowPrivateAccess = "true"))
		class AShooterCharacter* ShooterCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "Movement", meta = (AllowPrivateAccess = "true"))
		float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bIsInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bIsAccelerating;
};
