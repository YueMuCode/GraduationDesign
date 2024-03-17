// Fill out your copyright notice in the Description page of Project Settings.


#include "GameLevel1GameMode.h"

#include "GameFramework/PlayerStart.h"
#include "GraduationDesign/Character/PlayerCharacter.h"
#include "GraduationDesign/PlayerState/PlayerPlayerState.h"
#include "Kismet/GameplayStatics.h"

void AGameLevel1GameMode::PlayerEliminated(APlayerCharacter* ElimmedCharacter, AMyPlayerController* victimController,
                                           AMyPlayerController* AttackerController)
{
	//记录攻击者的分数
	APlayerPlayerState* AttackerPlayerstate=AttackerController?Cast<APlayerPlayerState>(AttackerController->PlayerState):nullptr;
	APlayerPlayerState* VictimPlayerState=victimController?Cast<APlayerPlayerState>(victimController->PlayerState):nullptr;
	if(AttackerPlayerstate&&AttackerPlayerstate!=VictimPlayerState)
	{
		AttackerPlayerstate->AddToScore(1.f);
	}

	
	if(ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
	}
}

void AGameLevel1GameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if(ElimmedCharacter)
	{
		//先销毁玩家
		ElimmedCharacter->Reset();//?
		ElimmedCharacter->Destroy();
	}
	if(ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this,APlayerStart::StaticClass(),PlayerStarts);
		int32 Selection=FMath::RandRange(0,PlayerStarts.Num()-1);//随机选择一个复活点进行复活
		RestartPlayerAtPlayerStart(ElimmedController,PlayerStarts[Selection]);
	}
}
