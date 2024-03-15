// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	CollisionBox=CreateDefaultSubobject<UBoxComponent>(TEXT("碰撞盒组件"));
	SetRootComponent(CollisionBox);
	//下面的碰撞设置
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);//这个碰撞形式？
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block);

	//创建运动组件
	ProjectileMovementComponent=CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("运动组件"));
	ProjectileMovementComponent->bRotationFollowsVelocity=true;
	
}
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

