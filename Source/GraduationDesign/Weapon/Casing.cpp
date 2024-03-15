// Fill out your copyright notice in the Description page of Project Settings.


#include "Casing.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = false;
	//创还能子弹壳的静态网格体
	CasingMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("子弹壳"));
	SetRootComponent(CasingMesh);
	//添加子弹壳的物品特性
	CasingMesh->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);//忽略相机的碰撞
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);
	CasingMesh->SetNotifyRigidBodyCollision(true);
	ShellEjectionimpulse=10.f;//速度
}
void ACasing::BeginPlay()
{
	Super::BeginPlay();
	//添加子弹壳的物理特性
	CasingMesh->OnComponentHit.AddDynamic(this,&ACasing::OnHit);
	CasingMesh->AddImpulse(GetActorForwardVector()*ShellEjectionimpulse);
	
}

void ACasing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//子弹壳弹出落地之后
void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if(ShellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this,ShellSound,GetActorLocation());
	}
	Destroy();
}

