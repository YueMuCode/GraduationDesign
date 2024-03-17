// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "GraduationDesign/Interface/InterractWithCrosshairsInterface.h"
#include "GraduationDesign/PlayerComponent/CombatComponent.h"
#include "GraduationDesign/TypeFiles/TurningInPlace.h"
#include "Sound/SoundCue.h"
#include "GraduationDesign/TypeFiles/CombatState.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class GRADUATIONDESIGN_API APlayerCharacter : public ACharacter,public IInterractWithCrosshairsInterface
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

	//播放蒙太奇
	void PlayFireMontage(bool bAiming);
	void PlayHitReactMontage();
	void PlayElimMontage();
	void PlayReloadMontage();
	//调整枪口
	FVector GetHitTarget()const;

	//瞄准视野
	FORCEINLINE UCameraComponent*GetFollowCamera()const {return FollowCamera;}

	//服务器上多播受伤蒙太奇
	// UFUNCTION(NetMulticast,Unreliable)
	// void MulticastHit();

	//代理选择平滑
	FORCEINLINE bool ShouldRotateRootBone() const {return bRotateRootBone;}
	void CalculateAO_Pitch();
	virtual void OnRep_ReplicatedMovement() override;
	//被淘汰
	UFUNCTION(NetMulticast,Reliable)
	void MulticastElim();
	void Elim();//只发生在服务器上
	bool bElimmed=false;
	FORCEINLINE bool IsElimmed()const {return bElimmed;}

	//重生计时器
	FTimerHandle ElimTimer;
	UPROPERTY(EditDefaultsOnly)
	float ElimDelay=3.f;
	void ElimTimerFinished();

	//玩家死亡网格体溶解
	//相当于timeline节点
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DTimeline;//不知道为什么啊，这个bug不能使用DissolveTimeline这个变量名，不然会创建失败
	
	FOnTimelineFloat DissolveTrack;
	UFUNCTION()
	void UpdateDissolveeMaterial(float DissolveValue);
	void StartDissolve();
	//曲线
	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;
	//材质
	//动态材质，我们可以修改他的值在运行的时候
	UPROPERTY(VisibleAnywhere,Category=Elim)
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;
	//材质在蓝图上设置，设置了材质实例之后，由动态材质修改值
	UPROPERTY(EditAnywhere,Category=Elim)
	UMaterialInstance*DissolveMaterialInstance;

	//淘汰时出现的机器人
	UPROPERTY(EditAnywhere)
	UParticleSystem* ElimBotEffect;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ElimBotComponent;

	UPROPERTY(EditAnywhere)
	USoundCue* ElimBotSound;
	//处理机器人播放结束后消失
	virtual void Destroyed() override;
	
	//用于刷新玩家 血量
	FORCEINLINE  float GetHealth()const {return Health;}
	FORCEINLINE  float GetMAXHealth()const {return MaxHealth;}

	//初始化分数的UI（从0开始）
	void PollInit();
	UPROPERTY()
	class APlayerPlayerState* PlayerPlayerState;

	//获取combatstate以实现换弹功能
	ECombatState GetCombatState() const;
		
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
	void FireButtonPressed();
	void FireButtonReleased();
	void ReloadButtonPressed();
	
	void AimOffset(float DeltaTime);

	//代理旋转平滑
	void SimProxiesTurn();

	//子弹伤害
	UFUNCTION()
	void ReceiveDamage(AActor*DamageActor,float Damage,const UDamageType*DAmageType,class AController* InstigatorController,AActor*DamageCauser);

	//更新HPUI
	void UpdateHUDHealth();





	
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
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess="true"))
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

	//蒙太奇
	UPROPERTY(EditAnywhere,Category=Combat)
	UAnimMontage* FireWeaponMontage;

	//受伤动画
	UPROPERTY(EditAnywhere,Category=Combat)
	UAnimMontage* HitReactMontage;

	//淘汰蒙太奇动画
	UPROPERTY(EditAnywhere,Category=Combat)
	UAnimMontage* ElimMontage;

	//装弹蒙太奇动画
	UPROPERTY(EditAnywhere,Category=Combat)
	UAnimMontage* ReloadMontage;

	
	//防止相机怼到墙上丢失视野
	void HideCameraIfCharacterClose();
	UPROPERTY(EditAnywhere)
	float CameraThreshold=200.f;

	//代理旋转平滑
	bool bRotateRootBone;
	float TurnThreshold=0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();

	//玩家血量
	UPROPERTY(EditAnywhere,Category="PlayStats")
	float MaxHealth=100.f;

	UPROPERTY(ReplicatedUsing=OnRep_Health,VisibleAnywhere,Category="PlayStats")
	float Health=100.f;

	UFUNCTION()
	void OnRep_Health();

	UPROPERTY()
	AMyPlayerController*MyPlayerController;

	
};
