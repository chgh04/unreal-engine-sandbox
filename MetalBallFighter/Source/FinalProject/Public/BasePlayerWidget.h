// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BasePlayerWidget.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UBasePlayerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// UI를 갱신할 함수들을 외부에서 호출 가능하도록
	UFUNCTION(BlueprintCallable)
	void InitializeWidget(class ABaseCharacter* Player);

	UFUNCTION(BlueprintCallable)
	void UpdateHP(float CurrentHP, float MaxHP);

	UFUNCTION(BlueprintCallable)
	void UpdateEXP(int32 CurrentXP, int32 RequiredXP);

	UFUNCTION(BlueprintCallable)
	void UpdateLevel(int32 NewLevel);

	UFUNCTION(BlueprintCallable)
	void UpdateHealingItem(int32 NewCount);

protected:

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* EXPBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LevelText;

	UPROPERTY(meta = (BindWIdget))
	UTextBlock* HealingItemText;

private:
	UPROPERTY()
	class ABaseCharacter* CachedPlayer;
};
