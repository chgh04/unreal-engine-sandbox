// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UEndGameWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "EndGame")
	void RestartLevel();

	UFUNCTION(BlueprintCallable, Category = "EndGame")
	void QuitGame();

	UFUNCTION(BlueprintCallable, Category = "EndGame")
	void SetupEndGameInput(bool bPauseGame = true);
};
