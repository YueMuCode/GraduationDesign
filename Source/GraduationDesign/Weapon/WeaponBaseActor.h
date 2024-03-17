// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponTypes.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "WeaponBaseActor.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_EquippedSecondary UMETA(DisplayName = "Equipped Secondary"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),
	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class GRADUATIONDESIGN_API AWeaponBaseActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponBaseActor();
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;//系统提供的函数，指定对应的变量进行复制
	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);//当碰撞区域发生碰撞时

	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
public:	
	virtual void Tick(float DeltaTime) override;
	void ShowPickUpWidget(bool bShowWidget);
	void SetWeaponState(EWeaponState State);//内联函数它将函数的代码插入到调用函数的地方，而不是跳转到函数的地址执行。
	FORCEINLINE USphereComponent* GetAreaSphere() const {return AreaSphere;}
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh()const {return WeaponMesh;}
	//武器开火
	virtual void Fire(const FVector& HitTarget);

	//绘制准心
	UPROPERTY(EditAnywhere,Category="Crosshairs")
	UTexture2D* CrosshairCenter;

	UPROPERTY(EditAnywhere,Category="Crosshairs")
	UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere,Category="Crosshairs")
	UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere,Category="Crosshairs")
	UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere,Category="Crosshairs")
	UTexture2D* CrosshairsBottom;

	//瞄准视野放大
	UPROPERTY(EditAnywhere)
	float ZoomedFOV=30.f;
	UPROPERTY(EditAnywhere)
	float ZoomInterSpeed=20.f;
	FORCEINLINE float GetZoomedFOV() const {return ZoomedFOV;}
	FORCEINLINE float GetZoomInterSpeed() const {return ZoomInterSpeed;}
	//自动开火
	UPROPERTY(EditAnywhere,Category=Combat)
	float FireDelay=.15f;
	UPROPERTY(EditAnywhere,Category=Combat)
	bool bAutomatic=true;
	//玩家死亡之后掉落武器
	void Dropped();

	//子弹数量
	virtual void OnRep_Owner() override;
	void SetHUDAmmo();

	//处理子弹小于零的情况
	bool IsEmpty();

	//子弹容量
	FORCEINLINE EWeaponType GetWeaponType()const {return WeaponType;}
	FORCEINLINE int32 GetAmmo()const {return Ammo;}
	FORCEINLINE int32 GetMagCapacity()const {return MagCapacity;}
	void AddAmmo(int32 AmmoToAdd);

	//捡起武器音效
	UPROPERTY(EditAnywhere)
	USoundCue* EquipSoundCue;
protected:

private:

	UPROPERTY(VisibleAnywhere,Category="武器组件")
	USkeletalMeshComponent* WeaponMesh;//武器的网格体SKM
	
	UPROPERTY(VisibleAnywhere,Category="武器组件")
	USphereComponent* AreaSphere;//这把武器的碰撞区域，用与拾取等
	
	UPROPERTY(ReplicatedUsing=OnRep_WeaponState,VisibleAnywhere,Category="武器组件")
	EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere,Category="武器组件")
	UStaticMeshComponent*WeaponStaticMesh;
	
	 UPROPERTY(VisibleAnywhere,Category="武器组件")
	 class UWidgetComponent* PickUpWidget;

	UFUNCTION()
	void OnRep_WeaponState();

	//武器开火
	UPROPERTY(EditAnywhere,Category="武器组件")
	UAnimationAsset* FireAnimation;//?

	//子弹壳
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACasing> CasingClass;

	//这把武器的子弹容量
	UPROPERTY(EditAnywhere,ReplicatedUsing=OnRep_Ammo)
	int32 Ammo;
	UFUNCTION()
	void OnRep_Ammo();

	void SpednRound();
	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

	UPROPERTY()
	class APlayerCharacter* PlayerOwnerCharacter;
	UPROPERTY()
	class AMyPlayerController* MyPlayerOwnerController;
//设置子弹容量
	UPROPERTY(EditAnywhere)
	EWeaponType	WeaponType;
};
