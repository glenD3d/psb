// Mandirigma Games


#include "AbilitySystem/AttributeSets/MG_AttributeSetBase.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

void UMG_AttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::ClampAngle(GetHealth(), 0.f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxMovementSpeedAttribute())
	{
		ACharacter* OwningCharacter = Cast<ACharacter>(GetOwningActor());
		UCharacterMovementComponent* CharacterMovement = OwningCharacter ? OwningCharacter->GetCharacterMovement() : nullptr;		

		if (CharacterMovement)
		{
			const float MaxSpeed = GetMaxMovementSpeed();

			CharacterMovement->MaxWalkSpeed = MaxSpeed;
		}
	}
}

void UMG_AttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMG_AttributeSetBase, Health, OldHealth);
}

void UMG_AttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMG_AttributeSetBase, MaxHealth, OldMaxHealth);
}

void UMG_AttributeSetBase::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMG_AttributeSetBase, Stamina, OldStamina);
}

void UMG_AttributeSetBase::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMG_AttributeSetBase, MaxStamina, OldMaxStamina);
}

void UMG_AttributeSetBase::OnRep_MaxMovementSpeed(const FGameplayAttributeData& OldMaxMovementSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMG_AttributeSetBase, MaxMovementSpeed, OldMaxMovementSpeed);
}


void UMG_AttributeSetBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UMG_AttributeSetBase, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMG_AttributeSetBase, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMG_AttributeSetBase, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMG_AttributeSetBase, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMG_AttributeSetBase, MaxMovementSpeed, COND_None, REPNOTIFY_Always);
}