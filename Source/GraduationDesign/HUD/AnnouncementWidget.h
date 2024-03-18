// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AnnouncementWidget.generated.h"

/**
 * 
 */
UCLASS()
class GRADUATIONDESIGN_API UAnnouncementWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock*WarmupTime;
	UPROPERTY(meta=(BindWidget))
	UTextBlock*InfoText;
	UPROPERTY(meta=(BindWidget))
	UTextBlock*AnnouncementText;
};
