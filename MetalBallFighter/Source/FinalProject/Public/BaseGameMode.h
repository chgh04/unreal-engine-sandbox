// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BaseGameMode.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API ABaseGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABaseGameMode();

protected:

	virtual void BeginPlay() override;

public:

	void ClearGame();

	void GameOver();

protected:
	float WidgetTimer = 0.f;

	bool bIsClearGame = false;

	bool bIsGameOver = false;

	void OnClearGameTimerExpired();
	
};
