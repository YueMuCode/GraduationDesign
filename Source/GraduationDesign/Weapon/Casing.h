// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

UCLASS()
class GRADUATIONDESIGN_API ACasing : public AActor
{
	GENERATED_BODY()
	
public:	
	ACasing();
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent*HitComp,AActor*OtherActor,UPrimitiveComponent*OtherComp,FVector NormalImpulse,const FHitResult& Hit);
private:
	//子弹壳的静态网格体
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent*CasingMesh;

	//子弹壳物理特性
	UPROPERTY(EditAnywhere)
	float ShellEjectionimpulse;

	//子弹壳落地声音
	UPROPERTY(EditAnywhere)
	class USoundCue* ShellSound;
};
