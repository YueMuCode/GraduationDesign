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
protected:
	virtual  void BeginPlay() override;
	//玩家血量
	APlayerHUD* PlayerHUD;
	
};
