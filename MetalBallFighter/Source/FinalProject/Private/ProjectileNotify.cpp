// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileNotify.h"
#include "BaseEnemyCharacter.h"

void UProjectileNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp)
	{
		return;
	}

	ABaseEnemyCharacter* Enemy = Cast<ABaseEnemyCharacter>(MeshComp->GetOwner());

	if (Enemy)
	{
		Enemy->SpawnProjectile();
	}
}