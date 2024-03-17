// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GraduationDesign/Character/PlayerCharacter.h"
#include "GraduationDesign/HUD/CharacterOverlayWidget.h"
#include "GraduationDesign/HUD/PlayerHUD.h"

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//玩家血量
	GetHUD();
	PlayerHUD=Cast<APlayerHUD>(GetHUD());
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

