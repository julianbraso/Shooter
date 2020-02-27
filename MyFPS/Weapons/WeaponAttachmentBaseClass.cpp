// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponAttachmentBaseClass.h"
#include "Runtime/Engine/Classes/Components/SceneComponent.h"


// Sets default values
AWeaponAttachmentBaseClass::AWeaponAttachmentBaseClass()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	root = CreateDefaultSubobject<USceneComponent>(FName("root"));
	SetRootComponent(root);
	Mesh_FP = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh_FP"));
	Mesh_FP->SetOnlyOwnerSee(true);
	Mesh_FP->LightingChannels.bChannel0 = false;
	Mesh_FP->LightingChannels.bChannel1 = true;
	Mesh_FP->bCastInsetShadow = true;
	Mesh_TP = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh_TP"));
	Mesh_TP->bCastHiddenShadow = true;
	bReplicates = true;
}

UStaticMeshComponent * AWeaponAttachmentBaseClass::GetMesh_FP()
{

	return Mesh_FP;
}

UStaticMeshComponent * AWeaponAttachmentBaseClass::GetMesh_TP()
{

	return Mesh_TP;
}

// Called when the game starts or when spawned
void AWeaponAttachmentBaseClass::BeginPlay()
{
	Super::BeginPlay();
	
	Mesh_FP->SetOnlyOwnerSee(true);
	Mesh_FP->SetOwnerNoSee(false);
	Mesh_TP->SetOwnerNoSee(true);
}

// Called every frame
void AWeaponAttachmentBaseClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

