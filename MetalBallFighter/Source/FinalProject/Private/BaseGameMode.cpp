// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameMode.h"
#include "BasePlayerController.h"
#include "Kismet/GameplayStatics.h"


ABaseGameMode::ABaseGameMode()
{
	bIsClearGame = false;
	bIsGameOver = false;
}

void ABaseGameMode::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false);
}

void ABaseGameMode::ClearGame()
{	
	UE_LOG(LogTemp, Warning, TEXT("ClearGameCalled"));
	

	FTimerHandle ClearHandle;
	GetWorldTimerManager().SetTimer(
		ClearHandle,
		this,
		&ABaseGameMode::OnClearGameTimerExpired,
		5.0f,
		false
	);
}

void ABaseGameMode::OnClearGameTimerExpired()
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (ABasePlayerController* BasePC = Cast<ABasePlayerController>(PC))
		{
			BasePC->ShowGameClear();
		}
	}
}

void ABaseGameMode::GameOver()
{
	bIsGameOver = true;

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (ABasePlayerController* BasePC = Cast<ABasePlayerController>(PC))
		{
			BasePC->ShowGameOver();
		}
	}
}