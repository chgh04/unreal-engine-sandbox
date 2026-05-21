// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"
#include "Blueprint/UserWidget.h"
#include "BasePlayerWidget.h"
#include "BaseCharacter.h"
#include "EndGameWidget.h"

ABasePlayerController::ABasePlayerController()
{
	TeamID = FGenericTeamId(1);
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerWidgetClass)
	{
		PlayerWidget = CreateWidget<UBasePlayerWidget>(this, PlayerWidgetClass);
		PlayerWidget->AddToViewport();

		ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(GetPawn());
		if (PlayerCharacter && PlayerWidget)
		{
			PlayerWidget->InitializeWidget(PlayerCharacter);

			PlayerCharacter->OnHealthChanged.AddDynamic(PlayerWidget, &UBasePlayerWidget::UpdateHP);
			PlayerCharacter->OnExperienceChanged.AddDynamic(PlayerWidget, &UBasePlayerWidget::UpdateEXP);
			PlayerCharacter->OnLevelUp.AddDynamic(PlayerWidget, &UBasePlayerWidget::UpdateLevel);
			PlayerCharacter->OnHealingItemChanaged.AddDynamic(PlayerWidget, &UBasePlayerWidget::UpdateHealingItem);
		}
	}
}

void ABasePlayerController::ShowGameOver()
{
	if (!GameOverWidgetClass)
	{
		return;
	}

	CurrentEndGameWidget = CreateWidget<UEndGameWidget>(this, GameOverWidgetClass);
	if (CurrentEndGameWidget)
	{
		CurrentEndGameWidget->AddToViewport();
		CurrentEndGameWidget->SetupEndGameInput(true);
	}
}

void ABasePlayerController::ShowGameClear()
{
	if (!GameOverWidgetClass)
	{
		return;
	}

	CurrentEndGameWidget = CreateWidget<UEndGameWidget>(this, GameClearWidgetClass);
	if (CurrentEndGameWidget)
	{
		CurrentEndGameWidget->AddToViewport();
		CurrentEndGameWidget->SetupEndGameInput(true);
	}
}