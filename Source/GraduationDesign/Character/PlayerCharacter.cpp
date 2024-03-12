// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GraduationDesign/HUD/OverHeadWidget.h"

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

