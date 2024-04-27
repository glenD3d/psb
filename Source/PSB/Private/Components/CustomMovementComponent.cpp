// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CustomMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/PSB_Character.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PSB/DebugMacros.h"

void UCustomMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningPlayerAnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();
	if (OwningPlayerAnimInstance)
	{
		OwningPlayerAnimInstance->OnMontageEnded.AddDynamic(this, &UCustomMovementComponent::OnClimbMontageEnded);
		OwningPlayerAnimInstance->OnMontageBlendingOut.AddDynamic(this, &UCustomMovementComponent::OnClimbMontageEnded);
	}
}

void UCustomMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/*TraceClimbableSurfaces();
	TraceFromEyeHeight(100.f);*/
}

void UCustomMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	if (IsClimbing())
	{
		bOrientRotationToMovement = false;
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(48.f);
	}

	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == ECustomMovementMode::MOVE_Climb)
	{
		bOrientRotationToMovement = true;
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(96.f);

		const FRotator CurrentRotation = UpdatedComponent->GetRelativeRotation();
		//Debug::Print(TEXT("Rotation:") + CurrentRotation.ToString(), FColor::Green, 1);
		if (CurrentRotation.Yaw == 0.f)
		{

			// Set character ability to stand again after climbing the wall. This is to ensure he is not spider-man.
			const FRotator DirtyRotation = UpdatedComponent->GetComponentRotation();
			const FRotator CleanStandRotation = FRotator(0.f, DirtyRotation.Yaw, 0.f);
			const FRotator NegateRotation = FRotator(0.f, CleanStandRotation.Yaw * -0.1, 0.f);
			UpdatedComponent->SetRelativeRotation(NegateRotation);

			StopMovementImmediately();
		}
		else
		{
			// Set character ability to stand again after climbing the wall. This is to ensure he is not spider-man.
			const FRotator DirtyRotation = UpdatedComponent->GetComponentRotation();
			const FRotator CleanStandRotation = FRotator(0.f, DirtyRotation.Yaw, 0.f);
			UpdatedComponent->SetRelativeRotation(CleanStandRotation);

			StopMovementImmediately();
		}
	}

	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

void UCustomMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	if (IsClimbing())
	{
		PhysClimb(deltaTime, Iterations);
	}

	Super::PhysCustom(deltaTime, Iterations);
}

float UCustomMovementComponent::GetMaxSpeed() const
{
	if (IsClimbing())
	{
		return MaxClimbSpeed;
	}
	else
	{
		return Super::GetMaxSpeed();
	}
}

float UCustomMovementComponent::GetMaxAcceleration() const
{
	if (IsClimbing())
	{
		return MaxClimbAcceleration;
	}
	else
	{
		return Super::GetMaxAcceleration();
	}
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
			PlayClimbMontage(IdleToClimbMontage);
		}
		else
		{
			Debug::Print(TEXT("Can NOT Start Climbing"));
		}
	}
	else
	{
		// Stop climbing
		StopClimbing();
	}
}

bool UCustomMovementComponent::CanStartClimbing()
{

	if (IsFalling()) return false;
	if (!TraceClimbableSurfaces()) return false;
	if (!TraceFromEyeHeight(100.f).bBlockingHit) return false;

	return true;

}

void UCustomMovementComponent::StartClimbing()
{
	SetMovementMode(MOVE_Custom, ECustomMovementMode::MOVE_Climb);
}

void UCustomMovementComponent::StopClimbing()
{
	SetMovementMode(MOVE_Falling);
}

// This is similar to the Tick function, it will will be run every frame.
void UCustomMovementComponent::PhysClimb(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	/* Process all the climbable surfaces info */
	TraceClimbableSurfaces();
	ProcessClimbableSurfaceInfo();

	//Debug function to check the floor, keep this in to check floor later with proper animations.
	//if (CheckHasReachedFloor())
	//{
	//	Debug::Print(TEXT("Floor Reached"),FColor::Green,1);
	//}
	//else
	//{
	//	Debug::Print(TEXT("Floor Not Reached"), FColor::Red, 1);
	//}
	/* Check if we should stop climbing */
	if (CheckShouldStopClimbing() || CheckHasReachedFloor())
	{
		StopClimbing();
	}

	RestorePreAdditiveRootMotionVelocity();

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		// MaxBreakClimbDeceleration default value is set in the header file.
		//Define the Max Climb Speed and Acceleration.
		CalcVelocity(deltaTime, 0.f, true, MaxBreakClimbDeceleration);
	}

	ApplyRootMotionToVelocity(deltaTime);

	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const FVector Adjusted = Velocity * deltaTime;
	FHitResult Hit(1.f);

	// This is the function that actually moves our character.
	// Handle climb rotation
	SafeMoveUpdatedComponent(Adjusted, GetClimbRotation(deltaTime), true, Hit);

	if (Hit.Time < 1.f)
	{
		//adjust and try again
		HandleImpact(Hit, deltaTime, Adjusted);
		SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}
	/* Snap movement to climbable surfaces */
	SnapMovementToClimbableSurfaces(deltaTime);
}

// This function is run every frame, it is important that the two member variables are Zeroed out.
void UCustomMovementComponent::ProcessClimbableSurfaceInfo()
{
	CurrentClimbableSurfaceLocation = FVector::ZeroVector;
	CurrentClimbableSurfaceNormal = FVector::ZeroVector;

	// This is the array
	if (ClimbableSurfacesTracedResults.IsEmpty()) return;

	// Loop through though the ClimbableSurfacesTracedResults
	for (const FHitResult& TracedHitResult : ClimbableSurfacesTracedResults)
	{
		CurrentClimbableSurfaceLocation += TracedHitResult.ImpactPoint;
		CurrentClimbableSurfaceNormal += TracedHitResult.ImpactNormal;
	}

	// Get the average location of all possible locations inside the Array
	CurrentClimbableSurfaceLocation /= ClimbableSurfacesTracedResults.Num();

	// Normalize the normal
	CurrentClimbableSurfaceNormal = CurrentClimbableSurfaceNormal.GetSafeNormal();

}

// Checks the top face with the climbing face normal and gets the Dot Product.
bool UCustomMovementComponent::CheckShouldStopClimbing()
{
	if (ClimbableSurfacesTracedResults.IsEmpty()) return true;

	const float DotResult = FVector::DotProduct(CurrentClimbableSurfaceNormal, FVector::UpVector);
	const float DegreeDiff = FMath::RadiansToDegrees(FMath::Acos(DotResult));

	if (DegreeDiff <= 60.f)
	{
		return true;
	}

	//Debug::Print(TEXT("Degree Diff:") + FString::SanitizeFloat(DegreeDiff), FColor::Cyan, 1);

	return false;

}

// CHecks the floor to exit the climb animation.
bool UCustomMovementComponent::CheckHasReachedFloor()
{
	const FVector DownVector = -UpdatedComponent->GetUpVector();
	const FVector StartOffset = DownVector * 50.f;

	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + DownVector;

	TArray<FHitResult> PossibleFloorHits = DoCapsuleTraceMultiByObject(Start, End, true);

	if (PossibleFloorHits.IsEmpty()) return false;

	for (const FHitResult& PossibleFloorHit:PossibleFloorHits)
	{
		// When we reach the floor, we will most likely have two hits. One is the Climbable surface and one is the floor.
		// We will filter these hits.

		const bool bFloorReached = FVector::Parallel(-PossibleFloorHit.ImpactNormal, FVector::UpVector) && GetUnrotatedClimbVelocity().Z<-50.f;

		if (bFloorReached)
		{
			return true;
		}
	}

	return false;
}

FQuat UCustomMovementComponent::GetClimbRotation(float DeltaTime)
{
	const FRotator CurrentRotation = UpdatedComponent->GetRelativeRotation();
	//Debug::Print(TEXT("Rotation:") + CurrentRotation.ToString(), FColor::Green, 1);

	if (CurrentRotation.Yaw == 0.f)

	{
		const FQuat CurrentQuat = UpdatedComponent->GetComponentQuat();

		if (HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity())
		{
			return CurrentQuat;
		}

		// This function returns the face normal of the climbable surface and sets the rotation.
		// This allows the same distance from the climbable surface on either direction.
		const FQuat TargetQuat = FRotationMatrix::MakeFromX(CurrentClimbableSurfaceNormal).ToQuat();

		// Hard coded value of turn speed 5.f
		return FMath::QInterpTo(CurrentQuat, TargetQuat, DeltaTime, 5.f);
	}
	else
	{
		const FQuat CurrentQuat = UpdatedComponent->GetComponentQuat();

		if (HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity())
		{
			return CurrentQuat;
		}

		// This function returns the face normal of the climbable surface and sets the rotation.
		const FQuat TargetQuat = FRotationMatrix::MakeFromX(-CurrentClimbableSurfaceNormal).ToQuat();

		// Hard coded value of turn speed 5.f
		return FMath::QInterpTo(CurrentQuat, TargetQuat, DeltaTime, 5.f);
	}

}

// This may need an if check to check the direction of the player Yaw. 
void UCustomMovementComponent::SnapMovementToClimbableSurfaces(float DeltaTime)
{
	//MGMGMG//
	const FVector ComponentForward = UpdatedComponent->GetRightVector();
	//MGMGMG//
	const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();

	const FVector ProjectedCharacterToSurface = (CurrentClimbableSurfaceLocation - ComponentLocation).ProjectOnTo(ComponentForward);

	const FVector SnapVector = -CurrentClimbableSurfaceNormal * ProjectedCharacterToSurface.Length();

	UpdatedComponent->MoveComponent(SnapVector * DeltaTime * MaxClimbSpeed, UpdatedComponent->GetComponentQuat(), true);
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
	//const FVector RightVector = UpdatedComponent->GetRightVector();

	const FRotator CurrentRotation = UpdatedComponent->GetRelativeRotation();
	//Debug::Print(TEXT("Rotation:") + CurrentRotation.ToString(), FColor::Green, 1);

	if(CurrentRotation.Yaw == 0.f)
	{
		const FVector ForwardVector = UpdatedComponent->GetForwardVector();

		//const FVector NegatedRightVector = RightVector * -1.0f;
		const FVector NegatedForwardVector = ForwardVector * -1.0f;

		//const FVector StartOffset = NegatedRightVector * 30.f;
		const FVector NegatedStartOffset = NegatedForwardVector * 30.f;

		const FVector Start = UpdatedComponent->GetComponentLocation() + NegatedStartOffset;
		const FVector End = Start + UpdatedComponent->GetForwardVector();

		// Only call the debug function for one frame since we are calling this function every frame when we start climbing. 
		ClimbableSurfacesTracedResults = DoCapsuleTraceMultiByObject(Start, End, true);

		return !ClimbableSurfacesTracedResults.IsEmpty();
	}
	else
	{
		const FVector ForwardVector = UpdatedComponent->GetForwardVector();

		//const FVector NegatedRightVector = RightVector * -1.0f;
		const FVector NegatedForwardVector = ForwardVector * -1.0f;

		//const FVector StartOffset = NegatedRightVector * 30.f;
		const FVector StartOffset = ForwardVector * 30.f;

		const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
		const FVector End = Start + UpdatedComponent->GetForwardVector();

		// Only call the debug function for one frame since we are calling this function every frame when we start climbing. 
		ClimbableSurfacesTracedResults = DoCapsuleTraceMultiByObject(Start, End, true);

		return !ClimbableSurfacesTracedResults.IsEmpty();
	}
	//return !ClimbableSurfacesTracedResults.IsEmpty();

}

// By Default the variable TraceStartOffset has a value of 0. 
FHitResult UCustomMovementComponent::TraceFromEyeHeight(float TraceDistance, float TraceStartOffset)
{
	const FRotator CurrentRotation = UpdatedComponent->GetRelativeRotation();
	//Debug::Print(TEXT("Rotation:") + CurrentRotation.ToString(), FColor::Green, 1);

	if (CurrentRotation.Yaw == 0.f)
	{
		// Local Variable to get the component location
		const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();

		// To trace from eye height, we need to know how much we need to offset to start trace
		const FVector EyeHeightOffset = UpdatedComponent->GetUpVector() * (CharacterOwner->BaseEyeHeight + TraceStartOffset);

		const FVector Start = ComponentLocation + EyeHeightOffset;
		// This was originally GetForwardVector and changed to GetRightVector for the side scroller movement.
		const FVector ForwardVector = UpdatedComponent->GetForwardVector();
		const FVector NegatedForwardVector = ForwardVector * -1.0f;
		//const FVector End = Start + UpdatedComponent->GetRightVector() * TraceDistance;
		const FVector End = Start + NegatedForwardVector * TraceDistance;

		return DoLineTraceSingleByObject(Start, End, false);
	}
	else
	{
		// Local Variable to get the component location
		const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();

		// To trace from eye height, we need to know how much we need to offset to start trace
		const FVector EyeHeightOffset = UpdatedComponent->GetUpVector() * (CharacterOwner->BaseEyeHeight + TraceStartOffset);

		const FVector Start = ComponentLocation + EyeHeightOffset;
		// This was originally GetForwardVector and changed to GetRightVector for the side scroller movement.
		const FVector ForwardVector = UpdatedComponent->GetForwardVector();
		//const FVector NegatedForwardVector = ForwardVector * -1.0f;
		//const FVector End = Start + UpdatedComponent->GetRightVector() * TraceDistance;
		const FVector End = Start + ForwardVector * TraceDistance;

		return DoLineTraceSingleByObject(Start, End, false);
	}
}


// Character anim montage
void UCustomMovementComponent::PlayClimbMontage(UAnimMontage* MontageToPlay)
{
	if (!MontageToPlay) return;
	if (!OwningPlayerAnimInstance) return;
	if (OwningPlayerAnimInstance->IsAnyMontagePlaying()) return;

	// Set up left right montages to play based on character direction. 
	OwningPlayerAnimInstance->Montage_Play(MontageToPlay);
}

void UCustomMovementComponent::OnClimbMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == IdleToClimbMontage)
	{
		StartClimbing();
	}
}

void UCustomMovementComponent::AdjustCharacter() const
{
	const FRotator TurnCharacter = UpdatedComponent->GetComponentRotation();
	const FRotator RotateCharacter = FRotator(0.f, TurnCharacter.Yaw - 90.0f, 0.f);
	UpdatedComponent->SetRelativeRotation(RotateCharacter);
}

FVector UCustomMovementComponent::GetUnrotatedClimbVelocity() const
{
	return UKismetMathLibrary::Quat_UnrotateVector(UpdatedComponent->GetComponentQuat(), Velocity);
}
#pragma endregion
