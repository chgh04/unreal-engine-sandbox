// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BaseCharacter.h"

// Sets default values
ABaseItem::ABaseItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	RootComponent = CollisionSphere;

	CollisionSphere->InitSphereRadius(50.f);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Overlap);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::OnItemBeginOverlap);
}

// Called every frame
void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseItem::OnItemBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Picked Up Item: %s"), *ItemName);

		switch (ItemType)
		{
		case EItemType::Heal:
			PlayerCharacter->AddHealingItem(1);
			break;

		case EItemType::Exp:
			break;

		case EItemType::Buff:
			break;

		case EItemType::Other:
			break;

		default:
			break;
		}


		Destroy();
	}
	else
	{
		return;
	}
}

