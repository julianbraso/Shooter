// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ClothingBaseClass.h"
#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MYFPS_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	UFUNCTION()
		TArray<FItemData> GetClothing();

	UFUNCTION(Server,Reliable,WithValidation,BlueprintCallable)
		void SetClothing(FItemData c);

protected:
	/*Clothing Variables*/
	UPROPERTY(Replicated, BlueprintReadOnly)
		FItemData Clothing_Torso;
	UPROPERTY(Replicated, BlueprintReadOnly)
		FItemData Clothing_Jacket;
	UPROPERTY(Replicated, BlueprintReadOnly)
		FItemData Clothing_Feet;
	UPROPERTY(Replicated, BlueprintReadOnly)
		FItemData Clothing_Legs;
	void CopyProperties(APlayerState* PlayerState);

	UFUNCTION()
	virtual void Reset() override;
	
};
