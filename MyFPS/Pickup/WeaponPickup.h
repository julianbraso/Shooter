// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupBaseClass.h"
#include "WeaponPickup.generated.h"

/**
 * 
 */
UCLASS()
class MYFPS_API AWeaponPickup : public APickupBaseClass
{
	GENERATED_BODY()
public:
	AWeaponPickup();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AWeaponBaseClass> WeaponClass;
	UPROPERTY()
		int32 AmmoOnWeapon;

};
