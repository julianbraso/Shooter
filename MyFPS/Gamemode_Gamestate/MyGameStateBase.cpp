// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameStateBase.h"
#include "Net/UnrealNetwork.h"

AMyGameStateBase::AMyGameStateBase() {

}

void AMyGameStateBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyGameStateBase,AlivePlayers);
	DOREPLIFETIME(AMyGameStateBase, Timer_Seconds);
	DOREPLIFETIME(AMyGameStateBase, Timer_Minutes);
}
