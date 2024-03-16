// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

/**
 * 
 */
//绘制武器准心
USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()

public:
	UTexture2D* CrosshairsCenter;
	UTexture2D* CrosshairsLeft;
	UTexture2D* CrosshairsRight;
	UTexture2D* CrosshairsTop;
	UTexture2D* CrosshairsBottom;

	//准心抖动效果
	float CrosshairSpread;
	//武器准心颜色
	FLinearColor CrosshairsColor;
};

UCLASS()
class GRADUATIONDESIGN_API APlayerHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void DrawHUD() override;

	FORCEINLINE void SetHUDPackage(const FHUDPackage&Package){HUDPackage=Package;}

	//玩家血量
	class UCharacterOverlayWidget* CharacterOverlay;
	UPROPERTY(EditAnywhere,Category="Player Stats")
	TSubclassOf< UUserWidget> CharacterOverlayClass;

protected:
	virtual void BeginPlay() override;
	void AddCharacterOverlay();
private:
	//绘制武器准心
	FHUDPackage HUDPackage;

	void DrawCrosshair(UTexture2D* Texture,FVector2D ViewportCenter,FVector2D Spread,FLinearColor CrosshairsColor);

	//准心抖动参数
	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax=16.f;
};
