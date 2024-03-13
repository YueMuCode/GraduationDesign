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
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UCombatComponent();
	friend class APlayerCharacter;//友类的成员函数就可以访问另一个类(APlayerCharacter)的私有成员和受保护成员
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	 APlayerCharacter* Character;
	class AWeaponBaseActor* EquippedWeapon;


public:
	void EquipWeapon( AWeaponBaseActor* WeaponToEquip);
	
protected:

private:
		
};
