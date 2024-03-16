// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GraduationDesign/GameMode/GameLevel1GameMode.h"
#include "GraduationDesign/Weapon/WeaponBaseActor.h"
#include "Kismet/KismetMathLibrary.h"
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


	GetCharacterMovement()->NavAgentProps.bCanCrouch=true;//开启了运动系统中的蹲伏功能

	//解决相机与人物碰撞的问题
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
	GetMesh()->SetCollisionObjectType(ECC_EngineTraceChannel1);
	//原地转身
	TurningInPlace=ETurningInPlace::ETIP_NotTurning;

	//设置网络的更新评率
	NetUpdateFrequency=66.f;
	MinNetUpdateFrequency=33.f;

	//转身速度
	//GetCharacterMovement()->RotationRate=FRotator(0.f,0.f,850.f);

	//创建Timeline组件
	DissolveTimeline=CreateDefaultSubobject<UTimelineComponent>("Timeline组件");
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	UpdateHUDHealth();

	if(HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this,&APlayerCharacter::ReceiveDamage);
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(GetLocalRole()>ROLE_SimulatedProxy&&IsLocallyViewed())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication+=DeltaTime;
		if(TimeSinceLastMovementReplication>0.25f)
		{
			OnRep_ReplicatedMovement(); 
		}
		CalculateAO_Pitch();
	}
	HideCameraIfCharacterClose();
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//玩家输入组件，绑定玩家的输入映射
	PlayerInputComponent->BindAxis("MoveForward",this,&ThisClass::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",this,&ThisClass::MoveRight);
	PlayerInputComponent->BindAxis("Turn",this,&ThisClass::Turn);
	PlayerInputComponent->BindAxis("LookUp",this,&ThisClass::LookUp);
	
	PlayerInputComponent->BindAction("Jump",IE_Pressed,this,&APlayerCharacter::Jump);
	PlayerInputComponent->BindAction("Equip",IE_Pressed,this,&ThisClass::EquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch",IE_Pressed,this,&ThisClass::CrouchButtonPressed);

	PlayerInputComponent->BindAction("Aim",IE_Pressed,this,&ThisClass::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim",IE_Released,this,&ThisClass::AimButtonReleased);

	PlayerInputComponent->BindAction("Fire",IE_Pressed,this,&ThisClass::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire",IE_Released,this,&ThisClass::FireButtonReleased);
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(APlayerCharacter,OverlapWeapon,COND_OwnerOnly);//对OverlapWeapon进行复制，类型为拥有该对象的客户端（通常是服务器）才会复制这个属性
	DOREPLIFETIME(APlayerCharacter,Health);
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(CombatComponent)
	{
		CombatComponent->Character=this;
	}
}

AWeaponBaseActor* APlayerCharacter::GetEquippedWeapon()
{
	if(CombatComponent==nullptr)return nullptr;
	return CombatComponent->EquippedWeapon;
}

//播放蒙太奇片段
void APlayerCharacter::PlayFireMontage(bool bAiming)
{
	if(CombatComponent==nullptr||CombatComponent->EquippedWeapon==nullptr)return;

	UAnimInstance* AnimInstance=GetMesh()->GetAnimInstance();
	if(AnimInstance&&FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName=bAiming?FName("RifleAim"):FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void APlayerCharacter::PlayHitReactMontage()
{
	if(CombatComponent==nullptr||CombatComponent->EquippedWeapon==nullptr)return;

	UAnimInstance* AnimInstance=GetMesh()->GetAnimInstance();
	if(AnimInstance&&HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("FromFront");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void APlayerCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance=GetMesh()->GetAnimInstance();
	if(AnimInstance&&ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);
	}
}
void APlayerCharacter::MulticastElim_Implementation()
{
	bElimmed=true;
	PlayElimMontage();
	//如果设置了材质，在这里创建动态材质
	if(DissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance=UMaterialInstanceDynamic::Create(DissolveMaterialInstance,this);
		GetMesh()->SetMaterial(0,DynamicDissolveMaterialInstance);//修改mesh的材质
		GetMesh()->SetMaterial(1,DynamicDissolveMaterialInstance);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"),0.55f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Glow"),200.f);
	}
	//开始动态修改材质的参数
	StartDissolve();
}

FVector APlayerCharacter::GetHitTarget() const
{
	if(CombatComponent==nullptr)return FVector();
	return CombatComponent->HitTarget;
}

void APlayerCharacter::CalculateAO_Pitch()
{
	AO_Pitch=GetBaseAimRotation().Pitch;
	if(AO_Pitch>90.f&&!IsLocallyControlled())
	{
		FVector2D InRange(270.f,360.f);
		FVector2D OutRange(-90,0.f);
		AO_Pitch=FMath::GetMappedRangeValueClamped(InRange,OutRange,AO_Pitch);
	}
}

void APlayerCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	TimeSinceLastMovementReplication=0.f;
}

void APlayerCharacter::Elim()
{
	MulticastElim();
	GetWorldTimerManager().SetTimer(
		ElimTimer,
		this,
		&APlayerCharacter::ElimTimerFinished,
		ElimDelay
	);
}


void APlayerCharacter::ElimTimerFinished()
{
	AGameLevel1GameMode* GameLevel1GameMode=GetWorld()->GetAuthGameMode<AGameLevel1GameMode>();
	if(GameLevel1GameMode)
	{
		GameLevel1GameMode->RequestRespawn(this,Controller);
	}
}

void APlayerCharacter::UpdateDissolveeMaterial(float DissolveValue)
{
	if(DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"),DissolveValue);
	//	DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Grow"),0.55f);
	}
}

//当播放轨道，就回调UpdateDissolveeMaterial，利用曲线插值慢慢实现值的变化效果
void APlayerCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this,&APlayerCharacter::APlayerCharacter::UpdateDissolveeMaterial);
	if(DissolveCurve&&DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve,DissolveTrack);
		DissolveTimeline->Play();
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
void APlayerCharacter::CrouchButtonPressed()
{
	if(bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();//调用系统的蹲伏系统
	}
	
}
void APlayerCharacter::AimButtonPressed()
{
	if(CombatComponent)
	{
		CombatComponent->SetAiming(true);
	}
}
void APlayerCharacter::AimButtonReleased()
{
	if(CombatComponent)
	{
		CombatComponent->SetAiming(false);
	}
}
void APlayerCharacter::Jump()
{

	if(bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}
void APlayerCharacter::FireButtonPressed()
{
	if(CombatComponent)
	{
		CombatComponent->FireButtonPressed(true);
	}
}
void APlayerCharacter::FireButtonReleased()
{
	if(CombatComponent)
	{
		CombatComponent->FireButtonPressed(false);
	}
}


void APlayerCharacter::AimOffset(float DeltaTime)
{
	if(CombatComponent&&CombatComponent->EquippedWeapon==nullptr)return;
	FVector Velocity=GetVelocity();
	Velocity.Z=0.f;
	float Speed=Velocity.Size();
	bool bIsAir=GetCharacterMovement()->IsFalling();

	if(Speed==0.f&&!bIsAir)
	{
		bRotateRootBone=true;//代理旋转平滑参数
		FRotator CurrentAimRotation=FRotator(0.f,GetBaseAimRotation().Yaw,0.f);
		FRotator DeltaAimRotation=UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation,StartingAimRotation);
		AO_Yaw=DeltaAimRotation.Yaw;
		if(TurningInPlace==ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw=AO_Yaw;
		}
		bUseControllerRotationYaw=true;
		TurnInPlace(DeltaTime);
	}
	if(Speed>0.f||bIsAir)
	{
		bRotateRootBone=false;//代理旋转平滑参数
		StartingAimRotation=FRotator(0.f,GetBaseAimRotation().Yaw,0.f);
		AO_Yaw=0.f;
		bUseControllerRotationYaw=true;
		TurningInPlace=ETurningInPlace::ETIP_NotTurning;
	}

	//这里源码的处理方式与我们的代码有点错误，需要进行修正，不然网络多人传值不正确,原因在于，源码用了无符号的形式存储pitch
	AO_Pitch=GetBaseAimRotation().Pitch;
	if(AO_Pitch>90.f&&!IsLocallyControlled())
	{
		FVector2D InRange(270.f,360.f);
		FVector2D OutRange(-90,0.f);
		AO_Pitch=FMath::GetMappedRangeValueClamped(InRange,OutRange,AO_Pitch);
	}
}

//处理模拟端的旋转不平滑问题
void APlayerCharacter::SimProxiesTurn()
{
	if(CombatComponent==nullptr||CombatComponent==nullptr)return;
	float Speed=CalculateSpeed();
	bRotateRootBone=false;
	if(Speed>0.f)
	{
		TurningInPlace=ETurningInPlace::ETIP_NotTurning;
		return;
	}

	ProxyRotationLastFrame=ProxyRotation;
	ProxyRotation=GetActorRotation();
	ProxyYaw=UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation,ProxyRotationLastFrame).Yaw;

	//重新处理转身
	if(FMath::Abs(ProxyYaw)>TurnThreshold)
	{
		if(ProxyYaw>TurnThreshold)
		{
			TurningInPlace=ETurningInPlace::ETIP_Right;
		}
		else if(ProxyYaw<-TurnThreshold)
		{
			TurningInPlace=ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace=ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace=ETurningInPlace::ETIP_NotTurning;
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

bool APlayerCharacter::IsWeaponEquipped()
{
	return (CombatComponent&&CombatComponent->EquippedWeapon);
}

bool APlayerCharacter::IsAiming()
{
	return (CombatComponent&&CombatComponent->bAiming);
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

void APlayerCharacter::TurnInPlace(float DeltaTime)
{
	if(AO_Yaw>=90.f)
	{
		TurningInPlace=ETurningInPlace::ETIP_Right;
	}
	else if(AO_Yaw<-90.f)
	{
		TurningInPlace=ETurningInPlace::ETIP_Left;
	}
	if(TurningInPlace!=ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw=FMath::FInterpTo(InterpAO_Yaw,0.f,DeltaTime,4.f);
		AO_Yaw=InterpAO_Yaw;
		if(FMath::Abs(AO_Yaw)<15.f)
		{
			TurningInPlace=ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation=FRotator(0.f,GetBaseAimRotation().Yaw,0.f);
		}
	}
}

//多播受伤蒙太奇
// void APlayerCharacter::MulticastHit_Implementation()
// {
// 	PlayHitReactMontage();
// }

void APlayerCharacter::HideCameraIfCharacterClose()
{
	if(!IsLocallyControlled())return;
	if((FollowCamera->GetComponentLocation()-GetActorLocation()).Size()<CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if(CombatComponent&&CombatComponent->EquippedWeapon&&CombatComponent->EquippedWeapon->GetWeaponMesh())
		{
			CombatComponent->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee=true;
		}
	}else
	{
		GetMesh()->SetVisibility(true);
		if(CombatComponent&&CombatComponent->EquippedWeapon&&CombatComponent->EquippedWeapon->GetWeaponMesh())
		{
			CombatComponent->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee=false;
		}
	}
}

float APlayerCharacter::CalculateSpeed()
{
	FVector Velocity=GetVelocity();
	Velocity.Z=0.f;
	return Velocity.Size();
}


void APlayerCharacter::ServerEquipButtonPressed_Implementation()
{
	CombatComponent->EquipWeapon(OverlapWeapon);
}

//接受子弹伤害
void APlayerCharacter::ReceiveDamage(AActor* DamageActor, float Damage, const UDamageType* DAmageType,
	AController* InstigatorController, AActor* DamageCauser)
{
	Health=FMath::Clamp(Health-Damage,0.f,MaxHealth);
	UpdateHUDHealth();
	PlayHitReactMontage();

	//调用GameMode从而实现玩家淘汰等功能
	if(Health==0.f)
	{
		AGameLevel1GameMode* GameLevel1GameMode= GetWorld()->GetAuthGameMode<AGameLevel1GameMode>();
		if(GameLevel1GameMode)
		{
			MyPlayerController=MyPlayerController==nullptr?Cast<AMyPlayerController>(Controller):MyPlayerController;
			AMyPlayerController* AttackerController=Cast<AMyPlayerController>(InstigatorController);
			GameLevel1GameMode->PlayerEliminated(this,MyPlayerController,AttackerController);
		}
	}
	
}


//Player Stats
void APlayerCharacter::OnRep_Health()
{
	UpdateHUDHealth();
	PlayHitReactMontage();
	
}
void APlayerCharacter::UpdateHUDHealth()
{
	MyPlayerController=MyPlayerController==nullptr? Cast<AMyPlayerController>(Controller):MyPlayerController;
	if(MyPlayerController)
	{
		MyPlayerController->SetHUDHealth(Health,MaxHealth);
	}

}
