// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponSightAttachment.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Components/SceneCaptureComponent2D.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"

AWeaponSightAttachment::AWeaponSightAttachment() {
	ADSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ADS camera"));
	ADSCameraComponent->SetupAttachment(Mesh_FP);
	ProxyMesh = CreateDefaultSubobject<UStaticMesh>(TEXT("ProxyMesh"));
	//ProxyMesh->SetupAttachment(Mesh_FP);
	//ProxyMesh->SetVisibility(false, false);
	//SceneCaptureComp = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Scene capture comp"));
	//SceneCaptureComp->SetupAttachment(ADSCameraComponent);

	bReplicates = true;
}

void AWeaponSightAttachment::SetAiming(bool b)
{
		if (AttachmentType == EAttachmentType::AT_Scope)
		{
//			SceneCaptureComp->SetVisibility(b);
			if (b)
			{
				SetLensOpacity(0.2f);
				Mesh_FP->SetStaticMesh(ProxyMesh);
			}
			else {
				SetLensOpacity(1.0f);
				Mesh_FP->SetStaticMesh(bufferMesh);
			}
		}
}


void AWeaponSightAttachment::BeginPlay()
{
	//SetAiming(false);
	bufferMesh = Mesh_FP->GetStaticMesh();
}

void AWeaponSightAttachment::Tick(float DeltaTime)
{
}


