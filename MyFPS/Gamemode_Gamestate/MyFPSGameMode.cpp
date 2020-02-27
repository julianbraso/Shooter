// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "MyFPSGameMode.h"
#include "MyGameStateBase.h"
#include "MyPlayerState.h"
#include "GameInstanceSteam.h"
#include "MyFPSHUD.h"
#include "Kismet/GameplayStatics.h"
#include "MyFPSCharacter.h"
#include "MyPlayerController.h"
#include "UObject/ConstructorHelpers.h"


AMyFPSGameMode::AMyFPSGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AMyFPSHUD::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassFinder(TEXT("/Game/Blueprints/PlayerController_BP"));
	PlayerControllerClass = PlayerControllerClassFinder.Class;

	static ConstructorHelpers::FClassFinder<AMyGameStateBase> GameStateClassFinder(TEXT("/Game/Blueprints/GameState_BP"));
	GameStateClass = GameStateClassFinder.Class;

	static ConstructorHelpers::FClassFinder<AMyPlayerState> PlayerStateClassFinder(TEXT("/Game/Blueprints/PlayerState"));
	PlayerStateClass = PlayerStateClassFinder.Class;

	StartMatchDelay = 90.f;

	fRandomSpawnRange = 400.f;

	bBegan = false;

	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;

}

void AMyFPSGameMode::PlayerDied(AController* killer, APlayerController* victim)
{
	playersAlive-=1;
	//GetGameState<AMyGameStateBase>()
	GetGameState<AMyGameStateBase>()->AlivePlayers = playersAlive;
	killer->PlayerState->Score += 1;
	

	if (GameModeType == EGameModeType::GM_FFA)
	{
		if (!victim)
		{
			GLog->Log("NO PLAYER CONTROLLER FOUND");
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "NO PLAYER CONTROLLER FOUND");
			return;
		}
		victim->UnPossess();
		Respawn(victim);
	}
	
	AMyFPSCharacter* ps = Cast<AMyFPSCharacter>(victim->GetPawn());

	if (ps)
	{
		ps->PrintToScreen("killed by " + killer->GetName());
	}

	/*AMyPlayerState* ps = Cast<AMyPlayerState>(killer->PlayerState);
	if (ps) {
		ps->
	}*/
}

void AMyFPSGameMode::Tick(float deltaTime) {
	if (GameModeType == EGameModeType::GM_BR)
	{
		if (playersAlive >= minPlayersToStart)
		{
			if (bBegan == false)
			{
				BeginTimer();
			}
			//StartMatch();
		}
	}
}

FVector AMyFPSGameMode::GetRandomSpawnPointIn(FVector Loc)
{
	float x = FMath::RandRange(Loc.X, Loc.X + fRandomSpawnRange);
	float y = FMath::RandRange(Loc.Y, Loc.Y + fRandomSpawnRange);
	float z = Loc.Z;
	return FVector(x,y,z);
}

void AMyFPSGameMode::Respawn(APlayerController * NewPlayer)
{
	if (PSArray.Num() == 0)
	{
		GLog->Log("No player start!");
		return;
	}

	FActorSpawnParameters SpawnInfo;
	FRotator Rotation(0.0f, 0.0f, 0.0f);

	if (!DefaultPlayerClass) {
		GLog->Log("No default player class");
		return;
	}

	if (!NewPlayer) {
		GLog->Log("No valid player reference passed onto respawn...");
		return;
	}

	playersAlive += 1;
	GetGameState<AMyGameStateBase>()->AlivePlayers = playersAlive;

	FTransform T = FTransform(Rotation, PSArray[FMath::RandRange(0, PSArray.Num() - 1)]->GetActorLocation(), FVector(1, 1, 1));

	AMyFPSCharacter* c = GetWorld()->SpawnActorDeferred<AMyFPSCharacter>(DefaultPlayerClass , T);
	
	//c->SetActorLocation(GetRandomSpawnPointIn(PSArray[FMath::RandRange(0, PSArray.Num() - 1)]->GetActorLocation()));
	//c->PossessedBy(NewPlayer);
	//RestartPlayer(NewPlayer);
	if (c)
	{
		NewPlayer->Possess(c);
		c->FinishSpawning(T);
	}
	else {
		GLog->Log("Error spawning new player...");
		Respawn(NewPlayer);
	}
}

void AMyFPSGameMode::PostLogin(APlayerController* NewPlayer) {
	AMyFPSCharacter * c = Cast<AMyFPSCharacter>(NewPlayer->GetPawn());
	if (c)
	{
		c->WaitForLevelStreaming();
		//c->ClientHandleClothing();
	}
	else {
		GLog->Log("Failed to cast player on PostLogin");
	}

		/*
		TArray<FItemData> a = pS->GetClothing();
		for (int i = 0;i<a.Num();i++)
		{
			if (a[i].ClothingClass)
			{
				switch (a[i].ClothingClass->GetDefaultObject<AClothingBaseClass>()->ClothingData.ClothingType)
				{
				case EClothingType::CT_Boots:
					gI->Clothing_Feet=a[i];
					break;
				case EClothingType::CT_Torso:
					gI->Clothing_Torso = a[i];
					break;
				case EClothingType::CT_Pants:
					gI->Clothing_Legs = a[i];
					break;
				case EClothingType::CT_Jacket:
					gI->Clothing_Jacket = a[i];
					break;
				default:
					break;
				}
			}
		}*/
	Super::PostLogin(NewPlayer);
	playerArray.Add(NewPlayer);
	playersAlive += 1;
	GetGameState<AMyGameStateBase>()->AlivePlayers = playersAlive;

	if (GameModeType == EGameModeType::GM_BR)
	{
		TArray<AActor*> a;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("LobbyPS")), a);

		if (a.Num() > 0)
		{
			LobbyPlayerStart = Cast<APlayerStart>(a[0]);
		}

		if (LobbyPlayerStart)
		{
			NewPlayer->GetPawn()->SetActorLocation(GetRandomSpawnPointIn(LobbyPlayerStart->GetActorLocation()));
		}
		else {
			GLog->Log("No player start!");
		}
	}

	if (GameModeType == EGameModeType::GM_FFA)
	{
		if (PSArray.Num() == 0)
		{
			GLog->Log("No player start!");
			return;
		}
		NewPlayer->GetPawn()->SetActorLocation(GetRandomSpawnPointIn(PSArray[FMath::RandRange(0,PSArray.Num()-1)]->GetActorLocation()));

	}

}

void AMyFPSGameMode::Logout(AController * Exiting)
{
	Super::Logout(Exiting);
	//playersAlive--;
	//GetGameState<AMyGameStateBase>()->AlivePlayers = playersAlive;
}

void AMyFPSGameMode::BeginPlay()
{
	if (GameModeType == EGameModeType::GM_FFA)
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PSArray);
	}

	UGameInstanceSteam * gI = Cast<UGameInstanceSteam>(GetWorld()->GetGameInstance());
	if (gI)
	{

	}

	/*GetWorld()->GetTimerManager().SetTimer(TimerHandle, this,&AMyFPSGameMode::StartMatch,StartMatchDelay,false );
	CurrentTimerSeconds = StartMatchDelay;
	MatchTimer();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle2, this, &AMyFPSGameMode::MatchTimer, 1.0f, true);*/
}

void AMyFPSGameMode::MatchTimer()
{
	if (CurrentTimerSeconds > 0)
	{
		CurrentTimerSeconds -= 1;
	}
	
	if (CurrentTimerSeconds >= 60) {
		TimerMinutes += 1;
		CurrentTimerSeconds -= 60;
	}
	if (TimerMinutes > 0 && CurrentTimerSeconds <= 0)
	{
		CurrentTimerSeconds = 60;
		TimerMinutes -= 1;
	}
	TimerSeconds = CurrentTimerSeconds;
	GetGameState<AMyGameStateBase>()->Timer_Seconds = TimerSeconds;
	GetGameState<AMyGameStateBase>()->Timer_Minutes = TimerMinutes;

	for (int i = 0; i < playerArray.Num();i++) {
		AMyFPSCharacter* p;
		p = Cast<AMyFPSCharacter>(playerArray[i]->GetPawn());
		if (p)
		{
			p->ShowTimer(true);
		}
		else {
			GLog->Log("Error in gamemode: !p");
		}
	}
}

void AMyFPSGameMode::StartMatch()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle2);
	for (int i = 0; i < playerArray.Num(); i++) {
		AMyFPSCharacter* p;
		p = Cast<AMyFPSCharacter>(playerArray[i]->GetPawn());
		p->ShowTimer(false);
		p->SetActorLocation(GetRandomSpawnPointIn(GamePlayerStart->GetActorLocation()));
		p->WaitForLevelStreaming();
	}
	
}

void AMyFPSGameMode::BeginTimer()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AMyFPSGameMode::StartMatch, StartMatchDelay, false);
	CurrentTimerSeconds = StartMatchDelay;
	MatchTimer();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle2, this, &AMyFPSGameMode::MatchTimer, 1.0f, true);
	bBegan = true;
}
