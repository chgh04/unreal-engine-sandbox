// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemyWidget.h"
#include "Components/ProgressBar.h"

void UBaseEnemyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (HPbar)
	{
		HPbar->SetPercent(1.0f);
	}
}

void UBaseEnemyWidget::UpdateHP(float Percent)
{
	if (HPbar)
	{
		HPbar->SetPercent(Percent);
	}
}

