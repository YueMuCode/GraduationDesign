// Fill out your copyright notice in the Description page of Project Settings.


#include "Casing.h"

// Sets default values
ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = false;
	//创还能子弹壳的静态网格体
	CasingMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("子弹壳"));
	SetRootComponent(CasingMesh);
}
void ACasing::BeginPlay()
{
	Super::BeginPlay();
	
}
void ACasing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

