// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlayWidget.generated.h"

/**
 * 
 */
UCLASS()
class GRADUATIONDESIGN_API UCharacterOverlayWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(meta=(BindWidget))
	class UProgressBar*HealthBar;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock*HealthText;
	//分数UI
	UPROPERTY(meta=(BindWidget))
	class UTextBlock*ScoreText;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock*ScoreAmount;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock*DefeatsAmount;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock*WeaponAmmoAmount;
};
