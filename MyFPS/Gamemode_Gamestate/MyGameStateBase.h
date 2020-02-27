// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MyGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class MYFPS_API AMyGameStateBase : public AGameStateBase
{
	AMyGameStateBase();
	GENERATED_BODY()
	
public:
	UPROPERTY(Replicated,BlueprintReadOnly)
		int32 AlivePlayers;
	UPROPERTY(Replicated, BlueprintReadOnly)
		float Timer_Seconds;
	UPROPERTY(Replicated, BlueprintReadOnly)
		float Timer_Minutes;
};
