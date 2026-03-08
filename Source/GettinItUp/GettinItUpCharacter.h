// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "GettinItUpCharacter.generated.h"


class UInputAction;


UCLASS(config=Game)
class AGettinItUpCharacter : public ACharacter
{
	GENERATED_BODY()
	
#pragma region Climbing
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AxeGaming, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UCapsuleComponent> LeftAxe;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AxeGaming, meta = (AllowPrivateAccess = "true"))
	FName LeftAxeComponentTagName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AxeGaming, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UCapsuleComponent> RightAxe;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AxeGaming, meta = (AllowPrivateAccess = "true"))
	FName RightAxeComponentTagName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsLeftAxeGripping = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsRightAxeGripping = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AxeGaming, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveLeftAxeAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AxeGaming, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveRightAxeAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AxeGaming, meta = (AllowPrivateAccess = "true"))
	float MaxAxeSpeed = 50.f;

	virtual void Tick(float DeltaSeconds) override;

protected:
	
	FVector2D LeftAxeAcceleration;
	
	FVector2D RightAxeAcceleration;
	
	void MoveLeftAxe(const FInputActionValue& Value);
	
	void MoveRightAxe(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void CeaseRightAxeMovement();

	UFUNCTION(BlueprintCallable)
	void CeaseLeftAxeMovement();
	
	void ApplyControlInputToAxeVelocity(float DeltaTime, FVector2D& AxeAccelerationInput, UCapsuleComponent* Axe);

#pragma endregion Climbing
	
#pragma region Generated

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

public:
	AGettinItUpCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
#pragma endregion Generated
};

