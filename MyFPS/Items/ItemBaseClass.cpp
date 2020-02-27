// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemBaseClass.h"
#include "WeaponBaseClass.h"
#include "ArmorBaseClass.h"
#include "ClothingBaseClass.h"
#include "Items/AmmoItemClass.h"
#include "PickupBaseClass.h"

// Sets default values
AItemBaseClass::AItemBaseClass()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AItemBaseClass::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItemBaseClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

