// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponAttachmentBaseClass.h"
#include "Runtime/Engine/Classes/Sound/SoundBase.h "
#include "WeaponSupressorAttachment.generated.h"

/**
 * 
 */
UCLASS()
class MYFPS_API AWeaponSupressorAttachment : public AWeaponAttachmentBaseClass
{
	GENERATED_BODY()
		AWeaponSupressorAttachment();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
