// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PlayerPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class GRADUATIONDESIGN_API APlayerPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	//计算游戏分数
	virtual void OnRep_Score() override;
	void AddToScore(float ScoreAmount);

private:
	class APlayerCharacter* Character;
	class AMyPlayerController* Controller;
};
