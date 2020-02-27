// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupBaseClass.h"
#include "ClothingBaseClass.h"
#include "ClothingPickupClass.generated.h"

/**
 * 
 */
UCLASS()
class MYFPS_API AClothingPickupClass : public APickupBaseClass
{
	GENERATED_BODY()
		AClothingPickupClass();
public:
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AClothingBaseClass> ClothingClass;
	
	
	
};
