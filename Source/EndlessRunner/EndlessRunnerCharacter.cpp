// Copyright Epic Games, Inc. All Rights Reserved.

#include "EndlessRunnerCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "EndlessRunner.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"

AEndlessRunnerCharacter::AEndlessRunnerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AEndlessRunnerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bDeath == false)
	{
		RunEndless();
		TurnCorner();
	}
}

void AEndlessRunnerCharacter::RunEndless()
{
	if (AController* MyController = GetController())
	{
		const FRotator Rotation = MyController->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(ForwardDirection, 1.0f);
	}
}

void AEndlessRunnerCharacter::TurnCorner()
{
	switch (TurnStep)
	{
	case ETurnStep::Turning:
		if (AController* MyController = GetController())
		{
			constexpr float TurnCompleteTolerance = 0.5f;

			if (DesiredTurnRotation.Equals(MyController->GetControlRotation(), TurnCompleteTolerance))
			{
				MyController->SetControlRotation(DesiredTurnRotation);
				SetTurnStep(ETurnStep::TurnComplete);
			}
			else
			{
				const FRotator NewRotation = FMath::RInterpTo(MyController->GetControlRotation(), DesiredTurnRotation, GetWorld()->GetDeltaSeconds(), 5.0f);
				MyController->SetControlRotation(NewRotation);
			}
		}
		break;

	case ETurnStep::TurnComplete:
		SetTurnStep(ETurnStep::CanNotTurn);
		break;
	}
}

void AEndlessRunnerCharacter::SetCanTurn()
{
	if (TurnStep == ETurnStep::CanNotTurn)
	{
		TurnStep = ETurnStep::CanTurn;
	}
}

void AEndlessRunnerCharacter::ClearCanTurn()
{
	if (TurnStep == ETurnStep::CanTurn)
	{
		TurnStep = ETurnStep::CanNotTurn;
	}
}

void AEndlessRunnerCharacter::DoDeath()
{
	bDeath = true;

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		DisableInput(PlayerController);
	}

	if (USkeletalMeshComponent* MeshComponent = GetMesh())
	{
		MeshComponent->SetVisibility(false);
	}

	if (DeathParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathParticle, GetActorLocation());
	}

	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
	}

	FTimerHandle RestartTimerHandle;

	GetWorldTimerManager().SetTimer(RestartTimerHandle, 
		FTimerDelegate::CreateWeakLambda(this, [this]()
			{
				UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()));
			}),
		2.0f, false
	);
}

void AEndlessRunnerCharacter::AddCoin()
{
	int32 OldCount = AddedCoinCount++;

	if (OnAddedCoin.IsBound())
	{
		OnAddedCoin.Broadcast(AddedCoinCount, OldCount);
	}
}

void AEndlessRunnerCharacter::MoveJustLeftRight(const FInputActionValue& InValue)
{
	if (TurnStep == ETurnStep::CanNotTurn)
	{
		if (AController* MyController = GetController())
		{
			const FVector2D& MovementVector = InValue.Get<FVector2D>();

			const FRotator& Rotation = MyController->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			const FVector& RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			AddMovementInput(RightDirection, MovementVector.X);
		}
	}
}

void AEndlessRunnerCharacter::CheckTurn(const FInputActionValue& InValue)
{
	const FVector2D& MovementVector = InValue.Get<FVector2D>();

	if (TurnStep == ETurnStep::CanTurn &&
		FMath::IsNearlyZero(MovementVector.X) == false)
	{
		DesiredTurnRotation += FRotator(0.0f, MovementVector.X > 0.f ? 90.0f : MovementVector.X < 0.f ? -90.f : 0.0f, 0.0f);
		SetTurnStep(ETurnStep::Turning);
	}
}

void AEndlessRunnerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Started, this, &AEndlessRunnerCharacter::CheckTurn);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AEndlessRunnerCharacter::MoveJustLeftRight);

		// EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AEndlessRunnerCharacter::Move);
		// EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AEndlessRunnerCharacter::Look);

		// Looking
		// EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AEndlessRunnerCharacter::Look);
	}
	else
	{
		UE_LOG(LogEndlessRunner, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AEndlessRunnerCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AEndlessRunnerCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AEndlessRunnerCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		
		AddMovementInput(RightDirection, Right);
		AddMovementInput(ForwardDirection, Forward);
	}
}

void AEndlessRunnerCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AEndlessRunnerCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AEndlessRunnerCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

