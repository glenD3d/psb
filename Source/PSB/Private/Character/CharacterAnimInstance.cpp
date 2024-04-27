// Fill out your copyright notice in the Description page of Project Settings.

/* Includes */
#include "Character/CharacterAnimInstance.h"
#include "Character/PSB_Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CustomMovementComponent.h"
/* Once we include the KismetMathLibrary, we can use functions on it. Contains Static Functions. */
#include "Kismet/KismetMathLibrary.h"

/* Anything that happens in the parent version is executed here. */
void UCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	/* Similar to Bleuprints and getting the Pawn Owner, we need to cast it PSB_Character. Unreal has it's own cast
	function, which is different than regular C++.*/
	PSB_Character = Cast<APSB_Character>(TryGetPawnOwner());
	/* Check to see if it is not a null ptr. If it is not a nullptr then we can access member functions. */
	if (PSB_Character)
	{
		/* Is there a function that we can access? If so, let's initialize. */
		PSB_CharacterMovement = PSB_Character->GetCharacterMovement();
		// For Climbing system
		CustomMovementComponent = PSB_Character->GetCustomMovementComponent();
		
	}
}

/* Same as above, this is an override of a parent function. */
void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (!PSB_Character || !CustomMovementComponent) return;

	/* Get the character movement component.*/
	if (PSB_CharacterMovement)
	{
		/* If it is not null, then access the Velocity. This will be used to set the ground speed by returning the Vector Length.
		VSizeXY is the Vector Length function in Blueprints. Now we set the Ground Speed using this function.*/		
		GroundSpeed = UKismetMathLibrary::VSizeXY(PSB_CharacterMovement->Velocity);
		bIsFalling = PSB_CharacterMovement->IsFalling();
		IsCrouching = PSB_CharacterMovement->IsCrouching();
		// Character state Enum variable for switching animations when holding a weapon. 
		CharacterState = PSB_Character->GetCharacterState();
	}

	// For the climbing system
	GetGroundSpeed();
	GetAirSpeed();
	GetShouldMove();
	GetIsFalling();
	GetIsClimbing();
	GetClimbVelocity();
}

void UCharacterAnimInstance::GetGroundSpeed()
{
	GroundSpeed = UKismetMathLibrary::VSizeXY(PSB_Character->GetVelocity());
}

void UCharacterAnimInstance::GetAirSpeed()
{
	AirSpeed = PSB_Character->GetVelocity().Z;
}

void UCharacterAnimInstance::GetShouldMove()
{
	bShouldMove = CustomMovementComponent->GetCurrentAcceleration().Size() > 0 && GroundSpeed > 5.f && !bIsFalling;
}

void UCharacterAnimInstance::GetIsFalling()
{
	bIsFalling = CustomMovementComponent->IsFalling();
}

void UCharacterAnimInstance::GetIsClimbing()
{
	bIsClimbing = CustomMovementComponent->IsClimbing();
}

void UCharacterAnimInstance::GetClimbVelocity()
{
	ClimbVelocity = CustomMovementComponent->GetUnrotatedClimbVelocity();
}