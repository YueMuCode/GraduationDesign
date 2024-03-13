// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GraduationDesign/HUD/OverHeadWidget.h"
#include "Net/UnrealNetwork.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraArm=CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraArm->SetupAttachment(GetMesh());//弹簧臂附着在Character的网格体上
	CameraArm->TargetArmLength=600.f;//弹簧臂的长度，在蓝图中可以修改
	CameraArm->bUsePawnControlRotation=true;//鼠标输入可以影响pawn的旋转

	FollowCamera=CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	FollowCamera->SetupAttachment(CameraArm,USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation=false;

	bUseControllerRotationYaw=false;//角色不跟着控制器转动
	GetCharacterMovement()->bOrientRotationToMovement=true;

	
	OverheadWidget=CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	//战斗组件
	CombatComponent=CreateDefaultSubobject<UCombatComponent>(TEXT("战斗组件"));
	CombatComponent->SetIsReplicated(true);//可以进行复制
	
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//玩家输入组件，绑定玩家的输入映射
	PlayerInputComponent->BindAxis("MoveForward",this,&ThisClass::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",this,&ThisClass::MoveRight);
	PlayerInputComponent->BindAxis("Turn",this,&ThisClass::Turn);
	PlayerInputComponent->BindAxis("LookUp",this,&ThisClass::LookUp);
	
	PlayerInputComponent->BindAction("Jump",IE_Pressed,this,&ACharacter::Jump);
	PlayerInputComponent->BindAction("Equip",IE_Pressed,this,&ThisClass::EquipButtonPressed);
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(APlayerCharacter,OverlapWeapon,COND_OwnerOnly);//对OverlapWeapon进行复制，类型为拥有该对象的客户端（通常是服务器）才会复制这个属性
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(CombatComponent)
	{
		CombatComponent->Character=this;
	}
}

void APlayerCharacter::MoveForward(float value)
{
	if(Controller!=nullptr&&value!=0.f)
	{
		const FRotator YawRotation(0.f,Controller->GetControlRotation().Yaw,0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction,value);
	}
}
void APlayerCharacter::MoveRight(float value)
{
	if(Controller!=nullptr&&value!=0.f)
	{
		const FRotator YawRotation(0.f,Controller->GetControlRotation().Yaw,0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction,value);
	}
}
void APlayerCharacter::Turn(float value)
{
	AddControllerYawInput(value);
}
void APlayerCharacter::LookUp(float value)
{
	AddControllerPitchInput(value);
}

void APlayerCharacter::EquipButtonPressed()
{
	//
	if(CombatComponent)
	{
		if(HasAuthority())//如果是服务器正在调用
		{
			CombatComponent->EquipWeapon(OverlapWeapon);
		}
		else//如果不是则客户端发起RPC
		{
			ServerEquipButtonPressed();
		}
	}
}

void APlayerCharacter::SetOverlapWeapon(AWeaponBaseActor* Weapon)
{
	if(OverlapWeapon)//如果再次运行到这，意味着此时正在走出碰撞区域
	{
		OverlapWeapon->ShowPickUpWidget(false);
	}
	
	OverlapWeapon=Weapon;
	if(IsLocallyControlled())//当为服务器操作时
	{
		if(OverlapWeapon)
		{
			OverlapWeapon->ShowPickUpWidget(true);
		}
	} 
}

void APlayerCharacter::OnRep_OverLapWeapon(AWeaponBaseActor* LastWeapon)//当客户端进入到碰撞区域，触发属性复制，下面的函数将会执行
{
	if(OverlapWeapon)
	{
		OverlapWeapon->ShowPickUpWidget(true);//进入
	}
	if(LastWeapon)
	{
		LastWeapon->ShowPickUpWidget(false);//离开
	}

}

void APlayerCharacter::ServerEquipButtonPressed_Implementation()
{
	CombatComponent->EquipWeapon(OverlapWeapon);
}

