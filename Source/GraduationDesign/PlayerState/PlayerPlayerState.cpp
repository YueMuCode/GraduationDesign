// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPlayerState.h"

#include "GraduationDesign/Character/PlayerCharacter.h"

void APlayerPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character=Character==nullptr?Cast<APlayerCharacter>(GetPawn()):Character;
	if(Character)
	{
		Controller=Controller==nullptr?Cast<AMyPlayerController>(Character->Controller):Controller;
		if(Controller)
		{
			Controller->SetHUDScore(Score);
		}
	}
}

//计算游戏分数
void APlayerPlayerState::AddToScore(float ScoreAmount)
{
	Score+=ScoreAmount;//这个Score是state类内置的可复制变量
	Character=Character==nullptr?Cast<APlayerCharacter>(GetPawn()):Character;
	if(Character)
	{
		Controller=Controller==nullptr?Cast<AMyPlayerController>(Character->Controller):Controller;
		if(Controller)
		{
			Controller->SetHUDScore(Score);
		}
	}
}
