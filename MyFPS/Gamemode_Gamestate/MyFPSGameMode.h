// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MyFPSCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "Runtime/Core/Public/Math/Vector.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerStart.h"
#include "GameFramework/GameMode.h"
#include "MyFPSGameMode.generated.h"

UENUM(BlueprintType)
enum class EGameModeType : uint8 {
	GM_FFA UMETA(DisplayName = "Free for all"),
	GM_BR UMETA(DisplayName = "Battle Royale")
};

UCLASS(minimalapi)
class AMyFPSGameMode : public AGameModeBase
{
	GENERATED_BODY()
protected:

	void PostLogin(APlayerController* NewPlayer) override;

	void Logout(AController * Exiting) override;

	void BeginPlay();
	void Tick(float deltaTime) override;

	UPROPERTY()
		FTimerHandle TimerHandle;
	UPROPERTY()
		FTimerHandle TimerHandle2;

	UPROPERTY()
		TArray<APlayerController*> playerArray;

	UFUNCTION()
		void MatchTimer();

	UFUNCTION()
		void StartMatch();

	UFUNCTION()
		void BeginTimer();
	UPROPERTY()
		bool bBegan = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float StartMatchDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 minPlayersToStart;

	UPROPERTY()
		int32 TimerSeconds;

	UPROPERTY()
		float CurrentTimerSeconds;

	UPROPERTY()
		int32 TimerMinutes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float fRandomSpawnRange;

	UPROPERTY()
		TArray<AActor*> PSArray;

public:
	AMyFPSGameMode();

	UPROPERTY()
		int32 playersAlive;

	UFUNCTION()
		void PlayerDied(AController* killer,APlayerController* victim);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		APlayerStart* LobbyPlayerStart;

	UFUNCTION()
		FVector GetRandomSpawnPointIn(FVector Loc);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		APlayerStart* GamePlayerStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EGameModeType GameModeType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bFPPOnly;

	UFUNCTION()
		void Respawn(APlayerController* NewPlayer);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AMyFPSCharacter> DefaultPlayerClass;

};



