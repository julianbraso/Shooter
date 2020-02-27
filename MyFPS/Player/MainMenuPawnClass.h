// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ClothingBaseClass.h"
#include "MyPlayerState.h"
#include "Runtime/Engine/Classes/Components/SceneComponent.h"
#include "MainMenuPawnClass.generated.h"

UCLASS()
class MYFPS_API AMainMenuPawnClass : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMainMenuPawnClass();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
		class USceneComponent* root;
	/** Pawn mesh: 3rd person view body parts */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* Mesh3P_Legs;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* Mesh3P_Head;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* Mesh3P_Arms_lower;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* Mesh3P_Arms_upper;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* Mesh3P_Feet;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* Mesh3P_Torso;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* Mesh3P_Jacket;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* TP_BodyArmor;

	UPROPERTY()
		USkeletalMesh* NakedTorso;
	UPROPERTY()
		USkeletalMesh* NakedHead;
	UPROPERTY()
		USkeletalMesh* NakedArms;
	UPROPERTY()
		USkeletalMesh* NakedFeet;
	UPROPERTY()
		USkeletalMesh* NakedLegs;
	UPROPERTY()
		AMyPlayerState* PState;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void ChangeClothing(FClothingData c);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	
};
