// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "EndlessRunnerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAddedCoin, int32 /*InNewCount*/, int32 /*InOldCount*/);

enum class ETurnStep
{
	CanNotTurn,
	CanTurn,	
	Turning,
	TurnComplete,
};


UCLASS(abstract)
class AEndlessRunnerCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

public:

	/** Constructor */
	AEndlessRunnerCharacter();	

	virtual void Tick(float DeltaSeconds) override;
	
	void RunEndless();
	void TurnCorner();

	UFUNCTION(BlueprintCallable, Category = "Turn")
	void SetCanTurn();

	void ClearCanTurn();

	UFUNCTION(BlueprintCallable, Category = "EndlessRunner")
	void DoDeath();

	UFUNCTION(BlueprintCallable, Category = "EndlessRunner")
	void AddCoin();

	FORCEINLINE void SetTurnStep(ETurnStep InTurnStep) { TurnStep = InTurnStep; }

	void MoveJustLeftRight(const FInputActionValue& InValue);
	void CheckTurn(const FInputActionValue& InValue);
	
	FOnAddedCoin& GetOnAddedCoin() { return OnAddedCoin; }

private:
	ETurnStep TurnStep;
	FRotator DesiredTurnRotation;

	bool bDeath = false;

	int32 AddedCoinCount = 0;

	UPROPERTY(EditDefaultsOnly, Category = "EndlessRunner")
	TObjectPtr<class UParticleSystem> DeathParticle;

	UPROPERTY(EditDefaultsOnly, Category = "EndlessRunner")
	TObjectPtr<class USoundBase> DeathSound;

	FOnAddedCoin OnAddedCoin;

protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

