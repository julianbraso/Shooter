// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSubsystem.h"
#include "Engine.h"
#include "UnrealNetwork.h"
#include "Online.h"
#include "FindSessionsCallbackProxy.h"
#include "MenuWidget.generated.h"

/**
 * 
 */

UCLASS()

class MYFPS_API UMenuWidget : public UUserWidget
{
	
	//UMenuWidget(const FObjectInitializer& ObjectInitializer);
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
		void HandleClientTravel(APlayerController* PC, FBlueprintSessionResult result);
	UFUNCTION()
		void HandleDelegate();
	UPROPERTY()
		FName Session_Name;
	
		IOnlineSubsystem* OnlineSub;
	
		IOnlineSessionPtr Sessions;

	//FJoinCompleteDelegate JoinComplete_Del;

	
};