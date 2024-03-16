// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
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

