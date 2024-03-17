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
	virtual void OnPossess(APawn* InPawn) override;
protected:
	virtual  void BeginPlay() override;
	//玩家血量
	UPROPERTY()
	APlayerHUD* PlayerHUD;
	
};
