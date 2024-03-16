// Fill out your copyright notice in the Description page of Project Settings.


#include "GameLevel1GameMode.h"

#include "GraduationDesign/Character/PlayerCharacter.h"

void AGameLevel1GameMode::PlayerEliminated(APlayerCharacter* ElimmedCharacter, AMyPlayerController* victimController,
                                           AMyPlayerController* AttackerController)
{
	if(ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
	}
}
