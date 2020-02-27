// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WeaponBaseClass.h"
#include "MyGameStateBase.h"
#include "MyPlayerState.h"
#include "PickupBaseClass.h"
#include "ItemBaseClass.h"
#include "Runtime/Engine/Classes/Components/TimelineComponent.h"
#include "MyFPSCharacter.generated.h"



class UInputComponent;

UCLASS(config=Game)
class AMyFPSCharacter : public ACharacter
{
	GENERATED_BODY()

		/** Pawn mesh: 1st person view (arms; seen only by self) */
		UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* Mesh1P_Arms_lower;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* Mesh1P_Arms_upper;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* Mesh1P_Legs;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* Mesh1P_Torso;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* Mesh1P_Jacket;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* Mesh1P_Feet;


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
		class USkeletalMeshComponent* FP_BodyArmor;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* TP_BodyArmor;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* ThirdPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* FPCameraSceneComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* TransitionCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* ADSCameraComponent;      /*Aim down sights camera*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
		class UAudioComponent* AudioComponent;
public:
	AMyFPSCharacter();
	virtual void Tick(float deltaTime) override;

	UPROPERTY(EditAnywhere, Category = Camera)
		class USpringArmComponent* ADSCamera;

	UFUNCTION(Client, Reliable)
		void BuildClientInventory();

	UFUNCTION()
		bool GetIsADS();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void EjectWeaponMag();
	UFUNCTION(NetMulticast, Reliable)
		void EjectWeaponMag_All();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void SpawnWeaponMag();

	UFUNCTION(BlueprintCallable)
		void SetUseHandIKTrue();
	UFUNCTION(BlueprintCallable)
		void SetUseHandIKFalse();

	UFUNCTION(Client, Reliable)
		void RefreshADSCamPos();

	UPROPERTY(Replicated)
		bool bFPPOnly;

	bool _aim = false;

	UFUNCTION(Client, Reliable)
		void CallChangeWeapVisib();

	UFUNCTION()
		UCameraComponent* GetADSCam();
	//struct FItemData;

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
		void EquipAttachment(TSubclassOf<AWeaponAttachmentBaseClass> attachment);

	UFUNCTION(Client, Reliable)
		void ClientHandleClothing();


protected:
	virtual void BeginPlay();
	UPROPERTY()
		UTimelineComponent* CamTimeline;

	UPROPERTY()
		FVector TransitionCamStartLoc;
	UPROPERTY()
		FVector TransitionCamTargetLoc;

	UPROPERTY()
		bool CanChangeCamera;

	UPROPERTY()
		UCurveFloat* FloatCurve;

	UPROPERTY()
		FVector CamStartPos;

	UPROPERTY()
		FVector Mesh3pStartPos;

	UFUNCTION()
		void CrouchOffsetUp();

	UFUNCTION()
		void CrouchOffsetDown();

	UFUNCTION()
		void OnRep_Crouch();

	UPROPERTY()
		float CapsuleDefaultSize;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		bool bUseHandIk;

	UFUNCTION()
		void CheckWeaponStateDelay(AWeaponBaseClass * weapon);

	UPROPERTY()
		FTransform WeaponOriginalT;
	UPROPERTY()
		FRotator RotTarget;

	UFUNCTION()
		bool CompareItems(FItemData a, FItemData b);

	UFUNCTION()
		int32 FindItemInInventory(FItemData item);

	UFUNCTION()
		void TimelineCallback(float val);
	UFUNCTION()
		void TimelineFinishedCallback();

	void PlayTimeline();

	//	virtual void OnConstruction();
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = WeaponVars)
		bool bIsUnarmed;

	UPROPERTY()
		class UMyUserWidget* HUDWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
		TSubclassOf<class UMyUserWidget> HUDWidgetRef;

	UPROPERTY()
		class UMyUserWidget* InventoryWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
		TSubclassOf<class UMyUserWidget> InventoryWidgetRef;

	UFUNCTION()
		void AddItemToInventory(FItemData itemData, int32 amount);

	UFUNCTION()
		int32 GetCurrentAmmoByType(EAmmoType type);

	UFUNCTION()
		void DecreaseAmmoByType(EAmmoType type, int32 amount);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void ReloadWeapon();

	UPROPERTY(Replicated)
		bool bIsReloading;

	UFUNCTION()
		void OnReload();

	UFUNCTION(Server, Reliable, WithValidation)
		void PlayReloadAnimationOfType(EWeaponType type);

	UFUNCTION()
		int32 AddWeaponToItemInventory(FItemData itemData);

	UFUNCTION()
		void UnequipArmor(FArmorData armorData);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void RemoveItemFromInventoryAtIndex(int index);

	UPROPERTY()
		bool bIsInInventory;

	UPROPERTY()
		FHitResult HUDTraceHit;

	UPROPERTY()
		bool _b;

	UPROPERTY()
		class APickupBaseClass* pickupRef;

	UFUNCTION()
		void HandleMousePitch(float pitch);

	UFUNCTION(Server, WithValidation, Reliable)
		void SetPitch(float Pitch);

	UFUNCTION()
		void HandleMouseYaw(float yaw);

	UPROPERTY()
		class APickupBaseClass* ActionPickup;

	/*	 WeaponReference	*/
	UPROPERTY(ReplicatedUsing = "OnRep_WeaponRef")
		class AWeaponBaseClass* WeaponRef;


	UPROPERTY()
		class AArmorBaseClass* Armor_Head_Data;

	UPROPERTY()
		class AArmorBaseClass* Armor_Torso_Data;

	/*Clothing Variables*/

	UPROPERTY(ReplicatedUsing = "OnRep_Torso",EditAnywhere)
		class AClothingBaseClass * Clothing_Torso;
	UPROPERTY(ReplicatedUsing = "OnRep_Jacket", EditAnywhere)
		class AClothingBaseClass * Clothing_Jacket;
	UPROPERTY(ReplicatedUsing = "OnRep_Feet", EditAnywhere)
		class AClothingBaseClass * Clothing_Feet;
	UPROPERTY(ReplicatedUsing = "OnRep_Legs", EditAnywhere)
		class AClothingBaseClass * Clothing_Legs;

	UFUNCTION()
		void OnRep_Torso();
	UFUNCTION()
		void OnRep_Jacket();
	UFUNCTION()
		void OnRep_Feet();
	UFUNCTION()
		void OnRep_Legs();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMesh* Default_Feet_Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMesh* Default_Torso_Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMesh* Default_Legs_Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMesh* Default_Head_Mesh;               /*	variables to store default body parts	*/

	/*Clothing Functiones*/
	UFUNCTION()
		void UnequipCloth(AClothingBaseClass* cloth);
	UFUNCTION(Server, Reliable, WithValidation)
		void RPC_Server_UnequipCloth(AClothingBaseClass * cloth);
	UFUNCTION(NetMulticast, Reliable)
		void RPC_All_UnequipCloth(AClothingBaseClass * cloth);
	UFUNCTION()
		void EquipCloth(AClothingBaseClass* cloth);
	UFUNCTION(Client,Reliable)
		void RPC_Client_EquipCloth(AClothingBaseClass* cloth);
	UFUNCTION(Server, Reliable, WithValidation)
		void RPC_Server_EquipCloth(AClothingBaseClass * cloth);
	UFUNCTION(NetMulticast, Reliable)
		void RPC_All_EquipCloth(AClothingBaseClass * cloth);

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float traceDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float FpsTraceDistance;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector GunOffset;

	UFUNCTION()
		UCameraComponent* GetActiveCam();

	/** Sound to play each time we fire */
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Sound)
	class USoundBase* FireSound;*/

	UFUNCTION()
		void PlayWeaponFireSound();


	UFUNCTION()
		void PlayWeaponReloadSound();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		class UAnimMontage* Pistol_ReloadAnimation_TP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		class UAnimMontage* Pistol_ReloadAnimation_FP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		class UAnimMontage* Rifle_ReloadAnimation_TP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		class UAnimMontage* Rifle_ReloadAnimation_FP;



	UFUNCTION(NetMulticast, Reliable)
		void PlayAnimation_TP(UAnimMontage* anim);

	UFUNCTION(NetMulticast, Reliable)
		void WeaponReloadAnim_All();

	UFUNCTION(Client, Reliable)
		void PlayAnimation_FP(UAnimMontage* anim);

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		class UAnimMontage* PistolEquipAnimation_FP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		class UAnimMontage* PistolEquipAnimation_TP;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		class UAnimMontage* RifleEquipAnimation_FP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		class UAnimMontage* RifleEquipAnimation_TP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		class UAnimMontage* PistolHolsterAnimation_FP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		class UAnimMontage* PistolHolsterAnimation_TP;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		class UAnimMontage* RifleHolsterAnimation_FP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		class UAnimMontage* RifleHolsterAnimation_TP;

	UPROPERTY(Replicated)
		class AWeaponBaseClass* WeaponBuffer;

	UFUNCTION()
		void WaitForMontageToEndAndEquip();


	UFUNCTION(Client, Reliable)
		void AddPitchRecoil(const float value);
	UFUNCTION(Client, Reliable)
		void AddYawRecoil(const float value);
	UPROPERTY()
		FTimerHandle MontageHandler;

	UFUNCTION()
		void PrintToScreen(FString text);

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
		bool bCanFire;

	UPROPERTY()
		AMyGameStateBase * GS;

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void SetCanFire(bool b);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void AttachDetachToHandSocket(bool b);

	UFUNCTION(Netmulticast, Reliable)
		void RPC_All_AttachDetachToHandSocket(bool b);

	UFUNCTION(Client, Reliable)
		void RPC_Client_AttachDetachToHandSocket(bool b);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = Movement)
		bool bIsJumping;

	UPROPERTY(Replicated)
		bool bIsFPS;

	UFUNCTION(Server, Reliable, WithValidation)
		void SetbIsFPS(bool b);

	UFUNCTION(Server, Reliable, WithValidation)
		void SetbIsADS(bool b);

	UFUNCTION(Server, Reliable, WithValidation)
		void SetbUseHandIK(bool b);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
		class USoundBase* FootstepSFX;

	UFUNCTION(BlueprintCallable)
		void PlayFootstepSFX();

	UFUNCTION(Server, Reliable, WithValidation)
		void TryToAddItemToInventory(FItemData item);

	UFUNCTION(Client, Reliable)
		void ShowTimer(bool b);

	UFUNCTION(Client, Reliable)
		void WaitForLevelStreaming();

	UFUNCTION()
		virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser) override;

	UFUNCTION(Client, Reliable)
		void OnHit_Client();

protected:
	UPROPERTY()
		bool _c;

	UPROPERTY()
		AMyPlayerState* PS;

	UPROPERTY()
		AController* LastDMGInstigator;

	/** Fires a projectile. */
	void OnFire();

	/** Weapon Booleans **/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WeaponVars, Replicated)
		bool bPistolEquipped;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WeaponVars, Replicated)
		bool bRifleEquipped;

	UFUNCTION()
		void StopFiring();

	UFUNCTION(Server, Reliable, WithValidation)
		void StopFiring_Server();

	UFUNCTION()
		void ChangeFireMode();
	UFUNCTION(Server, Reliable, WithValidation)
		void ChangeFireMode_Server();

	UPROPERTY(Replicated, BlueprintReadOnly)
		bool bIsADS;
	UFUNCTION()
		void AimDownSights();
	UFUNCTION(Server, Reliable, WithValidation)
		void AimDownSights_Server();
	UFUNCTION()
		void StopADS();
	UFUNCTION(Server, Reliable, WithValidation)
		void StopADS_Server();

	/*Pitch for aim offset*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AimOffset, Replicated)
		float fPitch;

	////Sprinting stuff

	UFUNCTION()
		void Sprint();
	UFUNCTION()
		void StopSprinting();

	////end of sprinting stuff

	/*	Health stuff	*/

	UPROPERTY(ReplicatedUsing = "OnRep_Health")
		float fHealth;

	UFUNCTION()
		void OnRep_Health();

	UFUNCTION(Server, Reliable, WithValidation)
		void DecreaseHealth(float amount);

	UFUNCTION(Server, Reliable, WithValidation)
		void AddHealth(float amount);

	UFUNCTION(Server, Reliable, WithValidation)
		void Death(APlayerController* pc);

	UFUNCTION(NetMulticast, Reliable)
		void Death_All();

	UFUNCTION()
		void DeathRep();

	UFUNCTION(Client, Reliable)
		void Death_Client();

	UPROPERTY(ReplicatedUsing = "OnRep_IsDead")
		bool bIsDead;

	UFUNCTION()
		void OnRep_IsDead();

	/*Hit stuff/collision body*/

	UFUNCTION(Server, Reliable, WithValidation)
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);



	/**/		/**/

	////*	Crouching stuff	*///

	UPROPERTY(ReplicatedUsing = "OnRep_Crouch", VisibleAnywhere, BlueprintReadOnly, Category = Movement)
		bool bIsCrouching;
	UFUNCTION(Client, Reliable)
		void RPC_Client_Crouch();
	UFUNCTION(Server, Reliable, WithValidation)
		void RPC_Server_Crouch();

	////end of crouching stuff


	/////Rpc for jumping

	UFUNCTION(Server, Reliable, WithValidation)
		void RPC_Server_TrytoJump();

	UFUNCTION(NetMulticast, Reliable)
		void RPC_All_TrytoJump();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void RPC_Server_TrytoStopJump();

	UFUNCTION(NetMulticast, Reliable)
		void RPC_All_TrytoStopJump();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void ResetCharState();

	UFUNCTION()
		void TrytoJump();

	UFUNCTION()
		void TrytoStopJump();
	UPROPERTY()
		bool b_CanJump;
	UFUNCTION()
		void ResetJump();
	////////end of jumping rpc

	///////Fire related stuff
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerFire(FVector origin);

	UFUNCTION(NetMulticast, Reliable)
		void FireAll();

	UFUNCTION(Server, Reliable, WithValidation)
		void RPC_Server_Sprint();

	UFUNCTION(NetMulticast, Reliable)
		void RPC_All_Sprint();

	UFUNCTION(Server, Reliable, WithValidation)
		void RPC_Server_StopSprinting();

	UFUNCTION(NetMulticast, Reliable)
		void RPC_All_StopSprinting();
	/* end of fire related stuff */

	/* Action stuff */
	UFUNCTION()
		void Action();

	UFUNCTION()
		void OnRep_WeaponInvChanged();

	UFUNCTION()
		void OnRep_WeaponRef();

	UFUNCTION(Server, Reliable, WithValidation)
		void RPC_Server_Action(FVector origin);

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	UPROPERTY()
		float WalkSpeed;
	UPROPERTY()
		float RunSpeed;
	UPROPERTY()
		float SprintSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float CrouchSpeed;

	UPROPERTY()
		FTimerHandle TimerHandle;
	UPROPERTY()
		FTimerHandle TimerHandle2;

	UPROPERTY(Replicated)
		UCameraComponent* ActiveCamera;

	UPROPERTY()
		bool bTimeline_isReverse;

	UPROPERTY()
		bool bCanAim;

	UFUNCTION()
		FHitResult TraceFromCamera(UCameraComponent* currentCamera, float distance);

	UFUNCTION()
		FHitResult TraceFromOrigin(FVector origin, float distance);

	UFUNCTION()
		void HUDTrace();

	UFUNCTION()
		void FakeTick();

	UFUNCTION()
		void ChangeCurrentCamera();

	UFUNCTION()
		void ChangeCamera(bool b);

	UFUNCTION(Server, Reliable, WithValidation)
		void SetActiveCam(bool bFPS);

	UFUNCTION(Server, Reliable, WithValidation)
		void SetActiveCamComp(UCameraComponent* cam);

	UFUNCTION()
		int32 CheckCurrentWeaponSlot();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void UnequipAttachment(FItemData attachment, AWeaponBaseClass* weapon, int32 slot, bool isEquipped);

	UPROPERTY(ReplicatedUsing= "OnRep_CurrentWeapon", BlueprintReadOnly)
		AWeaponBaseClass* CurrentWeapon;

	UFUNCTION()
		void OnRep_CurrentWeapon();

	UFUNCTION(Client, Reliable)
		void ChangeWeaponsVisibility(bool b);

	UFUNCTION(Server, Reliable, WithValidation)
		void EquipWeapon(AWeaponBaseClass* weapon);

	UFUNCTION(Client, Reliable)
		void RPC_EquipWeapon_Client(AWeaponBaseClass* weapon);

	UFUNCTION(BlueprintCallable)
		void KeyboardEvent();

	UPROPERTY()
		int WeaponSlotBuffer;

	UFUNCTION(Server, Reliable, WithValidation)
		void HolsterCurrentWeapon();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void HolsterWeapon(AWeaponBaseClass* weapon);

	UFUNCTION(Server, Reliable, WithValidation)
		void HolsterWeapon_server(AWeaponBaseClass* weapon);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void DropItem(FItemData itemData, int32 index);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void DropWeapon(int32 slot, int32 index);

	UFUNCTION()
		FHitResult TraceDown();

	UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable)
		void VFX_EjectBulletCase();

	UFUNCTION(NetMulticast, Unreliable)
		void VFX_EjectBulletCase_ALL();

	UFUNCTION(Server, Reliable, WithValidation)
		void SetWeaponAimingFalse();

	UFUNCTION(Server, Reliable, WithValidation)
		void SetWeaponAimingTrue();

	UPROPERTY(ReplicatedUsing = "OnRep_WeaponInvChanged", VisibleAnywhere, BlueprintReadOnly)
		TArray<AWeaponBaseClass*> WeaponInventory;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		TArray<FItemData> Inventory;

	UFUNCTION()
		void ShowHideInventory();

	UFUNCTION()
		void InitWeaponInventory();

	UFUNCTION(BlueprintCallable)
		void InitClothing();

	UFUNCTION(Server, Reliable, WithValidation)
		void InitClothing_Server();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void InitClothing_All();

	UFUNCTION()
		void AddWeaponToInventory(AWeaponBaseClass* weapon);

	UFUNCTION()
		bool CanPickupWeapon(TSubclassOf<AWeaponBaseClass> weapon);

	UFUNCTION()
		bool CanPickupThisItem(APickupBaseClass* pickup);

	UFUNCTION()
		void EquipWeaponSlot1();

	UFUNCTION()
		void EquipWeaponSlot2();

	UFUNCTION()
		void EquipWeaponSlot3();

	UFUNCTION()
		void Walk();
	UFUNCTION()
		void StopWalking();


protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P_Arms_lower; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};