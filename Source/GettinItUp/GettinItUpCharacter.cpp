// Copyright Epic Games, Inc. All Rights Reserved.

#include "GettinItUpCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h"


//////////////////////////////////////////////////////////////////////////
// AGettinItUpCharacter

AGettinItUpCharacter::AGettinItUpCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AGettinItUpCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	LeftAxe = GetComponentsByTag(UCapsuleComponent::StaticClass(), LeftAxeComponentTagName)[0];
	RightAxe = GetComponentsByTag(UCapsuleComponent::StaticClass(), RightAxeComponentTagName)[0];
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGettinItUpCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		//Moving axes
		EnhancedInputComponent->BindAction(MoveLeftAxeAction, ETriggerEvent::Triggered, this, &AGettinItUpCharacter::MoveLeftAxe);
		EnhancedInputComponent->BindAction(MoveRightAxeAction, ETriggerEvent::Triggered, this, &AGettinItUpCharacter::MoveRightAxe);
		
		// // Old:
		//
		// // Jumping
		// EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		// EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		//
		// // Moving
		// EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGettinItUpCharacter::Move);
		//
		// // Looking
		// EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGettinItUpCharacter::Look);
	}
}

void AGettinItUpCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (LeftAxe)
	{
		ApplyControlInputToAxeVelocity(DeltaSeconds, LeftAxeAcceleration, LeftAxe.Get());
	}
	if (RightAxe)
	{
		ApplyControlInputToAxeVelocity(DeltaSeconds, RightAxeAcceleration, RightAxe.Get());
	}
}

void AGettinItUpCharacter::MoveLeftAxe(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (MovementVector.Length() > 1)
	{
		MovementVector /= MovementVector.Length();
	}

	LeftAxeAcceleration = MovementVector;
}

void AGettinItUpCharacter::MoveRightAxe(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (MovementVector.Length() > 1)
	{
		MovementVector /= MovementVector.Length();
	}

	RightAxeAcceleration = MovementVector;
}

void AGettinItUpCharacter::ApplyControlInputToAxeVelocity(float DeltaTime, FVector2D& AxeAccelerationInput, UCapsuleComponent* Axe)
{
	// const float AnalogInputModifier = (AxeAccelerationInput.SizeSquared() > 0.f ? AxeAccelerationInput.Size() : 0.f);
	// const float AdjustedMaxAxeSpeed = MaxAxeSpeed * AnalogInputModifier;
	// const bool bExceedingMaxSpeed = Axe->GetPhysicsLinearVelocity().Length() > AdjustedMaxAxeSpeed;
	// auto Velocity = Axe->GetPhysicsLinearVelocity();
	//
	// if (AnalogInputModifier > 0.f && !bExceedingMaxSpeed)
	// {
	// 	// Apply change in velocity direction
	// 	if (Velocity.SizeSquared() > 0.f)
	// 	{
	// 		// Change direction faster than only using acceleration, but never increase velocity magnitude.
	// 		const float TimeScale = FMath::Clamp(DeltaTime, 0.f, 1.f);
	// 		Axe->SetPhysicsLinearVelocity(Velocity + (FVector(AxeAccelerationInput.X, AxeAccelerationInput.Y, 0.f) * Velocity.Size() - Velocity) * TimeScale);
	// 		Velocity = Axe->GetPhysicsLinearVelocity();
	// 	}
	// }
	// else
	// {
	// 	// Dampen velocity magnitude based on deceleration.
	// 	if (Velocity.SizeSquared() > 0.f)
	// 	{
	// 		const FVector OldVelocity = Velocity;
	// 		const float VelSize = FMath::Max(Velocity.Size() -  DeltaTime, 0.f);
	// 		Axe->SetPhysicsLinearVelocity(Velocity.GetSafeNormal() * VelSize);
	// 		Velocity = Axe->GetPhysicsLinearVelocity();
	//
	// 		// Don't allow braking to lower us below max speed if we started above it.
	// 		if (bExceedingMaxSpeed && Velocity.SizeSquared() < FMath::Square(AdjustedMaxAxeSpeed))
	// 		{
	// 			Axe->SetPhysicsLinearVelocity(OldVelocity.GetSafeNormal() * AdjustedMaxAxeSpeed);
	// 			Velocity = Axe->GetPhysicsLinearVelocity();		
	// 		}
	// 	}
	// }
	//
	// // Apply acceleration and clamp velocity magnitude.
	// const float NewMaxSpeed = Velocity.Length() > AdjustedMaxAxeSpeed ? Velocity.Size() : AdjustedMaxAxeSpeed;
	// Axe->SetPhysicsLinearVelocity(Velocity + FVector(AxeAccelerationInput.X, AxeAccelerationInput.Y, 0.f) * DeltaTime);
	// Velocity = Axe->GetPhysicsLinearVelocity();
	// Axe->SetPhysicsLinearVelocity(Velocity.GetClampedToMaxSize(NewMaxSpeed));
	// Velocity = Axe->GetPhysicsLinearVelocity();
	
	// Adjust input directions to world direction.
	auto MappedVector = FVector(0.f, AxeAccelerationInput.X * MaxAxeSpeed, AxeAccelerationInput.Y * MaxAxeSpeed);
	// auto Transform = Axe->GetComponentTransform();
	// auto WorldRotator = UKismetMathLibrary::TransformRotation(Transform, Transform.Rotator());
	// auto CorrectedVector = UKismetMathLibrary::Quat_UnrotateVector(WorldRotator.Quaternion(), MappedVector);
	Axe->AddForce(MappedVector);
	
	AxeAccelerationInput.Set(0, 0);
}

void AGettinItUpCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AGettinItUpCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}




