// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "MyPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class MYFPS_API AMyPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
protected:
	FRotator camRot;
	AMyPlayerCameraManager();

	void BeginPlay();
	
	
	
};
