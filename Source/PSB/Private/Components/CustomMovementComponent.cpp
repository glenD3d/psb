// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CustomMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/PSB_Character.h"
#include "PSB/DebugMacros.h"

void UCustomMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/*TraceClimbableSurfaces();
	TraceFromEyeHeight(100.f);*/
}

#pragma region ClimbTraces

// Initial sphere trace to detect World Static Objects in front of character, World Static is set in the Custom Character Movement component.
TArray<FHitResult> UCustomMovementComponent::DoCapsuleTraceMultiByObject(const FVector& Start, const FVector& End, bool bShowDebugShape, bool bDrawPersistentShapes)
{
	TArray<FHitResult> OutCapsuleTraceHitResults;

	EDrawDebugTrace::Type DebugTraceType = EDrawDebugTrace::None;
	if (bShowDebugShape)
	{
		DebugTraceType = EDrawDebugTrace::ForOneFrame;

		if (bDrawPersistentShapes)
		{
			DebugTraceType = EDrawDebugTrace::Persistent;
		}
	}

	// Accessing the capsule trace component from KismetSystemLibrary
	UKismetSystemLibrary::CapsuleTraceMultiForObjects(
		this, 
		Start, 
		End, 
		ClimbCapsuleTraceRadius, 
		ClimbCapsuleHalfHeight, 
		ClimbableSurfaceTraceTypes, 
		false, TArray<AActor*>(), 
		DebugTraceType,
		OutCapsuleTraceHitResults,
		false
	);

	return OutCapsuleTraceHitResults;

}

// Trace function that can be used multiple times
FHitResult UCustomMovementComponent::DoLineTraceSingleByObject(const FVector& Start, const FVector& End, bool bShowDebugShape, bool bDrawPersistentShapes)
{
	FHitResult OutHit;

	EDrawDebugTrace::Type DebugTraceType = EDrawDebugTrace::None;

	if (bShowDebugShape)
	{
		DebugTraceType = EDrawDebugTrace::ForOneFrame;

		if (bDrawPersistentShapes)
		{
			DebugTraceType = EDrawDebugTrace::Persistent;
		}
	}

	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		Start,
		End,
		ClimbableSurfaceTraceTypes,
		false, TArray<AActor*>(),
		DebugTraceType,
		OutHit,
		false
	);
	return OutHit;
}

#pragma endregion

#pragma region ClimbCore

void UCustomMovementComponent::ToggleClimbing(bool bEnableClimb)
{
	if (bEnableClimb)
	{
		if (CanStartClimbing())
		{
			// Enter climb state
			Debug::Print(TEXT("Can Start Climbing"));
		}
		else
		{
			Debug::Print(TEXT("Can NOT Start Climbing"));
		}
	}
	else
	{
		// Stop climbing
	}
}

bool UCustomMovementComponent::CanStartClimbing()
{
	if (IsFalling()) return false;
	if (!TraceClimbableSurfaces()) return false;
	if (!TraceFromEyeHeight(100.f).bBlockingHit) return false;

	return true;
}

bool UCustomMovementComponent::IsClimbing() const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == ECustomMovementMode::MOVE_Climb;
	
}

// Trace for climbable surfaces, return true if there valid surfaces, false otherwise.
bool UCustomMovementComponent::TraceClimbableSurfaces()
{
	// Function to create climbable surfaces
	// Negated Right Vector, this was originally GetForwardvector and changed to GetRightVector for the side scroller movement.
	const FVector RightVector = UpdatedComponent->GetRightVector();

	const FVector NegatedRightVector = RightVector * -1.0f;
	const FVector StartOffset = NegatedRightVector * 30.f;
	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector();

	ClimbableSurfacesTracedResults = DoCapsuleTraceMultiByObject(Start, End, true, true);

	return !ClimbableSurfacesTracedResults.IsEmpty();
}

// By Default the variable TraceStartOffset has a value of 0. 
FHitResult UCustomMovementComponent::TraceFromEyeHeight(float TraceDistance, float TraceStartOffset)
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

	return DoLineTraceSingleByObject(Start, End, true, true);
}

#pragma endregion
