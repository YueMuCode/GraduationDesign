// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
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
public:	
	virtual void Tick(float DeltaTime) override;


public:

protected:

private:

	UPROPERTY(VisibleAnywhere,Category="武器组件")
	USkeletalMeshComponent* WeaponMesh;//武器的网格体SKM
	UPROPERTY(VisibleAnywhere,Category="武器组件")
	USphereComponent* AreaSphere;//这把武器的碰撞区域，用与拾取等
	UPROPERTY(VisibleAnywhere)
	EWeaponState WeaponState;

};
