// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "GraduationDesign/PlayerComponent/CombatComponent.h"
#include "GraduationDesign/TypeFiles/TurningInPlace.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class GRADUATIONDESIGN_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;//系统提供的函数，指定对应的变量进行复制
	virtual void PostInitializeComponents() override;//通常被用于在一个Actor或者Component被创建后立即执行一些额外的初始化操作
	void SetOverlapWeapon(AWeaponBaseActor*Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	FORCEINLINE float GetAO_Yaw() const {return AO_Yaw;}
	FORCEINLINE float GetAO_Pitch()const {return AO_Pitch;}
	AWeaponBaseActor* GetEquippedWeapon();

	FORCEINLINE ETurningInPlace GetTurningInPlace() const {return TurningInPlace;}
protected:
	void MoveForward(float value);
	void MoveRight(float value);
	void Turn(float value);
	void LookUp(float value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	virtual void Jump() override;
	
	void AimOffset(float DeltaTime);

private:

	//添加必要的两个组件，相机和弹簧臂
	UPROPERTY(VisibleAnywhere,Category=Camera)
	class USpringArmComponent* CameraArm;

	UPROPERTY(VisibleAnywhere,Category=Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(ReplicatedUsing=OnRep_OverLapWeapon)//当overlapweapon被复制的时候，应该要调用OnRep_OverLapWeapon函数
	 AWeaponBaseActor* OverlapWeapon;

	UFUNCTION()
	void OnRep_OverLapWeapon(AWeaponBaseActor* LastWeapon);//这个函数的第一个参数必须和(ReplicatedUsing宏下的变量类型一致
	

	//UI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	//自定义组件
	UPROPERTY(VisibleAnywhere)
	UCombatComponent* CombatComponent;

	//RPC
	UFUNCTION(Server,Reliable)
	void ServerEquipButtonPressed();//客户端调用，服务器执行


	float AO_Yaw;
	float InterpAO_Yaw;//原地转身插值
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);
};
