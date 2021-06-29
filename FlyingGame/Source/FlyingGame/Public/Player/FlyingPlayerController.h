#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FlyingPlayerController.generated.h"

class AFlyingPlayerPawn;

/**
 * Flying game player controller
 */
UCLASS()
class FLYINGGAME_API AFlyingPlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()
	
public:
	/** React to input axes */
	virtual void PlayerTick(float DeltaTime) override;

	/** Getter for CachedFlyingPlayerPawn */
	UFUNCTION(BlueprintPure)
	AFlyingPlayerPawn* GetFlyingPlayerPawn() const { return CachedFlyingPlayerPawn.Get(); }

	/** Scalar for look input for controlling yaw in degrees per second */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float YawLookScale = 60.f;

	/** Scalar for look input for controlling pitch in degrees per second */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PitchLookScale = 60.f;

	/** Maximum control rotation yaw. How far the camera can get away from the bird's rotation in yaw */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxControlRotationYaw = 20.f;

	/** Maximum control rotation pitch. How far the camera can get away from the bird's rotation in pitch */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxControlRotationPitch = 20.f;

	UFUNCTION(BlueprintPure, Category = FlyingPlayerController)
	bool GetIsLookFromGamepad() const { return bIsLookFromGamepad; }

protected:
	/** Add flying game input bindings */
	virtual void SetupInputComponent() override;

	/** Cache the pawn as an AFlyingPlayerPawn in CachedFlyingPlayerPawn */
	virtual void OnPossess(APawn* PawnToPossess) override;

	/** Uncache the pawn, setting CachedFlyingPlayerPawn back to null */
	virtual void OnUnPossess() override;

private:
	/** Input handling */
	void OnHorizontalMoveInput(float AxisValue);
	void OnVerticalMoveInput(float AxisValue);
	
	void OnHorizontalLookInput(float AxisValue);
	void OnVerticalLookInput(float AxisValue);
	void OnHorizontalLookInput_Gamepad(float AxisValue);
	void OnVerticalLookInput_Gamepad(float AxisValue);
	
	void OnBoostInput();

	/** Cached pawn as an AFlyingPlayerPawn */
	TWeakObjectPtr<AFlyingPlayerPawn> CachedFlyingPlayerPawn;

	/** The current values of the horizontal and vertical move axes */
	FVector2D CurrentMoveAxis;

	/** The current values of the horizontal and vertical look axes */
	FVector2D CurrentLookAxis;

	/** The current values of the horizontal and vertical look axes from the gamepad */
	FVector2D CurrentLookAxis_Gamepad;

	/** Whether or not a gamepad is being used to steer the player */
	bool bIsLookFromGamepad;
};