// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseProjectile.generated.h"

UCLASS()
class FINALPROJECT_API ABaseProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseProjectile();

	UPROPERTY(EditAnywhere, Category = "Status")
	float Speed = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Status")
	float LifeSpan = 5.f;

	UPROPERTY(EditAnywhere, Category = "Status")
	float Damage = 10.f;

	// 0~1
	UPROPERTY(EditAnywhere, Category = "Status")
	float HomingRate;

	UPROPERTY()
	AActor* TargetActor;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	class UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	class UParticleSystemComponent* ParticleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	class UParticleSystem* ParticleTemplate;

	float CurrentLife;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE class UBoxComponent* GetBoxComponent() const { return BoxComponent; }
	FORCEINLINE class UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }

};
