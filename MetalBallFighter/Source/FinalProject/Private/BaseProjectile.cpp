// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseProjectile.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "BaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "BaseEnemyCharacter.h"

// Sets default values
ABaseProjectile::ABaseProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	SetRootComponent(BoxComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	MeshComponent->SetupAttachment(BoxComponent);

	ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireFX"));
	ParticleComponent->SetupAttachment(RootComponent);

	ParticleComponent->bAutoActivate = true;
}

// Called when the game starts or when spawned
void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentLife = LifeSpan;

	if (ParticleTemplate)
	{
		ParticleComponent->SetTemplate(ParticleTemplate);
		ParticleComponent->Activate();
	}
}

// Called every frame
void ABaseProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentLife -= DeltaTime;

	if (CurrentLife > 0.0f)
	{
		FVector CurrentLocation = GetActorLocation();
		FVector CurrentDir = GetActorForwardVector();
		FVector Vel;

		if (TargetActor)
		{
			FVector TargetDir = (TargetActor->GetActorLocation() - CurrentLocation).GetSafeNormal();
			FVector NewDir = FMath::Lerp(CurrentDir, TargetDir, HomingRate).GetSafeNormal();
			Vel = NewDir * Speed * DeltaTime;
		}
		else
		{
			Vel = CurrentDir * Speed * DeltaTime;
		}

		FVector NextLocation = CurrentLocation + Vel;
		SetActorLocation(NextLocation);

		FHitResult HitResult;
		FCollisionObjectQueryParams CollsionObjectQueryParams;
		CollsionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);

		if (GetWorld()->LineTraceSingleByObjectType(HitResult, CurrentLocation, NextLocation, CollsionObjectQueryParams))
		{
			ABaseCharacter* Character = Cast<ABaseCharacter>(HitResult.GetActor());
			if (Character)
			{
				UGameplayStatics::ApplyPointDamage(
					Character,
					Damage,
					(NextLocation - CurrentLocation).GetSafeNormal(),
					HitResult,
					GetInstigatorController(),
					this,
					UDamageType::StaticClass()
				);

				Destroy();
			}
			else
			{
				ABaseEnemyCharacter* EnemyCharacter = Cast<ABaseEnemyCharacter>(HitResult.GetActor());
				if (EnemyCharacter)
				{
					UGameplayStatics::ApplyPointDamage(
						EnemyCharacter,
						(Damage/2),
						(NextLocation - CurrentLocation).GetSafeNormal(),
						HitResult,
						GetInstigatorController(),
						this,
						UDamageType::StaticClass()
					);

					Destroy();
				}
			}
		}
	}
	else
	{
		Destroy();
	}

}

