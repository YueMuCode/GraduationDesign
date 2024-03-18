// Fill out your copyright notice in the Description page of Project Settings.


#include "GameLevel1GameMode.h"

#include "GameFramework/PlayerStart.h"
#include "GraduationDesign/Character/PlayerCharacter.h"
#include "GraduationDesign/PlayerState/PlayerPlayerState.h"
#include "Kismet/GameplayStatics.h"

AGameLevel1GameMode::AGameLevel1GameMode()
{
	bDelayedStart=true;//等待游戏开始的状态，完成会变得没有网格体，在空中飞来飞去
}


void AGameLevel1GameMode::BeginPlay()
{
	Super::BeginPlay();
	LevelStartingTime=GetWorld()->GetTimeSeconds();
}


void AGameLevel1GameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(MatchState==MatchState::WaitingToStart)
	{
		CountdownTime=WarmupTime-GetWorld()->GetTimeSeconds()+LevelStartingTime;
		if(CountdownTime<=0.f)
		{
			StartMatch();
		}
	}
}

void AGameLevel1GameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for(FConstPlayerControllerIterator It=GetWorld()->GetPlayerControllerIterator();It;It++)
	{
		AMyPlayerController*MyPlayerController=Cast<AMyPlayerController>(*It);
		if(MyPlayerController)
		{
			MyPlayerController->OnMatchStateSet(MatchState);
		}
		
	}
}



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
	if(VictimPlayerState)
	{
		VictimPlayerState->AddtoDefeats(1);
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
