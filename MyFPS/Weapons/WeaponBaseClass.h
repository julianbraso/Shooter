// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBaseClass.h"
#include "WeaponIncludes.h"
#include "ItemIncludes.h"
#include "WeaponBaseClass.generated.h"

//class AItemBaseClass;
class AWeaponAttachmentBaseClass;

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	WT_Pistol UMETA(DisplayName = "Pistol"),
	WT_AR UMETA(DisplayName = "Assault Rifle"),
	WT_SMG UMETA(DisplayName = "SubMachineGun"),
	WT_Shotgun UMETA(DisplayName = "Shotgun")
};

UENUM(BlueprintType)
enum class EWeaponFireMode : uint8 {
	FM_Single UMETA(DisplayName = "Single"),
	FM_Auto UMETA(DisplayName = "Auto")
};



USTRUCT(BlueprintType)
struct FWeaponData {
	GENERATED_BODY();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enum)
		EWeaponType WeaponType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FName WeaponName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 WeaponID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class AItemBaseClass> ItemClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ItemIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Damage;
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
		TSubclassOf<class AMyFPSProjectile> ProjectileClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ProjectileVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		float FireRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Recoil)
		float HipRecoil;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Recoil)
		float AimingRecoil;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BulletSpread)
		float HipBulletSpread;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BulletSpread)
		float AimingBulletSpread;

	/*		Bullet Handling		*/
	UPROPERTY(BlueprintReadOnly)
		int32 CurrentAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo)
		int32 MaxMagAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo)
		int32 DefaultMagAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo)
		EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		EWeaponFireMode FireMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		bool CanChangeFireMode;
};

UCLASS()
class MYFPS_API AWeaponBaseClass : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBaseClass();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FP_Mesh)
		class USkeletalMeshComponent* SK_FP_Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TP_Mesh)
		class USkeletalMeshComponent* SK_TP_Mesh;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FWeaponData WeaponData;
	UFUNCTION(Server,Reliable,WithValidation)
		void Fire(FVector origin);
	UPROPERTY()
		FVector lastOrigin;

	UFUNCTION()
		void FireAuto();

	UFUNCTION(Client, Reliable)
		void Fire_Client();

	UFUNCTION(NetMulticast, Reliable)
		void Fire_All();

	UPROPERTY()
		bool isFiring = false;
	UPROPERTY(EditAnywhere)
		class UParticleSystem* MuzzleFx;

	UPROPERTY(EditAnywhere)
		class UParticleSystem* CaseFx;

	UPROPERTY(EditAnywhere)
		class USoundBase* CaseHitSound;

	UFUNCTION()
		void OnParticleCol(const FName EventName, float EmitterTime, int ParticleTime, const FVector Location, const FVector Velocity, const FVector Direction, const FVector Normal, const FName BoneName, UPhysicalMaterial * PhysMat);

	UFUNCTION()
		void PlayCaseHitSound(FVector loc);

	UFUNCTION()
		void CallAnimation(UAnimMontage* montageToPlay);

	UFUNCTION()
		void InterruptAllAnimation();

	UPROPERTY(Replicated,BlueprintReadWrite)
		bool bIsLoaded;

	UFUNCTION()
		void SetVisibility(bool b);

	UPROPERTY(EditAnywhere, Category = IronSight)
		bool bNeedsAttachableIronSights;

	UPROPERTY(Replicated,EditAnywhere,Category = Attachments)
		TSubclassOf<class AWeaponAttachmentBaseClass> IronSightsAttachment;
	/*UPROPERTY(Replicated, EditAnywhere, Category = Attachments)
		TSubclassOf<class AWeaponAttachmentBaseClass> SupressorAttachment;
	UPROPERTY(Replicated, EditAnywhere, Category = Attachments)
		TSubclassOf<class AWeaponAttachmentBaseClass> GripAttachment;*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly,ReplicatedUsing = "OnRep_Sight")
		class AWeaponSightAttachment* SightAttachment;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,ReplicatedUsing = "OnRep_Grip")
		class AWeaponGripAttachment* GripAttachment;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Supressor)
		class AWeaponSupressorAttachment* SupressorAttachment;

	UFUNCTION()
		bool HasSight();

	UFUNCTION()
		void OnRep_Sight();

	UFUNCTION()
		void OnRep_Grip();

	UFUNCTION()
		void OnRep_Supressor();

	UPROPERTY(Replicated)
		AWeaponAttachmentBaseClass* AttachmentSpawn;

	UPROPERTY()
		FVector ironSightsOgLoc;

	UFUNCTION(BlueprintCallable,Server,Reliable,WithValidation)
		void EquipAttachment(TSubclassOf<AWeaponAttachmentBaseClass> attachment,bool isEquipped);

	UFUNCTION(BlueprintCallable,Client, Reliable)
		void EquipAttachment_Client(TSubclassOf<AWeaponAttachmentBaseClass> attachment, bool isEquipped);

	UFUNCTION()
		void EquipIronSights();

	UFUNCTION(Client,Reliable)
		void EquipSights_Client();

	UFUNCTION(Server, Reliable,WithValidation)
		void EquipIronSights_Server();

	UFUNCTION(NetMulticast,Reliable)
		void EquipIronSights_All();

	UFUNCTION()
		void UnequipIronSights();

	UFUNCTION()
		void InitializeWeapon();
	

	/*Attachments bools*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = Attachments)
		bool bCanHaveSights;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attachments)
		bool bCanHaveScope;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attachments)
		bool bCanHaveSupressor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attachments)
		bool bCanHaveGrip;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = IronSight)
		float IronSightOffsetRoll;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = IronSight)
		float IronSightOffsetYaw;

	UFUNCTION()
		void CallFireAnimation();

	UFUNCTION()
		void CallReloadAnimation();

	UFUNCTION()
		bool CheckIfItCanFire();

	UFUNCTION()
		void StopFiring();

	UFUNCTION()
		void StartFiring();

	UFUNCTION()
		void ChangeFireMode();
	UFUNCTION()
		USoundBase* GetFireSound();

	UFUNCTION()
		USoundBase* GetReloadSound();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void UnequipAttachment(FItemData attachment, AWeaponBaseClass* weapon, int32 slot,bool isequipped);

	UFUNCTION(Client, Reliable)
		void UnequipAttachment_Client(FItemData attachment, AWeaponBaseClass* weapon, int32 slot, bool isequipped);

	UPROPERTY(Replicated,VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* ADSCameraComponent;

	UFUNCTION(Server, Reliable, WithValidation)
		void SetAimingTrue();
	UFUNCTION(Server,Reliable,WithValidation)
		void SetAimingFalse();

	UFUNCTION(Client,Reliable)
		void SetSightAiming(bool b);

	UFUNCTION()
		void ReloadAmount(int32 amount);

	UFUNCTION()
		int32 NeedsAmmoAmount();

	UFUNCTION()
		void EjectBulletCase();

	UFUNCTION(Client,Reliable)
		void EjectBulletCase_Client();

	UFUNCTION()
		UAnimMontage * GetCharReloadAnimation(bool FPS);

	UFUNCTION(NetMulticast,Reliable)
		void EjectMag();

	UPROPERTY(Replicated)
		AActor* MagToEject;

	UFUNCTION(Server,Reliable,WithValidation)
		void SpawnMag(bool b);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USoundBase* SupressedSound;

	UPROPERTY(Replicated)
		USoundBase* CurrentSound;
	
	UPROPERTY()
		FTimerHandle StatusHandle;

	UFUNCTION()
		void HandleWeaponStatus();

	UPROPERTY()
		float Recoil;
	UPROPERTY()
		float Accuracy;

	/** First person camera */


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* ReloadSoundEmpty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* ReloadSoundFull;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		class UAnimMontage* FireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		class UAnimMontage* CharFireAnimation_FP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		class UAnimMontage* CharFireAnimation_ADS;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		class UAnimMontage* CharFireAnimation_TP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		class UAnimMontage* ReloadAnimationFull;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		class UAnimMontage* ReloadAnimationEmpty;

	UPROPERTY()
		class UAnimMontage* ReloadAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		class UAnimMontage* CharReloadAnimation_Empty_FP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		class UAnimMontage* CharReloadAnimation_Empty_TP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		class UAnimMontage* CharReloadAnimation_Full_FP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		class UAnimMontage* CharReloadAnimation_Full_TP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		TSubclassOf<class AActor> MagClass;	

	UPROPERTY(Replicated)
		class AMyFPSCharacter* charRef;
		
	UPROPERTY(Replicated)
		bool bCanFire;

	UFUNCTION(Server,Reliable,WithValidation)
		void SetCanFireTrue();

	UFUNCTION(Server, Reliable, WithValidation)
		void SetCanFireFalse();

	UFUNCTION()
		void DecreaseAmmo();

	UPROPERTY()
		FTimerHandle FireRateHandle;

	UPROPERTY()
		FTimerHandle AutoFireModeHandle;

	UPROPERTY()
		float FieldOfView;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DefaultFieldOfView = 70.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
		float GetFieldOfView();
};
