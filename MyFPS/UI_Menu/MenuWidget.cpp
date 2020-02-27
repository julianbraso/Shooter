// Fill out your copyright notice in the Description page of Project Settings.

#include "MenuWidget.h"
#include "OnlineSubsystem.h"
#include "Engine.h"
#include "UnrealNetwork.h"
#include "FindSessionsCallbackProxy.h"
#include "Online.h"

/*UMenuWidget::UMenuWidget(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer) {
	// Get OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		Sessions->OnJoinSessionCompleteDelegates
	}
}*/

void UMenuWidget::HandleDelegate() {

}

void UMenuWidget::HandleClientTravel(APlayerController* PC, FBlueprintSessionResult result) {

	
		if (Sessions.IsValid())
		{
			FString TravelURL;
			FString t = result.OnlineResult.Session.SessionSettings.Settings.FindRef("SESSION_NAME").Data.ToString();
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, Sessions->GetResolvedConnectString(FName(*t), TravelURL)?"true":"false");
			if (PC && Sessions->GetResolvedConnectString(FName(*t), TravelURL))
			{
				// how it really looks like
				//GLog->Log("TravelURL", TravelURL);
				PC->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute,true);
			}
		}
}


