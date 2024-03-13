// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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

private:
	 APlayerCharacter* Character;

	UPROPERTY(Replicated)
	class AWeaponBaseActor* EquippedWeapon;


public:
	void EquipWeapon( AWeaponBaseActor* WeaponToEquip);
	
protected:

private:
		
};