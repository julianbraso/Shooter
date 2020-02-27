#pragma once

UENUM(BlueprintType)
enum class EAmmoType : uint8 {
	None UMETA(DisplayName = "None"),
	AT_9 UMETA(DisplayName = "9mm"),
	AT_45 UMETA(DisplayName = "45mm"),
	AT_556 UMETA(DisplayName = "5.56mm"),
	AT_762 UMETA(DisplayName = "7.62mm")
};