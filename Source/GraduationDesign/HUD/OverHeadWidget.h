// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverHeadWidget.generated.h"

/**
 * 
 */
UCLASS()
class GRADUATIONDESIGN_API UOverHeadWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* DisplayText;

	void SetDisplayText(FString Text);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole(APawn* Pawn);

protected:
	virtual void NativeDestruct() override;
};
