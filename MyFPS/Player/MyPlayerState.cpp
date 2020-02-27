// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerState.h"
#include "UnrealNetwork.h"
#include "GameInstanceSteam.h"
#include "ClothingBaseClass.h"

void AMyPlayerState::Reset() {
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "Reset called! not doing it pal! fuck ye...");
}

TArray<FItemData> AMyPlayerState::GetClothing() {
	TArray<FItemData> iArray;
	if (Clothing_Feet.ClothingClass)
	{
		iArray.Add(Clothing_Feet);
	}
	if (Clothing_Torso.ClothingClass)
	{
		iArray.Add(Clothing_Torso);
	}
	if (Clothing_Legs.ClothingClass)
	{
		iArray.Add(Clothing_Legs);
	}
	if (Clothing_Jacket.ClothingClass)
	{
		iArray.Add(Clothing_Jacket);
	}
	return iArray;
}

void AMyPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// Replicate to everyone
	DOREPLIFETIME(AMyPlayerState, Clothing_Feet);
	DOREPLIFETIME(AMyPlayerState, Clothing_Torso);
	DOREPLIFETIME(AMyPlayerState, Clothing_Jacket);
	DOREPLIFETIME(AMyPlayerState, Clothing_Legs);
}

bool AMyPlayerState::SetClothing_Validate(FItemData c) {
	return true;
}

void AMyPlayerState::SetClothing_Implementation(FItemData c)					///////NOT SO SURE ABOUT SECURITY IN THIS ONEE!!!!!
{
	if (!c.ClothingClass)
	{
		return;
	}

	UGameInstanceSteam * gI = Cast<UGameInstanceSteam>(GetWorld()->GetGameInstance());

	switch (c.ClothingClass->GetDefaultObject<AClothingBaseClass>()->ClothingData.ClothingType)
	{
	case EClothingType::CT_Boots:
		if (Clothing_Feet.ClothingClass == c.ClothingClass)
		{
			Clothing_Feet = FItemData();
			if (gI)
			{
				gI->Clothing_Feet = FItemData();
			}
			break;
		}
		Clothing_Feet = c;
		if (gI)
		{
			gI->Clothing_Feet = c;
		}
		break;
	case EClothingType::CT_Pants:
		if (Clothing_Legs.ClothingClass == c.ClothingClass)
		{
			Clothing_Legs = FItemData();
			if (gI)
			{
				gI->Clothing_Legs = FItemData();
			}
			break;
		}
		Clothing_Legs = c;
		if (gI)
		{
			gI->Clothing_Legs = c;
		}
		break;
	case EClothingType::CT_Torso:
		if (Clothing_Torso.ClothingClass == c.ClothingClass)
		{
			Clothing_Torso = FItemData();
			if (gI)
			{
				gI->Clothing_Torso = FItemData();
			}
			break;
		}
		Clothing_Torso = c;
		if (gI)
		{
			gI->Clothing_Torso = c;
		}
		break;
	case EClothingType::CT_Jacket:
		if (Clothing_Jacket.ClothingClass == c.ClothingClass)
		{
			Clothing_Jacket = FItemData();
			if (gI)
			{
				gI->Clothing_Jacket = FItemData();
			}
			break;
		}
		Clothing_Jacket = c;
		if (gI)
		{
			gI->Clothing_Jacket = c;
		}
		break;
	default:
		break;
	}
}

void AMyPlayerState::CopyProperties(APlayerState * PlayerState)
{
	if (PlayerState != nullptr)
	{
		AMyPlayerState* ps = Cast<AMyPlayerState>(PlayerState);
		if (ps)
		{
			ps->Clothing_Feet = Clothing_Feet;
			ps->Clothing_Jacket = Clothing_Jacket;
			ps->Clothing_Torso = Clothing_Torso;
			ps->Clothing_Legs = Clothing_Legs;
		}
	}
}
