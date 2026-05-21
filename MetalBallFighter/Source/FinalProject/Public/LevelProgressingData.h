// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelProgressingData.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct FLevelProgressingData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Level = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 CurrentXP = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RequiredXP = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float GrowthRate = 1.25f;
};
