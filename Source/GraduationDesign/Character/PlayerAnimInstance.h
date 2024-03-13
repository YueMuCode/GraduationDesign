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
};
