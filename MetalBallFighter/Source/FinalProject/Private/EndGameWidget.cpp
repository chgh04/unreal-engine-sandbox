// Fill out your copyright notice in the Description page of Project Settings.


#include "EndGameWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFrameWork/PlayerController.h"

void UEndGameWidget::RestartLevel()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(World, true);

	if (APlayerController* PC = GetOwningPlayer())
	{	
		FInputModeGameOnly GameMode;
		PC->SetInputMode(GameMode);

		PC->bShowMouseCursor = false;
		PC->SetPause(false);
	}

	UGameplayStatics::OpenLevel(World, FName(*CurrentLevelName));
}

void UEndGameWidget::QuitGame()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	APlayerController* PC = GetOwningPlayer();

	UKismetSystemLibrary::QuitGame(World, PC, EQuitPreference::Quit, false);
}

void UEndGameWidget::SetupEndGameInput(bool bPauseGame)
{	
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (bPauseGame)
		{
			PC->SetPause(true);
		}

		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}
}