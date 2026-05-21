// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "BasePlayerController.generated.h"

/**
 * 
 */

class UEndGameWidget;

UCLASS()
class FINALPROJECT_API ABasePlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UBasePlayerWidget> PlayerWidgetClass;

	UPROPERTY()
	UBasePlayerWidget* PlayerWidget;

public:
	ABasePlayerController();

	virtual FGenericTeamId GetGenericTeamId() const override { return TeamID; }
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override { TeamID = NewTeamID; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UEndGameWidget> GameOverWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UEndGameWidget> GameClearWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UEndGameWidget* CurrentEndGameWidget;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowGameOver();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowGameClear();

private:
	FGenericTeamId TeamID;
};
