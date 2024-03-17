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
	virtual void OnRep_Score() override;//内置
	UFUNCTION()
	virtual void OnRep_Defeats();//自己实现
	void AddToScore(float ScoreAmount);
	void AddtoDefeats(int32 DefeatsAmount);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY()
	class APlayerCharacter* Character;
	UPROPERTY()
	class AMyPlayerController* Controller;

	//死亡次数
	UPROPERTY(ReplicatedUsing=OnRep_Defeats);
	int32 Defeats;

	
};
