// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EAIState : uint8
{
    Idle    UMETA(DisplayName = "Idle"),
    Attack  UMETA(DisplayName = "Attack"),
    Dead    UMETA(DisplayName = "Dead"),
    Patrol  UMETA(DisplayName = "Patrol")
};