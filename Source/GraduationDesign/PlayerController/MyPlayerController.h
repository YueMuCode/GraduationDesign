// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GraduationDesign/HUD/PlayerHUD.h"
#include "MyPlayerController.generated.h"


/**
 * 
 */
UCLASS()
class GRADUATIONDESIGN_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	//血条数据
	void SetHUDHealth(float Health,float MaxHealth);
	//设置分数
	void SetHUDScore(float Score);
	void SetHUDDeafeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDMatchCountdown(float CountDownTime);
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;
protected:
	virtual  void BeginPlay() override;
	void SetHUDTime();
	//玩家血量
	UPROPERTY()
	APlayerHUD* PlayerHUD;
	
	float MatchTime=120.f;
	uint32 CountdownInt=0;
private:
	
	
};
