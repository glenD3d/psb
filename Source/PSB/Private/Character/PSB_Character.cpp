// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PSB_Character.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APSB_Character::APSB_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}



// Called when the game starts or when spawned
void APSB_Character::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PSB_CharacterContext, 0);
		}
	}
}

void APSB_Character::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	/*const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);*/
	const FRotator YawRotation(0.f, 0.f, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);

	//const FVector Forward = GetActorForwardVector();
	//AddMovementInput(Forward, MovementVector.Y);
	//const FVector Right = GetActorRightVector();
	//AddMovementInput(Right, MovementVector.X);
}

void APSB_Character::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerPitchInput(LookAxisVector.Y);
	AddControllerYawInput(LookAxisVector.X);
}

void APSB_Character::Crouch(const FInputActionValue& Value)
{
	if (GetMovementComponent())
	{
		GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	}
}

// Called every frame
void APSB_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APSB_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &APSB_Character::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APSB_Character::Look);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &APSB_Character::Crouch);
	}

	//PlayerInputComponent->BindAxis(FName("MoveForward"), this, &APSB_Character::MoveForward);
}

void APSB_Character::MoveForward(float Value)
{
	if (Controller && (Value != 0.f))
	{
		FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, Value);
	}
}



