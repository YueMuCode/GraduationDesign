// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GraduationDesign/Character/PlayerCharacter.h"
#include "GraduationDesign/Weapon/WeaponBaseActor.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#define TRACE_LENGTH 80000;
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	if(Character)
	{
		BaseWalkSpeed=Character->GetCharacterMovement()->MaxWalkSpeed;
		AimWalkSpeed=BaseWalkSpeed/2.f;
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//发出射线
	FHitResult HitResult;
	TraceUnderCrosshairs(HitResult);

}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	bAiming=bIsAiming;
	ServerSetAiming(bIsAiming);//如果是客户端执行到这里，就会触发从客户端调用的结果。
	if(Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed=bIsAiming?AimWalkSpeed:BaseWalkSpeed;
	}
}


void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming=bIsAiming;//
	if(Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed=bIsAiming?AimWalkSpeed:BaseWalkSpeed;
	}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if(EquippedWeapon&&Character)//客户端实现
	{
		Character->GetCharacterMovement()->bOrientRotationToMovement=false;
		Character->bUseControllerRotationYaw=true;
	}
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed=bPressed;
	if(bFireButtonPressed)
	{
		ServerFire();
	}
	
}

//射击射线
void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	//获取视口的大小
	FVector2D ViewportSize;
	if(GEngine&&GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	//获取视口屏幕的中心
	FVector2D CrosshairLocation(ViewportSize.X/2.f,ViewportSize.Y/2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	//一下以下一大堆数学计算
	bool bScreenToWorld=  UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this,0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if(bScreenToWorld)
	{
		FVector Start=CrosshairWorldPosition;
		FVector End=Start+CrosshairWorldDirection*TRACE_LENGTH;
	
		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECC_Visibility
		);
		//没有碰撞到东西
		if(!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint=End;
			HitTarget=End;
		}
		else
		{
			HitTarget=TraceHitResult.ImpactPoint;
			DrawDebugSphere(GetWorld(),TraceHitResult.ImpactPoint,12.f,12,FColor::Red);
		}
	}
}

//在服务器上实现多播RPC
void UCombatComponent::MulticastFire_Implementation()
{
	if(EquippedWeapon==nullptr)return;
	if(Character)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(HitTarget);//播放武器的开火动画
	}
}

//实现开火动画RPC
void UCombatComponent::ServerFire_Implementation()
{
	MulticastFire();
}



void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCombatComponent,EquippedWeapon);//EquippedWeapon进行复制
	DOREPLIFETIME(UCombatComponent,bAiming);
}

void UCombatComponent::EquipWeapon(AWeaponBaseActor* WeaponToEquip)
{
	if(Character==nullptr||WeaponToEquip==nullptr)return;
	EquippedWeapon=WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);//设置武器的状态

	//将武器附着在人物的骨骼上
	const USkeletalMeshSocket*HandSocket= Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if(HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon,Character->GetMesh());
	}
	EquippedWeapon->SetOwner(Character);

	//当持枪的时候，需要将视角锁定,并且跟随控制器旋转
	Character->GetCharacterMovement()->bOrientRotationToMovement=false;
	Character->bUseControllerRotationYaw=true;
}
