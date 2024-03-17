// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPlayerState.h"

#include "GraduationDesign/Character/PlayerCharacter.h"
#include "Net/UnrealNetwork.h"

void APlayerPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character=Character==nullptr?Cast<APlayerCharacter>(GetPawn()):Character;
	if(Character)
	{
		Controller=Controller==nullptr?Cast<AMyPlayerController>(Character->Controller):Controller;
		if(Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}



//计算游戏分数
void APlayerPlayerState::AddToScore(float ScoreAmount)
{
	//Score+=ScoreAmount;//这个Score是state类内置的可复制变量
	SetScore(ScoreAmount+GetScore());//这个Score和SetScore是state类内置的可复制变量
	Character=Character==nullptr?Cast<APlayerCharacter>(GetPawn()):Character;
	if(Character)
	{
		Controller=Controller==nullptr?Cast<AMyPlayerController>(Character->Controller):Controller;
		if(Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}

void APlayerPlayerState::AddtoDefeats(int32 DefeatsAmount)
{
	Defeats+=DefeatsAmount;
	Character=Character==nullptr?Cast<APlayerCharacter>(GetPawn()):Character;
	if(Character)
	{
		Controller=Controller==nullptr?Cast<AMyPlayerController>(Character->Controller):Controller;
		if(Controller)
		{
			Controller->SetHUDDeafeats(Defeats);
		}
	}
}
void APlayerPlayerState::OnRep_Defeats()
{
	Character=Character==nullptr?Cast<APlayerCharacter>(GetPawn()):Character;
	if(Character)
	{
		Controller=Controller==nullptr?Cast<AMyPlayerController>(Character->Controller):Controller;
		if(Controller)
		{
			Controller->SetHUDDeafeats(Defeats);
		}
	}
}
void APlayerPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayerPlayerState,Defeats);
	
}
