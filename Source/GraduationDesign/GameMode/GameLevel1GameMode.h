// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameLevel1GameMode.generated.h"


namespace MatchState
{
	extern GRADUATIONDESIGN_API const  FName Cooldown;
}
/**
 * 
 */


UCLASS()
class GRADUATIONDESIGN_API AGameLevel1GameMode : public AGameMode
{
	GENERATED_BODY()
public:
	//匹配状态
	AGameLevel1GameMode();

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime=10.f;

	UPROPERTY(EditDefaultsOnly)
	float MatchTime=120.f;

	UPROPERTY(EditDefaultsOnly)
	float CooldownTime=10.f;
	
	float LevelStartingTime=0.f;

	float CountdownTime=0.f;


	virtual void Tick(float DeltaTime) override;
	//通知淘汰
	virtual void PlayerEliminated(class APlayerCharacter*ElimmedCharacter,class AMyPlayerController* victimController,AMyPlayerController* AttackerController);
	//通知重生
	virtual void RequestRespawn(ACharacter* ElimmedCharacter,AController*ElimmedController);

	FORCEINLINE float GetCountdownTime() const {return CountdownTime;}
protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;
};
