// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBaseActor.h"

// Sets default values
AWeaponBaseActor::AWeaponBaseActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	//SetReplicateMovement(true);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("武器网格体"));
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);//不开启碰撞

	// EnableCustomDepth(true);
	// WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	// WeaponMesh->MarkRenderStateDirty();


	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("碰撞区域"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);//不开启碰撞

	// PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	// PickupWidget->SetupAttachment(RootComponent);

}

void AWeaponBaseActor::BeginPlay()
{
	Super::BeginPlay();
	if(HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//在服务器上启用
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECR_Overlap);//在服务器上启用
	}
	
}

void AWeaponBaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

