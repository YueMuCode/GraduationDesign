// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	int NumberofPlayer= GameState.Get()->PlayerArray.Num();
	if(NumberofPlayer==2)
	{
		UWorld* World=GetWorld();//当前关卡
		if(World)
		{
			bUseSeamlessTravel=true;//开启多人游戏时服务器转换的无缝连接状态（有中间的过渡地图）
			World->ServerTravel(FString("/Game/Maps/GameLevel1?listen"));
		}
	}

	
}

//用于测试，方便 一个人的时候也能进入游戏开始的地图
void ALobbyGameMode::BeginAlone()
{
	UWorld* World=GetWorld();//当前关卡
	if(World)
	{
		bUseSeamlessTravel=true;//开启多人游戏时服务器转换的无缝连接状态（有中间的过渡地图）
		World->ServerTravel(FString("/Game/Maps/GameLevel1?listen"));
	}
}
