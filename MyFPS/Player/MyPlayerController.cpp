// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "MyPlayerCameraManager.h"


AMyPlayerController::AMyPlayerController(){
	PlayerCameraManagerClass = AMyPlayerCameraManager::StaticClass();
}

