// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class GRADUATIONDESIGN_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(BlueprintReadOnly,Category=Character,meta=(AllowPrivateAccess=true))
	class APlayerCharacter* PlayerCharacter;

	UPROPERTY(BlueprintReadOnly,Category=Character,meta=(AllowPrivateAccess=true))
	float Speed;

	UPROPERTY(BlueprintReadOnly,Category=Character,meta=(AllowPrivateAccess=true))
	bool bIsInAir;

	UPROPERTY(BlueprintReadOnly,Category=Character,meta=(AllowPrivateAccess=true))
	bool bIsAccelerating;

	UPROPERTY(BlueprintReadOnly,Category=Character,meta=(AllowPrivateAccess=true))
	bool bWeaponEquipped;

	UPROPERTY(BlueprintReadOnly,Category=Character,meta=(AllowPrivateAccess=true))
	bool bIsCrouched;

	UPROPERTY(BlueprintReadOnly,Category=Character,meta=(AllowPrivateAccess=true))
	bool bIsAiming;

	//以下变量作用于持枪动画
	UPROPERTY(BlueprintReadOnly,Category=Character,meta=(AllowPrivateAccess=true))
	float YawOffset;

	UPROPERTY(BlueprintReadOnly,Category=Character,meta=(AllowPrivateAccess=true))
	float Lean;

	FRotator CharacterRotationLastFrame;
	FRotator CharacterRotation;
	FRotator DeltaRotation;
};
