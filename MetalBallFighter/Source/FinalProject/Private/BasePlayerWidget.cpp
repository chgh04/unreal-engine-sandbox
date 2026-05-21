// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "BaseCharacter.h"

void UBasePlayerWidget::InitializeWidget(ABaseCharacter* Player)
{
	CachedPlayer = Player;

	UpdateHP(Player->GetCurrentHP(), Player->GetMaxHP());
	UpdateEXP(Player->GetCurrentEXP(), Player->GetRequiredEXP());
	UpdateLevel(Player->GetPlayerLevel());
	UpdateHealingItem(Player->GetHealingItemCount());
}

void UBasePlayerWidget::UpdateHP(float CurrentHP, float MaxHP)
{
	if (HPBar)
	{
		HPBar->SetPercent(CurrentHP / MaxHP);
	}
}

void UBasePlayerWidget::UpdateEXP(int32 CurrentXP, int32 RequiredXP)
{
	if (EXPBar)
	{
		EXPBar->SetPercent((float)CurrentXP / (float)RequiredXP);
	}
}

void UBasePlayerWidget::UpdateLevel(int32 NewLevel)
{
	if (LevelText)
	{
		LevelText->SetText(FText::AsNumber(NewLevel));
	}
}

void UBasePlayerWidget::UpdateHealingItem(int32 NewCount)
{
	if (HealingItemText)
	{
		HealingItemText->SetText(FText::AsNumber(NewCount));
	}
}