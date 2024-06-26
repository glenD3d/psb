// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PSB_Character.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Character.h"
#include "Components/CustomMovementComponent.h"
//#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "MotionWarpingComponent.h"

#include "PSB/DebugMacros.h"

// Sets default values
APSB_Character::APSB_Character(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CustomMovementComponent = Cast<UCustomMovementComponent>(GetCharacterMovement());

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComp"));

}

// Called when the game starts or when spawned
void APSB_Character::BeginPlay()
{
	Super::BeginPlay();

	AddInputMappingContext(PSB_CharacterContext, 0);
	if (CustomMovementComponent)
	{
		CustomMovementComponent->OnEnterClimbStateDelegate.BindUObject(this, &ThisClass::OnPlayerEnterClimbState);
		CustomMovementComponent->OnExitClimbStateDelegate.BindUObject(this, &ThisClass::OnPlayerExitClimbState);
	}
}

void APSB_Character::AddInputMappingContext(UInputMappingContext* ContextToAdd, int32 InPriority)
{
	if (!ContextToAdd) return;

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(ContextToAdd, InPriority);
		}
	}
}

void APSB_Character::RemoveInputMappingContext(UInputMappingContext* ContextToAdd)
{
	if (!ContextToAdd) return;

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(ContextToAdd);
		}
	}
}

void APSB_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Move and Looking
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &APSB_Character::HandleGroundMovementInput);
		EnhancedInputComponent->BindAction(ClimbMoveAction, ETriggerEvent::Triggered, this, &APSB_Character::HandleClimbMovementInput);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APSB_Character::Look);

		// Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &APSB_Character::Crouch);

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APSB_Character::Jump);

		// Equip and Attacking
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &APSB_Character::EKeyPressed);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &APSB_Character::Attack);

		// Climbing - Changed to Started. Will only happen once if the key is down and not constant if it was Triggered.
		EnhancedInputComponent->BindAction(ClimbAction, ETriggerEvent::Started, this, &APSB_Character::OnClimbActionStarted);

		// Climbing - Changed to Started. Will only happen once if the key is down and not constant if it was Triggered.
		EnhancedInputComponent->BindAction(ClimbHopAction, ETriggerEvent::Started, this, &APSB_Character::OnClimbHopActionStarted);
	}
}

void APSB_Character::HandleGroundMovementInput(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;

	// Input is a Vector 2D
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if(Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		//const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FRotator YawRotation(0.f, 0.f, 0.f);

		// Add movment
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APSB_Character::HandleClimbMovementInput(const FInputActionValue& Value)
{
	// Input is a Vector 2D
	const FVector2D MovementVector = Value.Get<FVector2D>();

	// Get Forward Vector
	const FVector ForwardDirection = FVector::CrossProduct(-CustomMovementComponent->GetClimbableSurfaceNormal(), GetActorRightVector());

	/* Get Right Vector
	const FVector RightDirection = FVector::CrossProduct(-CustomMovementComponent->GetClimbableSurfaceNormal(), -GetActorRightVector());*/

	const FRotator CurrentRotation = Controller->GetControlRotation();
	//Debug::Print(TEXT("Rotation:") + CurrentRotation.ToString(), FColor::Green, 1);

	// The if check is for the climbing function. When the player is facing a certain direction the controls need to be the same.
	if (CurrentRotation.Yaw == 0.f)
	{
		// add movement
		AddMovementInput(ForwardDirection, -MovementVector.X);
		//AddMovementInput(RightDirection, MovementVector.X);
	}
	else
	{
		AddMovementInput(ForwardDirection, -MovementVector.X);
	}

}

void APSB_Character::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerPitchInput(LookAxisVector.Y);
	AddControllerYawInput(LookAxisVector.X);
}

void APSB_Character::OnClimbActionStarted(const FInputActionValue& Value)
{
	if (!CustomMovementComponent) return;

	if (!CustomMovementComponent->IsClimbing())
	{
		CustomMovementComponent->ToggleClimbing(true);
	}
	else
	{
		CustomMovementComponent->ToggleClimbing(false);
	}
}

void APSB_Character::OnPlayerEnterClimbState()
{
	AddInputMappingContext(ClimbMappingContext, 1);
}

void APSB_Character::OnPlayerExitClimbState()
{
	RemoveInputMappingContext(ClimbMappingContext);
}

void APSB_Character::OnClimbHopActionStarted(const FInputActionValue& Value)
{
	if (CustomMovementComponent)
	{
		CustomMovementComponent->RequestHopping();
	}
}

void APSB_Character::Crouch(const FInputActionValue& Value)
{
	if (GetMovementComponent())
	{
		GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	}
}

void APSB_Character::Jump(const FInputActionValue& Value)
{
	Super::Jump();
}

void APSB_Character::EKeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"));
		// Set Character State, it does not set the anim instance
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
		// Need to set overlapping item to a nullptr, we want to use the E Key for several other functions.
		// Before setting Equip wpn, we set it to a nullptr.
		OverlappingItem = nullptr;

		// After calling Equip function and setting the character state to EquippedOneWeaponHandedWeapon, we will set EquippedWeapon.
		// This determines that Equip Montage will only play if the character has a weapon. 
		EquippedWeapon = OverlappingWeapon;
	}
	else
	{		
		if (CanDisarm())
		{
			PlayEquipMontage(FName("Unequip"));
			CharacterState = ECharacterState::ECS_Unequipped;
			// Disable sliding of character when unequipping a weapon.
			ActionState = EActionState::EAS_EquippingWeapon;

		}
		else if (CanArm())
		{
			PlayEquipMontage(FName("Equip"));
			CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
			// Disable sliding of character when equipping a weapon.
			ActionState = EActionState::EAS_EquippingWeapon;
		}
	}
}

void APSB_Character::Attack()
{
	if (CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

bool APSB_Character::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped;
}

// This function puts the weapon away. Unequip's the weapon.
bool APSB_Character::CanDisarm()
{
	// Check to see if the player is engaged in combat and check if the player has a weapon. Also check to see if the Montage is Set.
	return ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped;
}

bool APSB_Character::CanArm()
{
	// EquippedWeapon will also check to see if it is a nullptr.
	return ActionState == EActionState::EAS_Unoccupied && CharacterState == ECharacterState::ECS_Unequipped && EquippedWeapon;
}

void APSB_Character::Disarm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void APSB_Character::Arm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void APSB_Character::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void APSB_Character::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
		// Randomly select a montage to play
		const int32 Selection = FMath::RandRange(0, 2);
		FName SectionName = FName();
		switch (Selection)
		{
		case 0:
			SectionName = FName("Attack1");
			break;
		case 1:
			SectionName = FName("Attack2");
			break;
		case 2:
			SectionName = FName("Attack3");
		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}
}

void APSB_Character::PlayEquipMontage(FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	// Check the AnimInstance and check to see if the Montage is Set.
	if (AnimInstance && EquipMontage) 
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void APSB_Character::AttackEnd()
{
	// Set attacking action state back to Unoccupied. This will allow the
	// character to attack again.
	ActionState = EActionState::EAS_Unoccupied;
}

// Called every frame
void APSB_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

