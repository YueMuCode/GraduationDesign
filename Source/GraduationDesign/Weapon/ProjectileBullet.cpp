// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

//子弹伤害
void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	ACharacter*OwnerCharacter=Cast<ACharacter>(GetOwner());//子弹的owner是武器，武器的owner是玩家，所有能拿到玩家
	if(OwnerCharacter)
	{
		AController*OwnerController=OwnerCharacter->Controller;
		if(OwnerController)
		{
			UGameplayStatics::ApplyDamage(OtherActor,Damage,OwnerController,this,UDamageType::StaticClass());
		}
	}
	//父级定义了destroy，先调用super会摧毁
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	
}
