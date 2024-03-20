#pragma once
UENUM(BlueprintType)
enum class EWeaponType:uint8
{
	EWT_AssaulRifle UMETA(DisplayName="Assault Rifle"),
	EWT_RocketLauncher UMETA(DisplayName="Assault Rocket Launcher"),
	EWT_MAX UMETA(DisplayName="DefaultMAX")
};