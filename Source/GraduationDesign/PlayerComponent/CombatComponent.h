// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GraduationDesign/HUD/PlayerHUD.h"
#include "GraduationDesign/PlayerController/MyPlayerController.h"
#include "GraduationDesign/TypeFiles/CombatState.h"
#include "GraduationDesign/Weapon/WeaponTypes.h"
#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GRADUATIONDESIGN_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UCombatComponent();
	friend class APlayerCharacter;//友类的成员函数就可以访问另一个类(APlayerCharacter)的私有成员和受保护成员

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;//系统提供的函数，指定对应的变量进行复制
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void  SetAiming(bool bIsAiming);

	UFUNCTION(Server,Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	void FireButtonPressed(bool bPressed);

	//实现开火动画的RPC
	UFUNCTION(Server,Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	//多播RPC实现开火动画
	UFUNCTION(NetMulticast,Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	//射击瞄准射线
	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	//绘制武器准心
	void SetHUDCrossairs(float delta);

	//Reload功能
	void Reload();
	UFUNCTION(Server,Reliable)
	void ServerReload();
	void HandleReload();
	//用于刷新Reload状态
	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	//换弹逻辑
	int32 AmountToReload();
	void UpdateAmmoValues();

	
private:
	APlayerCharacter* Character;

	UPROPERTY(ReplicatedUsing=OnRep_EquippedWeapon)
	class AWeaponBaseActor* EquippedWeapon;

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	bool bFireButtonPressed;

	

	//绘制武器准心
	AMyPlayerController* Controller;
	APlayerHUD* HUD;
	FHUDPackage HUDPackage;
	//计算准心抖动
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairInAimFactor;
	float CrosshairShootingFactor;
	//调整枪口
	FVector HitTarget;

	//瞄准视野变大
	float DefaultFOV;

	UPROPERTY(EditAnywhere,Category=Combat)
	float ZoomedFOV=30.f;
	UPROPERTY(EditAnywhere,Category=Combat)
	float ZoomInterSpeed=20.f;
	UPROPERTY(EditAnywhere,Category=Combat)
	float CurrentFOV;
	void InterpFOV(float DeltaTime);

	//自动开火
	FTimerHandle FireTimer;
	bool bCanFire=true;
	void StartFireTimer();
	void FireTimerFinished();
	void Fire();

	//处理子弹小于零仍旧能开火的情况
	bool CanFire();
	//子弹容量
	UPROPERTY(ReplicatedUsing=OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	TMap<EWeaponType,int32> CarriedAmmoMap;

	//初始弹药容量
	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo=30;
	void InitializeCarriedAmmo();

	//装弹
	UPROPERTY(ReplicatedUsing=OnRep_CombatState)
	ECombatState CombatState=ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();
	
public:
	void EquipWeapon( AWeaponBaseActor* WeaponToEquip);
	
protected:

private:
		
};
