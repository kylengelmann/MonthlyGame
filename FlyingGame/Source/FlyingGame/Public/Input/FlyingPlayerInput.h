#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerInput.h"
#include "FlyingPlayerInput.generated.h"

UCLASS(config = Input, PerObjectConfig)
class UFlyingInputConfig : public UObject
{
	GENERATED_BODY()

public:
	/** Action mappings for this input configuration */
	UPROPERTY(config)
	TArray<struct FInputActionKeyMapping> ActionMappings;

	/** Axis mappings for this input configuration */
	UPROPERTY(config)
	TArray<struct FInputAxisKeyMapping> AxisMappings;

	/** Copy the mappings from the other config into this one */
	void CopyMappings(const UFlyingInputConfig* OtherConfig)
	{
		if (!OtherConfig) return;

		ActionMappings = OtherConfig->ActionMappings;
		AxisMappings = OtherConfig->AxisMappings;
	}
};

/**
 * Flying Game player input class. Adds functionality to the base player input class and defines values specific to Flying Game
 */
UCLASS()
class FLYINGGAME_API UFlyingPlayerInput : public UPlayerInput
{
	GENERATED_BODY()

public:

	/** Ctor */
	UFlyingPlayerInput();
	
	/** Name of the horizontal movement input axis */
	static const FName HorizontalMoveAxis;

	/** Name of the vertical movement input axis */
	static const FName VerticalMoveAxis;

	/** Name of the Boost input action */
	static const FName BoostAction;

	/** Initialize custom player input mappings */
	virtual void PostInitProperties() override;

private:
	/** Current keyboard and mouse input configuration */
	UPROPERTY(transient)
	UFlyingInputConfig* KeyboardInputConfig;

	/** Current gamepad input configuration */
	UPROPERTY(transient)
	UFlyingInputConfig* GamepadInputConfig;

	/** Apply the current gamepad and keyboard input mappings to the player input axis and action mappings */
	void ApplyInputMappings();
};