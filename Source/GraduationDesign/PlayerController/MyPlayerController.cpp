// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameMode.h"
#include "GraduationDesign/Character/PlayerCharacter.h"
#include "GraduationDesign/HUD/CharacterOverlayWidget.h"
#include "GraduationDesign/HUD/PlayerHUD.h"
#include "Net/UnrealNetwork.h"

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//玩家血量
	GetHUD();
	PlayerHUD=Cast<APlayerHUD>(GetHUD());
}

void AMyPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyPlayerController,MatchState);
}

void AMyPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	SetHUDTime();
	CheckTimeSync(DeltaSeconds);
	PollInit();
}



void AMyPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	PlayerHUD=PlayerHUD==nullptr?Cast<APlayerHUD>(GetHUD()):PlayerHUD;
	bool bHUDValid=PlayerHUD&&
		PlayerHUD->CharacterOverlay&&
			PlayerHUD->CharacterOverlay->HealthBar&&
				PlayerHUD->CharacterOverlay->HealthText;
	
	if(bHUDValid)
	{
		const float HealthPercent=Health/MaxHealth;
		PlayerHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText=FString::Printf(TEXT("%d%"),FMath::CeilToInt(Health),FMath::CeilToInt(HealthPercent));
		PlayerHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	else
	{
		bInitializeCharacterOverlay=true;
		HUDHealth=Health;
		HUDMaxHealth=MaxHealth;
	}
}

void AMyPlayerController::SetHUDScore(float Score)
{
	PlayerHUD=PlayerHUD==nullptr?Cast<APlayerHUD>(GetHUD()):PlayerHUD;
	bool bHUDValid=PlayerHUD&&PlayerHUD->CharacterOverlay&&	PlayerHUD->CharacterOverlay->ScoreAmount;
	if(bHUDValid)
	{
		FString ScoreText=FString::Printf(TEXT("%d"),FMath::FloorToInt(Score));
		PlayerHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
	else
	{
		bInitializeCharacterOverlay=true;
		HUDScore=Score;
	}
}

void AMyPlayerController::SetHUDDeafeats(int32 Defeats)
{
	PlayerHUD=PlayerHUD==nullptr?Cast<APlayerHUD>(GetHUD()):PlayerHUD;
	bool bHUDValid=PlayerHUD&&PlayerHUD->CharacterOverlay&&	PlayerHUD->CharacterOverlay->DefeatsAmount;
	if(bHUDValid)
	{
		FString DefeatsText=FString::Printf(TEXT("%d"),Defeats);
		PlayerHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
	else
	{
		bInitializeCharacterOverlay=true;
		HUDDEfeats=Defeats;
	}
}

void AMyPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	PlayerHUD=PlayerHUD==nullptr?Cast<APlayerHUD>(GetHUD()):PlayerHUD;
	bool bHUDValid=PlayerHUD&&PlayerHUD->CharacterOverlay&&	PlayerHUD->CharacterOverlay->WeaponAmmoAmount;
	if(bHUDValid)
	{
		FString AmmoText=FString::Printf(TEXT("%d"),Ammo);
		PlayerHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void AMyPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	PlayerHUD=PlayerHUD==nullptr?Cast<APlayerHUD>(GetHUD()):PlayerHUD;
	bool bHUDValid=PlayerHUD&&PlayerHUD->CharacterOverlay&&	PlayerHUD->CharacterOverlay->CarriedAmmoAmount;
	if(bHUDValid)
	{
		FString CarriedAmmoText=FString::Printf(TEXT("%d"),Ammo);
		PlayerHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(CarriedAmmoText));
	}
}

void AMyPlayerController::SetHUDMatchCountdown(float CountDownTime)
{
	PlayerHUD=PlayerHUD==nullptr?Cast<APlayerHUD>(GetHUD()):PlayerHUD;
	bool bHUDValid=PlayerHUD&&PlayerHUD->CharacterOverlay&&	PlayerHUD->CharacterOverlay->MatchCountdownText;
	if(bHUDValid)
	{
		int32 Minutes=FMath::FloorToInt(CountDownTime/60.f);
		int32 Seconds=CountDownTime-Minutes*60;
		
		FString CountdownText=FString::Printf(TEXT("%02d:%02d"),Minutes,Seconds);
		PlayerHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void AMyPlayerController::SetHUDTime()
{
	uint32 SecondsLeft=FMath::CeilToInt(MatchTime-GetServerTime());
	if(CountdownInt!=SecondsLeft)
	{
		SetHUDMatchCountdown(MatchTime-GetServerTime());
	}
	CountdownInt=SecondsLeft;                                                                     
}

void AMyPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime+=DeltaTime;
	if(IsLocalController()&&TimeSyncRunningTime>TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime=0.f;
	}
}



//修复服务端在死亡之后血条不刷新
void AMyPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	APlayerCharacter* PlayerCharacter=Cast<APlayerCharacter>(InPawn);
	if(PlayerCharacter)
	{
		SetHUDHealth(PlayerCharacter->GetHealth(),PlayerCharacter->GetMAXHealth());
	}
}

void AMyPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt=GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest,ServerTimeOfReceipt);
}

void AMyPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest,float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float SingleTripTime = 0.5f * RoundTripTime;
	float CurrentServerTime = TimeServerReceivedClientRequest + SingleTripTime;
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
	
}

float AMyPlayerController::GetServerTime()
{
	if (HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();
	}
	else
		return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void AMyPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if(IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
	
}

void AMyPlayerController::OnMatchStateSet(FName State)
{
	MatchState=State;
	if(MatchState==MatchState::InProgress)
	{
		PlayerHUD=PlayerHUD==nullptr?Cast<APlayerHUD>(GetHUD()):PlayerHUD;
		if(PlayerHUD)
		{
			PlayerHUD->AddCharacterOverlay();//热身时间结束，就显示UI
		}
	}
}


void AMyPlayerController::OnRep_MatchState()
{
	if(MatchState==MatchState::InProgress)
	{
		PlayerHUD=PlayerHUD==nullptr?Cast<APlayerHUD>(GetHUD()):PlayerHUD;
		if(PlayerHUD)
		{
			PlayerHUD->AddCharacterOverlay();//热身时间结束，就显示UI
		}
	}
}

void AMyPlayerController::PollInit()
{
	if(CharacterOverlayWidget==nullptr)
	{
		if(PlayerHUD&&PlayerHUD->CharacterOverlay)
		{
			CharacterOverlayWidget=PlayerHUD->CharacterOverlay;
			if(CharacterOverlayWidget)
			{
				SetHUDHealth(HUDHealth,HUDMaxHealth);
				SetHUDScore(HUDScore);
				SetHUDDeafeats(HUDDEfeats);
			}
		}
	}
}