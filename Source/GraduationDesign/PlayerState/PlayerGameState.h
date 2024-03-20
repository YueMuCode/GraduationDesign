// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "PlayerGameState.generated.h"

/**
 * 
 */
UCLASS()
class GRADUATIONDESIGN_API APlayerGameState : public AGameState
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(class APlayerPlayerState* ScoringPlayer);

	UPROPERTY(Replicated)
	TArray<APlayerPlayerState*> TopScoringPlayers;
private:

	float TopScore = 0.f;
	
};
