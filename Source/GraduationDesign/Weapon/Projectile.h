// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystemComponent.h"
#include "Projectile.generated.h"

UCLASS()
class GRADUATIONDESIGN_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
protected:
	virtual void BeginPlay() override;

	//子弹命中后事件的处理
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent*HitComp,AActor*OtherActor,UPrimitiveComponent*OtherComp,FVector NormalImpulse,const FHitResult& Hit);

	//子弹的伤害
	UPROPERTY(EditAnywhere)
	float Damage=20.f;

private:
	//碰撞盒
	UPROPERTY(EditAnywhere)
	class UBoxComponent*CollisionBox;
	
	//子弹运动组件
	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	//子弹尾迹粒子
	UPROPERTY(EditAnywhere)
	UParticleSystem* Tracer;

	UParticleSystemComponent* TracerComponent;

	//子弹命中粒子特效、音效
	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticles;
	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;

	
};
