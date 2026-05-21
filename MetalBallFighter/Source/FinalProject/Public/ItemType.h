// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemType.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EItemType : uint8
{
    Heal      UMETA(DisplayName = "Heal"),
    Exp       UMETA(DisplayName = "Experience"),
    Buff      UMETA(DisPlayName = "Buff"),
    Other     UMETA(DisplayName = "Other")
};
