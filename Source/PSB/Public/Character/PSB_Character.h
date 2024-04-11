// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"
#include "PSB_Character.generated.h"

//Declarations//
class UInputMappingContext;
class UInputAction;
class AItem;

UCLASS()
class PSB_API APSB_Character : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APSB_Character();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


protected:

	void MoveForward(float Value);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere,Category = Input)
	UInputMappingContext* PSB_CharacterContext;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* EquipAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Crouch(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);
	void EKeyPressed();

private:

	// This is for switching between animation poses when a weapon is unequipped or equipped.
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;


private:
	// If the item is set, then we know we are overlapping with an item.
	// It is a good idea for pointer variables to be UProperties as long as they are derived from a UObject.
	// To see it in the Details panel in the world at Runtime, give it the specifier VisibleInstanceOnly.
	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

public:
	// Good idea to keep setter and getter functions in there own public section.
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
};
