// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupBaseClass.h"
#include "WeaponAttachmentBaseClass.h"
#include "WeaponAttachmentPickup.generated.h"

/**
 * 
 */
UCLASS()
class MYFPS_API AWeaponAttachmentPickup : public APickupBaseClass
{
	AWeaponAttachmentPickup();
	GENERATED_BODY()
	
protected:

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AWeaponAttachmentBaseClass> AttachmentClass;
	
	
};
