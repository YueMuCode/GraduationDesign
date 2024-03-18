// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GraduationDesign/HUD/PlayerHUD.h"
#include "MyPlayerController.generated.h"


/**
 * 
 */
UCLASS()
class GRADUATIONDESIGN_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	//血条数据
	void SetHUDHealth(float Health,float MaxHealth);
	//设置分数
	void SetHUDScore(float Score);
	void SetHUDDeafeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDMatchCountdown(float CountDownTime);
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual float GetServerTime();
	virtual void ReceivedPlayer() override;
	

	void OnMatchStateSet(FName State);
	void HandleMatchHasStarted();
	
protected:
	virtual  void BeginPlay() override;
	void SetHUDTime();
	
	//同步客户端和服务器的时
	UFUNCTION(Server,Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	UFUNCTION(Client,Reliable)
	void ClientReportServerTime(float TimeOfClientRequest,float TimeServerReceivedClientRequest);

	float ClientServerDelta=0.f;

	UPROPERTY(EditAnywhere,Category=Time)
	float TimeSyncFrequency=5.f;

	float TimeSyncRunningTime=0.f;
	void CheckTimeSync(float DeltaTime);
	
	//玩家血量
	UPROPERTY()
	APlayerHUD* PlayerHUD;
	
	float MatchTime=120.f;
	uint32 CountdownInt=0;

	
	void PollInit();
private:
	
	//匹配状态
	UPROPERTY(ReplicatedUsing=OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	class UCharacterOverlayWidget* CharacterOverlayWidget;
	bool bInitializeCharacterOverlay=false;

	float HUDHealth;
	float HUDMaxHealth;
	float HUDScore;
	int32 HUDDEfeats;

};
