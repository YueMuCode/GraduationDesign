// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GraduationDesign/Character/PlayerCharacter.h"
#include "GraduationDesign/Interface/InterractWithCrosshairsInterface.h"
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

		//瞄准视角
		if(Character->GetFollowCamera())
		{
			DefaultFOV=Character->GetFollowCamera()->FieldOfView;//设置视角
			CurrentFOV=DefaultFOV;
		}
		//枪支弹药
		if(Character->HasAuthority())
		{
			InitializeCarriedAmmo();
		}
	}
	
	
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	SetHUDCrossairs(DeltaTime);

	//调账枪口对准射线
	if(Character&&Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget=HitResult.ImpactPoint;

		SetHUDCrossairs(DeltaTime);
		InterpFOV(DeltaTime);
	}

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCombatComponent,EquippedWeapon);//EquippedWeapon进行复制
	DOREPLIFETIME(UCombatComponent,bAiming);
	DOREPLIFETIME_CONDITION(UCombatComponent,CarriedAmmo,COND_OwnerOnly);
	DOREPLIFETIME(UCombatComponent,CombatState);
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
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);//设置武器的状态
		//将武器附着在人物的骨骼上
		const USkeletalMeshSocket*HandSocket= Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if(HandSocket)
		{
			HandSocket->AttachActor(EquippedWeapon,Character->GetMesh());
		}
		Character->GetCharacterMovement()->bOrientRotationToMovement=false;
		Character->bUseControllerRotationYaw=true;
	}
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed=bPressed;
	if(bFireButtonPressed&&EquippedWeapon)
	{
		Fire();
		
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

		//防止射线追踪到自身所做的处理
		if(Character)
		{
			float DistanceToCharacter=(Character->GetActorLocation()-Start).Size();
			Start+=CrosshairWorldDirection*(DistanceToCharacter+100.f);
			
		}
		
		FVector End=Start+CrosshairWorldDirection*TRACE_LENGTH;
	
		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECC_Visibility
		);
		//使用接口检测，如果碰撞到的actor实现了这个接口
		if(TraceHitResult.GetActor()&&TraceHitResult.GetActor()->Implements<UInterractWithCrosshairsInterface>() )
		{
			HUDPackage.CrosshairsColor=FLinearColor::Red;
		}
		else
		{
			HUDPackage.CrosshairsColor=FLinearColor::Green;
		}
		//没有碰撞到东西
		 if(!TraceHitResult.bBlockingHit)
		 {
		 	TraceHitResult.ImpactPoint=End;
		 	HitTarget=End;
		 }
		 else
		 {
		 	HitTarget=TraceHitResult.ImpactPoint;
			//DrawDebugSphere(GetWorld(),TraceHitResult.ImpactPoint,12.f,12,FColor::Red);
		}
	}
}

//绘制武器准心
void UCombatComponent::SetHUDCrossairs(float DeltaTime)
{
	if(Character==nullptr||Character->Controller==nullptr)return;
	Controller=Controller==nullptr?Cast<AMyPlayerController>(Character->Controller):Controller;
	if(Controller)
	{
		HUD=HUD==nullptr?Cast<APlayerHUD>(Controller->GetHUD()):HUD;
		if(HUD)
		{
			if(EquippedWeapon)
			{
				HUDPackage.CrosshairsCenter=EquippedWeapon->CrosshairCenter;
				HUDPackage.CrosshairsLeft=EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight=EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsTop=EquippedWeapon->CrosshairsTop;
				HUDPackage.CrosshairsBottom=EquippedWeapon->CrosshairsBottom;
				
			}
			else
			{
				HUDPackage.CrosshairsCenter=nullptr;
				HUDPackage.CrosshairsLeft=nullptr;
				HUDPackage.CrosshairsRight=nullptr;
				HUDPackage.CrosshairsTop=nullptr;
				HUDPackage.CrosshairsBottom=nullptr;
				
			}
			//计算屏幕抖动
			FVector2D walkSpeedRange(0.f,Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f,1.f);
			FVector Velocity=Character->GetVelocity();
			Velocity.Z=0.f;
			CrosshairVelocityFactor= FMath::GetMappedRangeValueUnclamped(walkSpeedRange,VelocityMultiplierRange,Velocity.Size());
			if(Character->GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirFactor=FMath::FInterpTo(CrosshairInAirFactor,2.25f,DeltaTime,2.25f);
			}
			else
			{
				CrosshairInAirFactor=FMath::FInterpTo(CrosshairInAirFactor,0.f,DeltaTime,30.f);
			}
			if(bAiming)
			{
				CrosshairInAimFactor=FMath::FInterpTo(CrosshairInAimFactor,0.58f,DeltaTime,30.f);
			}
			else
			{
				CrosshairInAimFactor=FMath::FInterpTo(CrosshairInAimFactor,0.f,DeltaTime,30.f);
			}

			CrosshairShootingFactor=FMath::FInterpTo(CrosshairShootingFactor,0.f,DeltaTime,20.f);
			
			HUDPackage.CrosshairSpread=CrosshairVelocityFactor+CrosshairInAirFactor-CrosshairInAimFactor+0.5f+CrosshairShootingFactor;
			HUD->SetHUDPackage(HUDPackage);
		}
	}
}



//在服务器上实现多播RPC
void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if(EquippedWeapon==nullptr)return;
	if(Character)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);//播放武器的开火动画
	}
}

//实现开火动画RPC
void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}




//瞄准视野变大
void UCombatComponent::InterpFOV(float DeltaTime)
{
	if(EquippedWeapon==nullptr)return ;
	if(bAiming)
	{
		//利用插值函数缓慢放大视角
		CurrentFOV=FMath::FInterpTo(CurrentFOV,EquippedWeapon->GetZoomedFOV(),DeltaTime,EquippedWeapon->GetZoomInterSpeed());
		
	}
	else
	{
		 CurrentFOV=FMath::FInterpTo(CurrentFOV,DefaultFOV,DeltaTime,ZoomInterSpeed);
	}
	if(Character&&Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}

//全自动开火
void UCombatComponent::StartFireTimer()
{
	if(EquippedWeapon==nullptr||Character==nullptr)return;
	Character->GetWorldTimerManager().SetTimer(
		FireTimer,
		this,
		&UCombatComponent::FireTimerFinished,
		 EquippedWeapon->FireDelay
	);
	
}
//全自动开火
void UCombatComponent::FireTimerFinished()
{
	if(EquippedWeapon==nullptr)return;
	bCanFire=true;
	if(bFireButtonPressed&&EquippedWeapon->bAutomatic)
	{
		Fire();
	}
}

void UCombatComponent::Fire()
{
	if(CanFire())
	{
		bCanFire=false;
		ServerFire(HitTarget);
		if(EquippedWeapon)
		{
			CrosshairShootingFactor=1.f;
		}
		StartFireTimer();
	}
	
}



void UCombatComponent::EquipWeapon(AWeaponBaseActor* WeaponToEquip)
{
	if(Character==nullptr||WeaponToEquip==nullptr)return;
	//如果装备了武器就不能装备同样的第二把
	if(EquippedWeapon)
	{
		EquippedWeapon->Dropped();
	}

	
	EquippedWeapon=WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);//设置武器的状态
	//将武器附着在人物的骨骼上
	const USkeletalMeshSocket*HandSocket= Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if(HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon,Character->GetMesh());
	}
	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->SetHUDAmmo();
	if(CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmo=CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	Controller=Controller==nullptr?Cast<AMyPlayerController>(Character->Controller):Controller;
	if(Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
	//当持枪的时候，需要将视角锁定,并且跟随控制器旋转
	Character->GetCharacterMovement()->bOrientRotationToMovement=false;
	Character->bUseControllerRotationYaw=true;
}

void UCombatComponent::Reload()
{
	if(CarriedAmmo>0&&CombatState!=ECombatState::ECS_Reloading)
	{
		ServerReload();
	}
}


void UCombatComponent::ServerReload_Implementation()
{
	if(Character==nullptr)return;
	CombatState=ECombatState::ECS_Reloading;
	HandleReload();
}

void UCombatComponent::FinishReloading()
{
	if(Character==nullptr)return;
	if(Character->HasAuthority())
	{
		CombatState=ECombatState::ECS_Unoccupied;
	}
}

void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
	case ECombatState::ECS_Reloading:
		HandleReload();
		break;
	}
}

void UCombatComponent::HandleReload()
{
	Character->PlayReloadMontage();
}


bool UCombatComponent::CanFire()
{
	if(EquippedWeapon==nullptr)return false;
	return !EquippedWeapon->IsEmpty()||!bCanFire;
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	Controller=Controller==nullptr?Cast<AMyPlayerController>(Character->Controller):Controller;
	if(Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
}
void UCombatComponent::InitializeCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AssaulRifle,StartingARAmmo);
}

