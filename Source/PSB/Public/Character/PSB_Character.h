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
class UAnimMontage;
class AWeapon;

class UCustomMovementComponent;
class UMotionWarpingComponent;

UCLASS(config=Game)
class PSB_API APSB_Character : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APSB_Character(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	void MoveForward(float Value);

private:

#pragma region Components

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UCustomMovementComponent* CustomMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UMotionWarpingComponent* MotionWarpingComponent;

#pragma endregion

#pragma region Input

	void OnPlayerEnterClimbState();

	void OnPlayerExitClimbState();

	void AddInputMappingContext(UInputMappingContext* ContextToAdd, int32 InPriority);

	void RemoveInputMappingContext(UInputMappingContext* ContextToAdd);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* PSB_CharacterContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* ClimbMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ClimbMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ClimbAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ClimbHopAction;

	/* Look Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

#pragma endregion

#pragma region Callbacks

	///* Callbacks for input*/
	//void Move(const FInputActionValue& Value);

	void HandleGroundMovementInput(const FInputActionValue& Value);
	void HandleClimbMovementInput(const FInputActionValue& Value);

	/* Callbacks for input*/
	void Look(const FInputActionValue& Value);

	void OnClimbActionStarted(const FInputActionValue& Value);

	void OnClimbHopActionStarted(const FInputActionValue& Value);

	/* Callbacks for input*/
	void Crouch(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);
	void EKeyPressed();
	void Attack();

#pragma endregion

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*
	* Play Montage Functions
	*/
	void PlayAttackMontage();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();
	bool CanAttack();

	void PlayEquipMontage(FName SectionName);
	bool CanDisarm();
	bool CanArm();

	UFUNCTION(BlueprintCallable)
	void Disarm();

	UFUNCTION(BlueprintCallable)
	void Arm();

	UFUNCTION(BlueprintCallable)

	void FinishEquipping();

private:

	// This is for switching between animation poses when a weapon is unequipped or equipped.
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;


private:
	// If the item is set, then we know we are overlapping with an item.
	// It is a good idea for pointer variables to be UProperties as long as they are derived from a UObject.
	// To see it in the Details panel in the world at Runtime, give it the specifier VisibleInstanceOnly.
	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	// Won't be changed from details panel, only changed in gameplay.
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* EquippedWeapon;

	/**
	* Animation Montages
	*/

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;

public:
	// Good idea to keep setter and getter functions in there own public section.
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }

	FORCEINLINE UCustomMovementComponent* GetCustomMovementComponent() const { return CustomMovementComponent; }

	FORCEINLINE UMotionWarpingComponent* GetMotionWarpingComponent() const { return MotionWarpingComponent; }
};
