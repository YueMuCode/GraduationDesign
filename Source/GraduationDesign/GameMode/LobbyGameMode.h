// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GRADUATIONDESIGN_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()
	virtual void PostLogin(APlayerController* NewPlayer) override;//当玩家加入当前服务器会触发

public:
	
	UFUNCTION(BlueprintCallable)
	void BeginAlone();
};
