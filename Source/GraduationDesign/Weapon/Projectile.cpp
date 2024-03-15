// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	//让子弹可以复制
	bReplicates=true;
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
	//生成子弹尾迹
	if(Tracer)
	{
		TracerComponent=UGameplayStatics::SpawnEmitterAttached(
			Tracer,CollisionBox,FName(),GetActorLocation(),GetActorRotation(),EAttachLocation::KeepWorldPosition);
	}
	//在服务器上处理
	if(HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this,&AProjectile::OnHit);
	}
}
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



//子弹命中结果处理
void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	//摧毁
	Destroy();//destroy函数底层内置了网络通信，可以直接使用。最终会在底层调用到destroyed
}
void AProjectile::Destroyed()
{
	Super::Destroyed();
	if(ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ImpactParticles,GetActorTransform());
	}
	if(ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this,ImpactSound,GetActorLocation());
	}
}


