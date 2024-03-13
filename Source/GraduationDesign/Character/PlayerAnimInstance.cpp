// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"

#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	PlayerCharacter=Cast<APlayerCharacter>(TryGetPawnOwner());//检查代码当前作用的Actor的引用，如果能转换为PlayerCharacter就返回引用，失败就返回null
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if(PlayerCharacter==nullptr)
	{
		PlayerCharacter=Cast<APlayerCharacter>(TryGetPawnOwner());
	}
	if(PlayerCharacter==nullptr)return;//如果实在还是拿不到值，就只能提前中断函数，防止空指针操作

	//给各个变量参数赋值，作用于动画蓝图
	FVector Velocity=PlayerCharacter->GetVelocity();
	Velocity.Z=0.f;
	Speed=Velocity.Size();
	
	bIsInAir=PlayerCharacter->GetCharacterMovement()->IsFalling();

	bIsAccelerating=PlayerCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size()>0.f?true:false;

	bWeaponEquipped=PlayerCharacter->IsWeaponEquipped();

	bIsCrouched=PlayerCharacter->bIsCrouched;//使用框架提供的变量，这个变量是已经可复制的

	bIsAiming=PlayerCharacter->IsAiming();
}
