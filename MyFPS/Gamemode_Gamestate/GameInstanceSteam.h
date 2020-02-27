// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Core/Public/Delegates/Delegate.h"
#include "ClothingBaseClass.h"
#include "AdvancedFriendsGameInstance.h"
#include "GameInstanceSteam.generated.h"

/**
 * 
 */

//DECLARE_MULTICAST_DELEGATE_TwoParams(FJoinDelegate, FName, SessionName, EOnJoinSessionCompleteResult::Type, Result);

UCLASS(Blueprintable, BlueprintType)
class MYFPS_API UGameInstanceSteam : public UAdvancedFriendsGameInstance
{
	UGameInstanceSteam(const FObjectInitializer& ObjectInitializer);
	GENERATED_BODY()

public:
	/*Clothing Variables*/
	UPROPERTY()
		FItemData Clothing_Torso;
	UPROPERTY()
		FItemData Clothing_Jacket;
	UPROPERTY()
		FItemData Clothing_Feet;
	UPROPERTY()
		FItemData Clothing_Legs;
	
};
