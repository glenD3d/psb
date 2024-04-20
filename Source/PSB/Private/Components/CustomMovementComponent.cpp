// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CustomMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/PSB_Character.h"

void UCustomMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TraceClimbableSurfaces();
	TraceFromEyeHeight(100.f);
}

#pragma region ClimbTraces

// Initial sphere trace to detect World Static Objects in front of character, World Static is set in the Custom Character Movement component.
TArray<FHitResult> UCustomMovementComponent::DoCapsuleTraceMultiByObject(const FVector& Start, const FVector& End, bool bShowDebugShape)
{
	TArray<FHitResult> OutCapsuleTraceHitResults;
	// Accessing the capsule trace component from KismetSystemLibrary
	UKismetSystemLibrary::CapsuleTraceMultiForObjects(
		this, 
		Start, 
		End, 
		ClimbCapsuleTraceRadius, 
		ClimbCapsuleHalfHeight, 
		ClimbableSurfaceTraceTypes, 
		false, TArray<AActor*>(), 
		bShowDebugShape? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
		OutCapsuleTraceHitResults,
		false
	);

	return OutCapsuleTraceHitResults;

}

// Trace function that can be used multiple times
FHitResult UCustomMovementComponent::DoLineTraceSingleByObject(const FVector& Start, const FVector& End, bool bShowDebugShape)
{
	FHitResult OutHit;
	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		Start,
		End,
		ClimbableSurfaceTraceTypes,
		false, TArray<AActor*>(),
		bShowDebugShape ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
		OutHit,
		false
	);
	return OutHit;
}

#pragma endregion

#pragma region ClimbCore

void UCustomMovementComponent::TraceClimbableSurfaces()
{
	// Function to create climbable surfaces
	// Negated Right Vector, this was originally GetForwardvector and changed to GetRightVector for the side scroller movement.
	const FVector RightVector = UpdatedComponent->GetRightVector();

	const FVector NegatedRightVector = RightVector * -1.0f;
	const FVector StartOffset = NegatedRightVector * 30.f;
	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector();

	DoCapsuleTraceMultiByObject(Start, End, true);
}

// By Default the variable TraceStartOffset has a value of 0. 
void UCustomMovementComponent::TraceFromEyeHeight(float TraceDistance, float TraceStartOffset)
{
	// Local Variable to get the component location
	const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();

	// To trace from eye height, we need to know how much we need to offset to start trace
	const FVector EyeHeightOffset = UpdatedComponent->GetUpVector() * (CharacterOwner->BaseEyeHeight + TraceStartOffset);

	const FVector Start = ComponentLocation + EyeHeightOffset;
	// This was originally GetForwardVector and changed to GetRightVector for the side scroller movement.
	const FVector RightVector = UpdatedComponent->GetRightVector();
	const FVector NegatedRightVector = RightVector * -1.0f;
	//const FVector End = Start + UpdatedComponent->GetRightVector() * TraceDistance;
	const FVector End = Start + NegatedRightVector * TraceDistance;

	DoLineTraceSingleByObject(Start, End, true);
}

#pragma endregion
