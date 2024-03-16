// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"

#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GraduationDesign/Weapon/WeaponBaseActor.h"
#include "Kismet/KismetMathLibrary.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	PlayerCharacter=Cast<APlayerCharacter>(TryGetPawnOwner());//检查代码当前作用的Actor的引用，如果能转换为PlayerCharacter就返回引用，失败就返回null
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
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

	//旋转平滑参数
	bRotateRootBone=PlayerCharacter->ShouldRotateRootBone();

	//无法理解下面这段代码的数学含义
	FRotator AimRotation=PlayerCharacter->GetBaseAimRotation();//?全局旋转
	FRotator MovementRotation=UKismetMathLibrary::MakeRotFromX(PlayerCharacter->GetVelocity());//?局部（根据玩家前进方向旋转）
	FRotator DeltaRot=UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation,AimRotation);
	DeltaRotation=FMath::RInterpTo(DeltaRotation,DeltaRot,DeltaTime,6.f);
	YawOffset=DeltaRotation.Yaw;
	
	CharacterRotationLastFrame=CharacterRotation;
	CharacterRotation=PlayerCharacter->GetActorRotation();
	const FRotator Delta=UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation,CharacterRotationLastFrame);
	const float Target=Delta.Yaw/DeltaTime;
	const float Interp=FMath::FInterpTo(Lean,Target,DeltaTime,6.f);
	Lean=FMath::Clamp(Interp,-90.f,90.0f);

	AO_Yaw=PlayerCharacter->GetAO_Yaw();
	AO_Pitch=PlayerCharacter->GetAO_Pitch();

	//IK武器左手
	EquippedWeapon=PlayerCharacter->GetEquippedWeapon();
	if(bWeaponEquipped&&EquippedWeapon&&EquippedWeapon->GetWeaponMesh()&&PlayerCharacter->GetMesh())
	{
		LeftHandTransform=EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"),RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		PlayerCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"),LeftHandTransform.GetLocation(),FRotator::ZeroRotator,OutPosition,OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		//调整武器指向子弹发射的方向

		//FTransform RightHandTransform=EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r"),RTS_World);
		//RightHandRotation=UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(),RightHandTransform.GetLocation()+(RightHandTransform.GetLocation()-PlayerCharacter->GetHitTarget()));
		
		// FTransform MuzzleTipTransform=EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"),RTS_World);
		// FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));
		// DrawDebugLine(GetWorld(),MuzzleTipTransform.GetLocation(),MuzzleTipTransform.GetLocation()+MuzzleX*10000.f,FColor::Red);
		// DrawDebugLine(GetWorld(),MuzzleTipTransform.GetLocation(),PlayerCharacter->GetHitTarget(),FColor::Orange);
		
		
	}

	TurningInPlace=PlayerCharacter->GetTurningInPlace();
}
