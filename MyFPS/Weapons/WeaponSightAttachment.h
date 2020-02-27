// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponAttachmentBaseClass.h"
#include "WeaponSightAttachment.generated.h"

/**
 * 
 */
UCLASS()
class MYFPS_API AWeaponSightAttachment : public AWeaponAttachmentBaseClass
{
	AWeaponSightAttachment();
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* ADSCameraComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ADS, meta = (AllowPrivateAccess = "true"))
		class USceneCaptureComponent2D* SceneCaptureComp;
	UFUNCTION()
		void SetAiming(bool b);
	UFUNCTION(BlueprintImplementableEvent)
		void SetLensOpacity(float f);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UStaticMesh* ProxyMesh;
	UPROPERTY()
		class UStaticMesh* bufferMesh;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere)
		float FieldOfView = 70.0f;
};
