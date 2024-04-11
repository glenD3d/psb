#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	// Example - Variable name is ECS_Unequipped and dispay name in UE is "Unequipped".
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "EquippedOneHandedWeapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "EquippedTwoHandedWeapon")
};