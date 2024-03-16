// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameLevel1GameMode.generated.h"

/**
 * 
 */
UCLASS()
class GRADUATIONDESIGN_API AGameLevel1GameMode : public AGameMode
{
	GENERATED_BODY()
public:
	//通知淘汰
	virtual void PlayerEliminated(class APlayerCharacter*ElimmedCharacter,class AMyPlayerController* victimController,AMyPlayerController* AttackerController);
	//通知重生
	virtual void RequestRespawn(ACharacter* ElimmedCharacter,AController*ElimmedController);
};
