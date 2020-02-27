// Fill out your copyright notice in the Description page of Project Setting3+6s.

#include "WeaponBaseClass.h"
#include "MyFPSProjectile.h"
#include "MyFPSCharacter.h"
#include "UnrealNetwork.h"
#include "Animation/AnimInstance.h"
#include "ParticleDefinitions.h"
#include "ItemBaseClass.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/GameFramework/ProjectileMovementComponent.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "WeaponAttachmentBaseClass.h"
#include "WeaponSightAttachment.h"
#include "WeaponGripAttachment.h"
#include "WeaponSupressorAttachment.h"
#include "Runtime/Engine/Classes/Components/SceneCaptureComponent2D.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"


// Sets default values
AWeaponBaseClass::AWeaponBaseClass()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SK_FP_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh FP"));
	SK_FP_Mesh->SetOnlyOwnerSee(true);
	SK_FP_Mesh->LightingChannels.bChannel0 = false;
	SK_FP_Mesh->LightingChannels.bChannel1 = true;
	SK_FP_Mesh->bCastInsetShadow = true;
	
	SK_TP_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh TP"));
	SK_TP_Mesh->SetOwnerNoSee(true);
	SK_TP_Mesh->bCastHiddenShadow = true;

	ADSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ADS camera"));
	ADSCameraComponent->SetupAttachment(SK_FP_Mesh);

	SetAimingFalse();
	
	bCanFire = true;

	bReplicates = true;

	bAlwaysRelevant = true;

}

void AWeaponBaseClass::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AWeaponBaseClass, WeaponData);
	DOREPLIFETIME(AWeaponBaseClass, MagToEject);
	DOREPLIFETIME(AWeaponBaseClass, bCanFire);
	DOREPLIFETIME(AWeaponBaseClass, bIsLoaded);
	DOREPLIFETIME(AWeaponBaseClass, ADSCameraComponent);
	DOREPLIFETIME(AWeaponBaseClass, SightAttachment);
	DOREPLIFETIME(AWeaponBaseClass, AttachmentSpawn);
	DOREPLIFETIME(AWeaponBaseClass, charRef);
	DOREPLIFETIME(AWeaponBaseClass, CurrentSound);
	DOREPLIFETIME(AWeaponBaseClass, SupressorAttachment);
}

void AWeaponBaseClass::Fire_Client_Implementation()
{
	if (SupressorAttachment)
	{
		return;
	}
	UParticleSystemComponent* emitter;
	if (charRef->bIsFPS)
	{
		emitter = UGameplayStatics::SpawnEmitterAttached(MuzzleFx, SK_FP_Mesh, FName("Muzzle"), SK_FP_Mesh->GetSocketLocation(TEXT("Muzzle")), SK_FP_Mesh->GetSocketRotation(TEXT("Muzzle")), SK_FP_Mesh->GetSocketTransform(TEXT("Muzzle")).GetScale3D(),EAttachLocation::KeepWorldPosition, true);
	}
	else {
		emitter = UGameplayStatics::SpawnEmitterAttached(MuzzleFx, SK_TP_Mesh, FName("Muzzle"), SK_TP_Mesh->GetSocketLocation(TEXT("Muzzle")), SK_TP_Mesh->GetSocketRotation(TEXT("Muzzle")), SK_TP_Mesh->GetSocketTransform(TEXT("Muzzle")).GetScale3D(), EAttachLocation::KeepWorldPosition, true);
	}
}

void AWeaponBaseClass::FireAuto()
{
	Fire(lastOrigin);
}

void AWeaponBaseClass::OnParticleCol(const FName EventName, float EmitterTime, int ParticleTime, const FVector Location, const FVector Velocity, const FVector Direction, const FVector Normal, const FName BoneName, UPhysicalMaterial * PhysMat)
{
	PlayCaseHitSound(Location);
	//charRef->PrintToScreen("Particle collided...");
}

void AWeaponBaseClass::PlayCaseHitSound(FVector loc)
{
	if (CaseHitSound) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), CaseHitSound, loc);
	}
}

void AWeaponBaseClass::CallAnimation(UAnimMontage* montageToPlay)
{
	SK_FP_Mesh->GetAnimInstance()->Montage_Play(montageToPlay);
	SK_TP_Mesh->GetAnimInstance()->Montage_Play(montageToPlay);
}

void AWeaponBaseClass::InterruptAllAnimation()
{

	if (!SK_FP_Mesh->GetAnimInstance()) {
		charRef->PrintToScreen("No Animation BP on weapon...");
		return;
	}

	if (SK_TP_Mesh->GetAnimInstance()->Montage_IsPlaying(NULL))
	{
		SK_TP_Mesh->GetAnimInstance()->Montage_Stop(NULL);
	}
	if (SK_FP_Mesh->GetAnimInstance()->Montage_IsPlaying(NULL))
	{
		SK_FP_Mesh->GetAnimInstance()->Montage_Stop(NULL);
	}
}

void AWeaponBaseClass::SetVisibility(bool b)
{
	charRef->PrintToScreen("Weapon: SetVisibility Called");

	if (b)
	{
		SK_FP_Mesh->SetVisibility(true, true);
		SK_TP_Mesh->SetVisibility(false, true);
		SK_FP_Mesh->SetOwnerNoSee(false);
		SK_TP_Mesh->SetOwnerNoSee(true);
		if (SightAttachment)
		{
			SightAttachment->Mesh_FP->SetVisibility(true, true);
			SightAttachment->Mesh_TP->SetVisibility(false, true);
		}
		if (GripAttachment)
		{
			GripAttachment->Mesh_FP->SetVisibility(true, true);
			GripAttachment->Mesh_TP->SetVisibility(false, true);
		}
		if (SupressorAttachment)
		{
			SupressorAttachment->Mesh_FP->SetVisibility(true, true);
			SupressorAttachment->Mesh_TP->SetVisibility(false, true);
		}
	}
	else {
		SK_FP_Mesh->SetVisibility(false, true);
		SK_TP_Mesh->SetVisibility(true, true);
		SK_FP_Mesh->SetOwnerNoSee(true);
		SK_TP_Mesh->SetOwnerNoSee(false);
		if (SightAttachment)
		{
			SightAttachment->Mesh_FP->SetVisibility(false, true);
			SightAttachment->Mesh_TP->SetVisibility(true, true);
		}
		if (GripAttachment)
		{
			GripAttachment->Mesh_FP->SetVisibility(false, true);
			GripAttachment->Mesh_TP->SetVisibility(true, true);
		}
		if (SupressorAttachment)
		{
			SupressorAttachment->Mesh_FP->SetVisibility(false, true);
			SupressorAttachment->Mesh_TP->SetVisibility(true, true);
		}
	}
}

void AWeaponBaseClass::CallFireAnimation()
{
	float a = SK_FP_Mesh->GetAnimInstance()->Montage_Play(FireAnimation);
	if (a<= 0.0f)
	{
		charRef->PrintToScreen("Failed to play weapon anim");
	}
	else {
		//charRef->PrintToScreen(FString::SanitizeFloat(a));
	}
	
	SK_TP_Mesh->GetAnimInstance()->Montage_Play(FireAnimation);
	if (charRef->GetIsADS())
	{
		//charRef->PrintToScreen("Is in ADS");
		if (CharFireAnimation_ADS) {
			charRef->PlayAnimation_FP(CharFireAnimation_ADS);
		}
	} else {
		//charRef->PrintToScreen("Not in ADS");
		if (CharFireAnimation_FP) {
			charRef->PlayAnimation_FP(CharFireAnimation_FP);
		}
	}
	
	if (CharFireAnimation_TP) {
		charRef->PlayAnimation_TP(CharFireAnimation_TP);
	}
}

void AWeaponBaseClass::CallReloadAnimation()
{
	if (WeaponData.CurrentAmmo > 0)
	{
		ReloadAnimation = ReloadAnimationFull;
	}
	else {
		ReloadAnimation = ReloadAnimationEmpty;											/*	ON PISTOL THIS DOESNT MATTER, BECAUSE THE ANIMBP TAKES CARE OF THE LOADED/UNLOADED WEAPON STATUS	*/	
	}
	if (ReloadAnimation) {
		if (SK_FP_Mesh->GetAnimInstance() == NULL)
		{
			//GLog->Log("No anim instance on weapon...");
			charRef->PrintToScreen("No anim instance on weapon...");
			return;
		}
		if (SK_FP_Mesh->GetAnimInstance()->Montage_Play(ReloadAnimation) == 0.f) {
			GLog->Log("Failed to play weapon animation");
		}
		SK_TP_Mesh->GetAnimInstance()->Montage_Play(ReloadAnimation);
		charRef->PlayWeaponReloadSound();
	}
}

bool AWeaponBaseClass::CheckIfItCanFire()
{
	return bCanFire;
}

void AWeaponBaseClass::StopFiring()
{
	GetWorldTimerManager().ClearTimer(AutoFireModeHandle);
}

void AWeaponBaseClass::StartFiring()
{

}

void AWeaponBaseClass::ChangeFireMode()
{
	if (WeaponData.CanChangeFireMode) {
		if (WeaponData.FireMode == EWeaponFireMode::FM_Single)
		{
			WeaponData.FireMode = EWeaponFireMode::FM_Auto;
		}
		else if (WeaponData.FireMode == EWeaponFireMode::FM_Auto)
		{
			WeaponData.FireMode = EWeaponFireMode::FM_Single;
		}
	}
}

USoundBase * AWeaponBaseClass::GetFireSound()
{
	return CurrentSound;
}

USoundBase * AWeaponBaseClass::GetReloadSound()
{
	if (WeaponData.CurrentAmmo >0)
	{
		return ReloadSoundFull;
	} else
	{
		return ReloadSoundEmpty;
	}
}

void AWeaponBaseClass::UnequipAttachment_Client_Implementation(FItemData attachment, AWeaponBaseClass * weapon, int32 slot, bool isequipped)
{
	
	AWeaponAttachmentBaseClass * a = attachment.AttachmentClass->GetDefaultObject<AWeaponAttachmentBaseClass>();
	if (!a)
	{
		charRef->PrintToScreen("Weapon Unequip Attachment: ERROOOOOORR");
	}

	if (a->AttachmentType == EAttachmentType::AT_Scope || a->AttachmentType == EAttachmentType::AT_Sight)
	{
		FieldOfView = DefaultFieldOfView;
	}
	if (!bNeedsAttachableIronSights)
	{
		charRef->PrintToScreen("UNEQUIP ATTACHMENT CLIENT");
		//charRef->PrintToScreen(ironSightsOgLoc.ToString());
		//charRef->PrintToScreen(ADSCameraComponent->RelativeLocation.ToString());
		ADSCameraComponent->SetRelativeLocation(ironSightsOgLoc);
		//ADSCameraComponent->SetWorldLocation(ironSightsOgLoc);
		charRef->RefreshADSCamPos();
	}
	charRef->CallChangeWeapVisib();
}

bool AWeaponBaseClass::UnequipAttachment_Validate(FItemData attachment, AWeaponBaseClass * weapon, int32 slot, bool isequipped) {
	return true;
}

void AWeaponBaseClass::UnequipAttachment_Implementation(FItemData attachment, AWeaponBaseClass * weapon, int32 slot, bool isequipped)
{

	if (!isequipped)
	{
		return;
	}

	charRef->PrintToScreen("Weapon Unequip Attachment: INIT");
	//charRef->PrintToScreen(SightAttachment->GetName());

	if (!attachment.AttachmentClass) {
		charRef->PrintToScreen("Weapon Unequip Attachment: ERROR no hay clase attachment");
		return;
	}

	AWeaponAttachmentBaseClass * a = attachment.AttachmentClass->GetDefaultObject<AWeaponAttachmentBaseClass>();
	if (!a)
	{
		charRef->PrintToScreen("Weapon Unequip Attachment: ERROOOOOORR");
	}
	
	if (!a->WeaponTypesAccepted.Contains(WeaponData.WeaponType))
	{
		charRef->PrintToScreen("Weapon Unequip Attachment: InvalidType");
		charRef->BuildClientInventory();
		return;
	}

	if (a->AttachmentType == EAttachmentType::AT_Scope || a->AttachmentType == EAttachmentType::AT_Sight)
	{
		AWeaponSightAttachment* s = Cast<AWeaponSightAttachment>(a);
		if (!s)
		{
			return;
		}
		charRef->PrintToScreen(s->GetName());
		if (SightAttachment)
		{
			if (s->GetClass() != SightAttachment->GetClass()) {
				charRef->PrintToScreen("attachment to unequip not equal to the one equipped");
				return;
			}
			SightAttachment->Destroy();
			SightAttachment = nullptr;
		}
		if (IronSightsAttachment && !SightAttachment)
		{
			EquipAttachment(IronSightsAttachment, false);
			return;
		}
		else {
			ADSCameraComponent->SetRelativeLocation(ironSightsOgLoc);
		}
	}
	else if(a->AttachmentType == EAttachmentType::AT_Supressor) {
		AWeaponSupressorAttachment* s = Cast<AWeaponSupressorAttachment>(a);
		if (!s)
		{
			return;
		}
		if (SupressorAttachment)
		{
			if (s->GetClass() != SupressorAttachment->GetClass())
			{
				charRef->PrintToScreen("attachment to unequip not equal to the one equipped");
				return;
			}
			CurrentSound = FireSound;
			SupressorAttachment->Destroy();
			SupressorAttachment = nullptr;
		}
		//charRef->PrintToScreen("Weapon Unequip Attachment: Attachment type not correct");
	}



	charRef->PrintToScreen("Weapon Unequip Attachment: INIT2");
	charRef->TryToAddItemToInventory(attachment);
	charRef->BuildClientInventory();
	UnequipAttachment_Client(attachment, weapon, slot, isequipped);
	FieldOfView = DefaultFieldOfView;
	//charRef->CallChangeWeapVisib();
}

bool AWeaponBaseClass::SetAimingTrue_Validate() {
	return true;
}

void AWeaponBaseClass::SetAimingTrue_Implementation()
{
	Accuracy = WeaponData.AimingBulletSpread;
	Recoil = WeaponData.AimingRecoil;
	if (SightAttachment)
	{
		SetSightAiming(true);
	}
}

bool AWeaponBaseClass::SetAimingFalse_Validate() {
	return true;
}

void AWeaponBaseClass::SetAimingFalse_Implementation()
{
	Accuracy = WeaponData.HipBulletSpread;
	Recoil = WeaponData.HipRecoil;
	if (SightAttachment)
	{		
		SetSightAiming(false);
	}
}

void AWeaponBaseClass::SetSightAiming_Implementation(bool b)
{
	if (SightAttachment)
	{
		//charRef->PrintToScreen("prendiendo scope");
		SightAttachment->SetAiming(b);
	}
}

void AWeaponBaseClass::ReloadAmount(int32 amount)
{
	WeaponData.CurrentAmmo += amount;
	if (WeaponData.CurrentAmmo > WeaponData.MaxMagAmmo)
	{
		WeaponData.CurrentAmmo = WeaponData.MaxMagAmmo;
	}
}


// Called when the game starts or when spawned
void AWeaponBaseClass::BeginPlay()
{
	Super::BeginPlay();
	CurrentSound = FireSound;
	SetAimingFalse();
	FieldOfView = DefaultFieldOfView;
	SK_FP_Mesh->SetOnlyOwnerSee(true);
	SK_FP_Mesh->SetOwnerNoSee(false);
	SK_TP_Mesh->SetOwnerNoSee(true);
	charRef = Cast<AMyFPSCharacter>(GetOwner());
	charRef->PrintToScreen(ADSCameraComponent->RelativeLocation.ToString());
	//charRef->CallChangeWeapVisib();
	ironSightsOgLoc = ADSCameraComponent->RelativeLocation;
	if (charRef) {
		//charRef->PrintToScreen("Success! char ref on weapon inited");
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "Failed to cast to char from weapon");
	}
	if (HasAuthority())
	{
		//+96ToComponent(SK_FP_Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale,TEXT("ADSCamSocket"));
		GetWorldTimerManager().SetTimer(StatusHandle, this, &AWeaponBaseClass::HandleWeaponStatus, 0.06f, true);

		if (bNeedsAttachableIronSights)
		{
			EquipIronSights();
		}
	}
}

bool AWeaponBaseClass::HasSight()
{
	if (!IronSightsAttachment && SightAttachment)
	{
		//charRef->PrintToScreen("no iron sight class but a sight is equipped");
		return true;
	} else
	if (SightAttachment && IronSightsAttachment && SightAttachment->GetClass() != IronSightsAttachment.Get())
	{
		//charRef->PrintToScreen("iron sight class present but different sight equipped");
		//charRef->PrintToScreen("Sight: "+SightAttachment->GetClass()->GetName());
		//charRef->PrintToScreen("Iron: " + IronSightsAttachment.Get()->GetName());
		return true;
	}
	else {
		//charRef->PrintToScreen("else this must be an iron sight");
		return false;
	}
}


void AWeaponBaseClass::OnRep_Sight()
{
	if (!SightAttachment)
	{
		charRef->PrintToScreen("Null sight attachment replicated");
		//EquipIronSights_Server();
		return;
	}
	if (!charRef->IsLocallyControlled())
	{
		charRef->PrintToScreen("this character is not locally controlled!");
		SightAttachment->Mesh_TP->AttachToComponent(this->SK_TP_Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("SightSocket"));
		return;
	}
		charRef->PrintToScreen("Weapon sight replicated");
		charRef->PrintToScreen(SightAttachment->GetName());
		EquipSights_Client();
		charRef->CallChangeWeapVisib();
}

void AWeaponBaseClass::OnRep_Grip()
{
}

void AWeaponBaseClass::OnRep_Supressor()
{
	if (!SupressorAttachment)
	{
		charRef->PrintToScreen("Null supressor attachment...");
		return;
	}
	if (!charRef->IsLocallyControlled())
	{
		SupressorAttachment->Mesh_TP->AttachToComponent(this->SK_TP_Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("SupressorSocket"));
		return;
	}
	charRef->PrintToScreen("Weapon supressor replicated");
	charRef->PrintToScreen(SupressorAttachment->GetName());
	SupressorAttachment->Mesh_FP->AttachToComponent(this->SK_FP_Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("SupressorSocket"));
	SupressorAttachment->Mesh_TP->AttachToComponent(this->SK_TP_Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("SupressorSocket"));
	charRef->CallChangeWeapVisib();
}

void AWeaponBaseClass::EquipAttachment_Client_Implementation(TSubclassOf<AWeaponAttachmentBaseClass> attachment, bool isEquipped) {
	
}

bool AWeaponBaseClass::EquipAttachment_Validate(TSubclassOf<AWeaponAttachmentBaseClass> attachment, bool isEquipped) {
	return true;
}

void AWeaponBaseClass::EquipAttachment_Implementation(TSubclassOf<AWeaponAttachmentBaseClass> attachment, bool isEquipped)
{
	charRef->PrintToScreen("equip attachment called");
		if (!attachment)
		{
			charRef->PrintToScreen("No attachment passed on equipattachment...");
			return;
		}

		if (isEquipped)
		{
			return;
		}

		if (!attachment->GetDefaultObject<AWeaponAttachmentBaseClass>()->WeaponTypesAccepted.Contains(WeaponData.WeaponType))
		{
			charRef->PrintToScreen("Invalid weapon type");
			charRef->BuildClientInventory();
			return;
		}

		UWorld* world = GetWorld();
		if (world)
		{
			AttachmentSpawn = world->SpawnActorDeferred<AWeaponAttachmentBaseClass>(attachment, FTransform(), this, Instigator, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
			AttachmentSpawn->FinishSpawning(FTransform(FTransform()));
		}
		if (AttachmentSpawn->AttachmentType == EAttachmentType::AT_Sight || AttachmentSpawn->AttachmentType == EAttachmentType::AT_Scope)		////if sight
		{
			if (SightAttachment)
			{
				//SightAttachment->Destroy();
				//SightAttachment = nullptr;
				if (IronSightsAttachment && SightAttachment->IsA(IronSightsAttachment)) {
					SightAttachment->Destroy();
					SightAttachment = nullptr;
				}
				else {
					if (!SightAttachment->ItemClass)
					{
						charRef->PrintToScreen("Noitemclass");
						return;
					}
					charRef->PrintToScreen("Already has sight...");
					UnequipAttachment(SightAttachment->ItemClass->GetDefaultObject<AItemBaseClass>()->ItemData, this, 0, true);
					charRef->PrintToScreen("Unequipped sight!");
				}
			}

			AWeaponSightAttachment* s = Cast<AWeaponSightAttachment>(AttachmentSpawn);
			if (s)
			{
					charRef->PrintToScreen("Equip: Cast correct");
					charRef->PrintToScreen("Equip: "+s->GetName());
					s->SetOwner(GetOwner());
					s->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));
					//s->Mesh_FP->AttachToComponent(SK_FP_Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("SightSocket"));
					s->Mesh_FP->AttachToComponent(SK_FP_Mesh, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), TEXT("SightSocket"));
					s->Mesh_TP->AttachToComponent(SK_TP_Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false), TEXT("SightSocket"));
					SightAttachment = s;
					SightAttachment->SetOwner(GetOwner());
					//ADSCameraComponent->SetRelativeLocation(SightAttachment->ADSCameraComponent->RelativeLocation);
					ADSCameraComponent->SetWorldLocation(SightAttachment->ADSCameraComponent->GetComponentLocation());
					charRef->RefreshADSCamPos();
					//SightAttachment->SceneCaptureComp->AttachToComponent(charRef->GetADSCam(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
					FieldOfView = s->FieldOfView;
				}
				else {
					charRef->PrintToScreen("Cast failed");
				}
		}
		else if (AttachmentSpawn->AttachmentType == EAttachmentType::AT_Supressor)
		{
			if (SupressorAttachment)
			{
				charRef->PrintToScreen("Already has suppressor...");
				return;
			}
			AWeaponSupressorAttachment* s = Cast<AWeaponSupressorAttachment>(AttachmentSpawn);
			if (s)
			{
				charRef->PrintToScreen("Supressor: Cast correct");
				charRef->PrintToScreen("Equip: " + s->GetName());
				s->SetOwner(GetOwner());
				s->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));
				s->Mesh_FP->AttachToComponent(SK_FP_Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("SupressorSocket"));
				s->Mesh_TP->AttachToComponent(SK_TP_Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false), TEXT("SupressorSocket"));
				SupressorAttachment = s;
				SupressorAttachment->SetOwner(GetOwner());
				/* CODE RELEVANT TO TAKE ACTION WHEN SUPPRESSOR EQUIPPED */
				CurrentSound = SupressedSound;
			}
			else {
				charRef->PrintToScreen("Cast failed");
			}
		}
		else if (AttachmentSpawn->AttachmentType == EAttachmentType::AT_Grip)
		{
			if (GripAttachment)
			{
				charRef->PrintToScreen("Already has grip...");
				return;
			}
			AWeaponGripAttachment* s = Cast<AWeaponGripAttachment>(AttachmentSpawn);
			if (s)
			{
				charRef->PrintToScreen("Cast correct");
				s->SetOwner(GetOwner());
				s->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));
				s->Mesh_FP->AttachToComponent(SK_FP_Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripSocket"));
				s->Mesh_TP->AttachToComponent(SK_TP_Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false), TEXT("GripSocket"));
				GripAttachment = s;
				GripAttachment->SetOwner(GetOwner());
				/* CODE RELEVANT TO TAKE ACTION WHEN GRIP EQUIPPED */

			}
			else {
				charRef->PrintToScreen("Cast failed");
			}
		}
		charRef->EquipAttachment(attachment);
		EquipAttachment_Client_Implementation(attachment, isEquipped);
		//charRef->CallChangeWeapVisib();
}

void AWeaponBaseClass::EquipIronSights() {
	if (IronSightsAttachment)
	{
		UWorld* world = GetWorld();
		if (world)
		{
			AWeaponAttachmentBaseClass * attachment = world->SpawnActorDeferred<AWeaponAttachmentBaseClass>(IronSightsAttachment,FTransform(), this, Instigator, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
			attachment->FinishSpawning(FTransform(FTransform()));
			AWeaponSightAttachment* s = Cast<AWeaponSightAttachment>(attachment);
			if (s)
			{
				charRef->PrintToScreen("EquipIronsights!");
				s->SetOwner(GetOwner());
				s->AttachToComponent(RootComponent,FAttachmentTransformRules(EAttachmentRule::SnapToTarget,false));
				s->Mesh_FP->AttachToComponent(SK_FP_Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("SightSocket"));
				//s->Mesh_FP->SetRelativeScale3D(FVector(1));
				s->Mesh_TP->AttachToComponent(SK_TP_Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false), TEXT("SightSocket"));
				SightAttachment = s;
				SightAttachment->SetOwner(GetOwner());
				//ADSCameraComponent->SetRelativeLocation(SightAttachment->ADSCameraComponent->RelativeLocation);
				ADSCameraComponent->SetWorldLocation(SightAttachment->ADSCameraComponent->GetComponentLocation());
				charRef->RefreshADSCamPos();
				//SightAttachment->SceneCaptureComp->AttachToComponent(charRef->GetADSCam(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
				//EquipAttachment(SightSpawn);g
				/*EquipIronSights_Client(s);
				EquipIronSights_All(s);*/
				//charRef->CallChangeWeapVisib();
			}
			else {
				charRef->PrintToScreen("Cast failed");
			}
		}	
	}
}

bool AWeaponBaseClass::EquipIronSights_Server_Validate() {
	return true;
}

void AWeaponBaseClass::EquipIronSights_Server_Implementation()
{
	EquipIronSights();
	EquipIronSights_All();
	SightAttachment->SetOwner(charRef);
}

void AWeaponBaseClass::EquipSights_Client_Implementation()
{
	if (!SightAttachment)
	{
		charRef->PrintToScreen("No sightattachment on this actor!");
		return;
	}
	//ADSCameraComponent->SetRelativeLocation(SightAttachment->ADSCameraComponent->RelativeLocation);
	SightAttachment->SetOwner(charRef);
	//SightAttachment->SceneCaptureComp->AttachToComponent(charRef->GetADSCam(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
	//SightAttachment->SceneCaptureComp->SetRelativeRotation(FRotator());
	charRef->PrintToScreen("Attached sight to adscam");
	//SightAttachment->SetAiming(true);
//	SightAttachment->SetAiming(false);
	//SightAttachment->Mesh_FP->AttachToComponent(SK_FP_Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("SightSocket"));
	SightAttachment->Mesh_FP->AttachToComponent(SK_FP_Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("SightSocket"));
	SightAttachment->Mesh_TP->AttachToComponent(SK_TP_Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("SightSocket"));
	//ADSCameraComponent->SetWorldLocation(SightAttachment->ADSCameraComponent->GetComponentLocation());

	ADSCameraComponent->SetRelativeLocation(GetActorTransform().InverseTransformPosition(SightAttachment->ADSCameraComponent->GetComponentLocation()));  //(SightAttachment->ADSCameraComponent->GetComponentLocation());

	charRef->RefreshADSCamPos();
	FieldOfView = SightAttachment->FieldOfView;
	charRef->CallChangeWeapVisib();
	/*if (bNeedsAttachableIronSights)
	{
		EquipIronSights_Server();
	}*/
}

void AWeaponBaseClass::EquipIronSights_All_Implementation()
{
	if (!SightAttachment)
	{
		charRef->PrintToScreen("ALL: No sightattachment on this actor!");
		return;
	}
	//ADSCameraComponent->SetRelativeLocation(SightAttachment->ADSCameraComponent->RelativeLocation);
	ADSCameraComponent->SetWorldLocation(SightAttachment->ADSCameraComponent->GetComponentLocation());
	SightAttachment->Mesh_TP->AttachToComponent(SK_TP_Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("SightSocket"));
}

void AWeaponBaseClass::UnequipIronSights()
{
	if (SightAttachment)
	{
		SightAttachment->Destroy();
		SightAttachment = nullptr;
	}
}

void AWeaponBaseClass::InitializeWeapon()
{
	if (SightAttachment)
	{
		EquipAttachment(SightAttachment->ItemClass->GetDefaultObject<AItemBaseClass>()->ItemData.AttachmentClass,false);
	}
	if (GripAttachment)
	{
		EquipAttachment(SightAttachment->ItemClass->GetDefaultObject<AItemBaseClass>()->ItemData.AttachmentClass, false);
	}
	if (SupressorAttachment)
	{
		EquipAttachment(SightAttachment->ItemClass->GetDefaultObject<AItemBaseClass>()->ItemData.AttachmentClass, false);
	}
}

void AWeaponBaseClass::HandleWeaponStatus() {
	if (WeaponData.CurrentAmmo <= 0)
	{
		bIsLoaded = false;
	}
	else {
		bIsLoaded = true;
	}
}

bool AWeaponBaseClass::Fire_Validate(FVector origin) {
	return true;
}

void AWeaponBaseClass::Fire_All_Implementation() {
	if (!charRef)
	{
		return;
	}

	 if (!charRef->IsLocallyControlled())
	{
		 if (!SupressorAttachment)
		 {
			 UParticleSystemComponent* e;
			 e = UGameplayStatics::SpawnEmitterAttached(MuzzleFx, SK_TP_Mesh, FName("Muzzle"), SK_TP_Mesh->GetSocketLocation(TEXT("Muzzle")), SK_TP_Mesh->GetSocketRotation(TEXT("Muzzle")), SK_TP_Mesh->GetSocketTransform(TEXT("Muzzle")).GetScale3D(), EAttachLocation::KeepWorldPosition, true);
		 }
	}
	CallFireAnimation();
	charRef->PlayWeaponFireSound();
}

void AWeaponBaseClass::Fire_Implementation(FVector origin) {
	if (HasAuthority()) {
		lastOrigin = charRef->GetActiveCam()->GetComponentLocation();
		if (bCanFire)
		{

			if (WeaponData.CurrentAmmo <= 0)
			{
				return;
			}

			if (WeaponData.FireMode == EWeaponFireMode::FM_Auto)
			{
				if (!AutoFireModeHandle.IsValid())
				{
					GetWorldTimerManager().SetTimer(AutoFireModeHandle, this, &AWeaponBaseClass::FireAuto, WeaponData.FireRate, true);
				}
			}

			bCanFire = false;
			GetWorldTimerManager().SetTimer(FireRateHandle, this, &AWeaponBaseClass::SetCanFireTrue, WeaponData.FireRate, false);
			if (WeaponData.ProjectileClass != NULL)
			{
				UWorld* const World = GetWorld();
				if (World != NULL)
				{
					if (charRef) {
						const FRotator SpawnRotation = FRotator(charRef->GetControlRotation().Pitch + FMath::RandRange(Accuracy*-1,Accuracy), charRef->GetControlRotation().Yaw + FMath::RandRange(Accuracy*-1, Accuracy), charRef->GetControlRotation().Roll);		/*	const FRotator SpawnRotation = charRef->GetControlRotation()	working shit */
						const FVector SpawnLocation = charRef->GetActiveCam()->GetComponentLocation() + SpawnRotation.Vector() * 50.f;				///charRef->GetActiveCam()->GetComponentLocation()
						FTransform T = FTransform(SpawnRotation, SpawnLocation, FVector(1, 1, 1));
						FActorSpawnParameters ActorSpawnParams;
						ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
						ActorSpawnParams.Instigator = Instigator;
						ActorSpawnParams.Owner = GetOwner();
						AMyFPSProjectile* proj;
						// spawn the projectile at the muzzle
						proj = World->SpawnActorDeferred<AMyFPSProjectile>(WeaponData.ProjectileClass, T, GetOwner(), GetInstigator(), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding);
						if (proj)
						{
							//proj->SetOwner(this);
							proj->Instigator = GetInstigator();
							proj->GetProjectileMovement()->MaxSpeed = WeaponData.ProjectileVelocity;
							proj->GetProjectileMovement()->InitialSpeed = WeaponData.ProjectileVelocity;
							proj->ProjectileDamage = WeaponData.Damage;
							proj->SetActorScale3D(FVector(1, 1, 1));
							proj->SetOwner(GetOwner());
							proj->FinishSpawning(T);
						}
						Fire_All();
						Fire_Client();
						charRef->AddPitchRecoil(Recoil);
						charRef->AddYawRecoil(Recoil);
						charRef->RefreshADSCamPos();
						DecreaseAmmo();
						//emitter->component->AttachTo(SK_FP_Mesh, "Muzzle", EAttachLocation::SnapToTarget);
					}
				}
			}
			else { charRef->PrintToScreen("No projectile class on weapon..."); }
		}
	}
}

void AWeaponBaseClass::EjectBulletCase() {
	if (Role != ROLE_AutonomousProxy)
	{
		//charRef->PrintToScreen("Eject bullet case!");
		UParticleSystemComponent* caseEmitter;
		caseEmitter = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), CaseFx, SK_TP_Mesh->GetSocketLocation(TEXT("CaseEject")), SK_TP_Mesh->GetSocketRotation(TEXT("CaseEject")), FVector(1.5f), true);

		if (caseEmitter)
		{
			caseEmitter->OnParticleCollide.AddDynamic(this, &AWeaponBaseClass::OnParticleCol);
		}
	}
	else {
		
	}
}

void AWeaponBaseClass::EjectBulletCase_Client_Implementation()
{
	UParticleSystemComponent* caseEmitter;
	if (charRef->bIsFPS)
	{
		caseEmitter = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), CaseFx, SK_FP_Mesh->GetSocketLocation(TEXT("CaseEject")), SK_FP_Mesh->GetSocketRotation(TEXT("CaseEject")), FVector(1.5f), true);
	}
	else {
		caseEmitter = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), CaseFx, SK_TP_Mesh->GetSocketLocation(TEXT("CaseEject")), SK_TP_Mesh->GetSocketRotation(TEXT("CaseEject")), FVector(1.5f), true);
	}
	if (caseEmitter)
	{
		caseEmitter->OnParticleCollide.AddDynamic(this, &AWeaponBaseClass::OnParticleCol);
	}
}

UAnimMontage * AWeaponBaseClass::GetCharReloadAnimation(bool FPS)
{
	if (FPS)
	{
		if (WeaponData.CurrentAmmo > 0)
		{
			return CharReloadAnimation_Full_FP;
		}
		else {
			return CharReloadAnimation_Empty_FP;
		}
	}
	else
	{
		if (WeaponData.CurrentAmmo > 0)
		{
			return CharReloadAnimation_Full_TP;
		}
		else {
			return CharReloadAnimation_Empty_TP;
		}
	}
}

void AWeaponBaseClass::EjectMag_Implementation()
{
	if (MagToEject)
	{
		TArray<UStaticMeshComponent*> StaticComps;
		MagToEject->GetComponents<UStaticMeshComponent>(StaticComps);
		for (int i = 0; i < StaticComps.Num(); i++)
		{
			StaticComps[i]->SetSimulatePhysics(true);
		}
		MagToEject->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}else{ 
		//charRef->PrintToScreen("No mag eject on variable!");
	}
}

bool AWeaponBaseClass::SpawnMag_Validate(bool b){
	return true;
}

void AWeaponBaseClass::SpawnMag_Implementation(bool b)
{
	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		if (!MagClass)
		{
			charRef->PrintToScreen("No mag class");
			return;
		}
		if (b)
		{
			MagToEject = World->SpawnActor<AActor>(MagClass, SK_FP_Mesh->GetSocketTransform(FName("Mag")));
			MagToEject->AttachToComponent(SK_FP_Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("Mag")));
			FTransform T = FTransform(SK_FP_Mesh->GetSocketRotation(FName(TEXT("Mag"))), SK_FP_Mesh->GetSocketLocation(FName(TEXT("Mag"))), FVector(1, 1, 1));
			MagToEject->SetActorTransform(T);
		}
		else {
			MagToEject = World->SpawnActor<AActor>(MagClass, SK_TP_Mesh->GetSocketTransform(FName("Mag")));
			MagToEject->AttachToComponent(SK_TP_Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("Mag")));
			FTransform T = FTransform(SK_TP_Mesh->GetSocketRotation(FName(TEXT("Mag"))), SK_TP_Mesh->GetSocketLocation(FName(TEXT("Mag"))), FVector(1, 1, 1));
			MagToEject->SetActorTransform(T);
		}	
			
	}
}

bool AWeaponBaseClass::SetCanFireTrue_Validate() {
	return true;
}

void AWeaponBaseClass::SetCanFireTrue_Implementation()
{
	bCanFire = true;
}

bool AWeaponBaseClass::SetCanFireFalse_Validate() {
	return true;
}

void AWeaponBaseClass::SetCanFireFalse_Implementation()
{
	bCanFire = false;
}

void AWeaponBaseClass::DecreaseAmmo()
{
	WeaponData.CurrentAmmo -= 1;
}

int32 AWeaponBaseClass::NeedsAmmoAmount()
{
	if (WeaponData.CurrentAmmo == WeaponData.MaxMagAmmo)
	{
		return 0;
	}
	return WeaponData.MaxMagAmmo - WeaponData.CurrentAmmo;
}

// Called every frame
void AWeaponBaseClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AWeaponBaseClass::GetFieldOfView()
{
	return FieldOfView;
}
