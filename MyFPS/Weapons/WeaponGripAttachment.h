// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponAttachmentBaseClass.h"
#include "WeaponGripAttachment.generated.h"

/**
 * 
 */
UCLASS()
class MYFPS_API AWeaponGripAttachment : public AWeaponAttachmentBaseClass
{
	GENERATED_BODY()
	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	AWeaponGripAttachment();
};
