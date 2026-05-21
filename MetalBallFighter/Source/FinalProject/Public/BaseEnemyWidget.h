// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseEnemyWidget.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UBaseEnemyWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPbar;

	UFUNCTION(BlueprintCallable)
	void UpdateHP(float Percent);
};
