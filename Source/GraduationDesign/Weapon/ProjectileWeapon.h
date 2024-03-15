// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBaseActor.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class GRADUATIONDESIGN_API AProjectileWeapon : public AWeaponBaseActor
{
	GENERATED_BODY()
	//开火
public:
	virtual void Fire(const FVector& HitTarget) override;
private:
	//生成子弹类
	UPROPERTY(EditAnywhere)
	TSubclassOf< class AProjectile> ProjectileClass;
};
