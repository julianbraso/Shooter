// Fill out your copyright notice in the Description page of Project Settings.

#include "ArmorBaseClass.h"


// Sets default values
AArmorBaseClass::AArmorBaseClass()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SK_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Armor Skeletal Mesh"));
}

// Called when the game starts or when spawned
void AArmorBaseClass::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AArmorBaseClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

