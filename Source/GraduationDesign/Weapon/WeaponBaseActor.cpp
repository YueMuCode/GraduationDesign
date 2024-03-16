// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBaseActor.h"

#include "Casing.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GraduationDesign/Character/PlayerCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AWeaponBaseActor::AWeaponBaseActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;//表示这个Acotr可以进行复制，这是必要的条件
	//SetReplicateMovement(true);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("武器骨骼网格体"));
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

	 PickUpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("拾取提示UI"));
	 PickUpWidget->SetupAttachment(RootComponent);
	
	WeaponStaticMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("武器静态网格体"));
	WeaponStaticMesh->SetupAttachment(RootComponent);
	WeaponStaticMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);//这一步可能会使后续的武器伤害检测发生问题
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);//不开启碰撞

}

void AWeaponBaseActor::BeginPlay()
{
	Super::BeginPlay();
	if(HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//在服务器上启用
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECR_Overlap);//在服务器上启用
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this,&ThisClass::OnSphereOverlap);//当发生碰撞事件时
		AreaSphere->OnComponentEndOverlap.AddDynamic(this,&ThisClass::OnSphereEndOverlap);//当走出碰撞区域时
	}
	if(PickUpWidget)
	{
		PickUpWidget->SetVisibility(false);
	}
}

void AWeaponBaseActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeaponBaseActor,WeaponState);
}

void AWeaponBaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeaponBaseActor::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* PlayerCharacter=Cast<APlayerCharacter>(OtherActor);
	if(PlayerCharacter)
	{
		PlayerCharacter->SetOverlapWeapon(this);//传送给PlayerCharacter进行属性的复制，属性的复制发生在PlayerCharacter
	}
}

void AWeaponBaseActor::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerCharacter* PlayerCharacter=Cast<APlayerCharacter>(OtherActor);
	if(PlayerCharacter)
	{
		PlayerCharacter->SetOverlapWeapon(nullptr);
	}
}

void AWeaponBaseActor::ShowPickUpWidget(bool bShowWidget)
{
	if(PickUpWidget)
	{
		PickUpWidget->SetVisibility(bShowWidget);
	}
}

void AWeaponBaseActor::SetWeaponState(EWeaponState State)
{
	WeaponState=State;
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickUpWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);//在服务器上执行
		//捡起掉落的武器需要重置状态
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_Dropped:
		if(HasAuthority())//在服务器
		{
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
}



void AWeaponBaseActor::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickUpWidget(false);
		//捡起掉落的武器需要重置状态
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_Dropped:
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
}
//武器开火动画
void AWeaponBaseActor::Fire(const FVector& HitTarget)
{
	if(FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation,false);
	}
	//生成子弹壳
	if(CasingClass)
	{
		const USkeletalMeshSocket* AmmoEjectSocket =WeaponMesh->GetSocketByName(FName("AmmoEject"));
		if(AmmoEjectSocket)
		{
			FTransform SocketTransform=AmmoEjectSocket->GetSocketTransform(WeaponMesh);
			UWorld* World=GetWorld();
			if(World)
			{
				World->SpawnActor<ACasing>(CasingClass,SocketTransform.GetLocation(),SocketTransform.GetRotation().Rotator());
			}
		}
	}
}
//武器掉落
void AWeaponBaseActor::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);
	//武器分离
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld,true);
	WeaponMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	
}

