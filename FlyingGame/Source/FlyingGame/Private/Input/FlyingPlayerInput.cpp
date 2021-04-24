#include "Input/FlyingPlayerInput.h"

DEFINE_LOG_CATEGORY_STATIC(LogFlyingPlayerInput, Log, Warning)

/** File specific consts */
static const FName KeyboardInputConfigObjectName = FName(TEXT("KeyboardInputConfig"));
static const FName GamepadInputConfigObjectName = FName(TEXT("GamepadInputConfig"));

/** Class statics */
const FName UFlyingPlayerInput::HorizontalMoveAxis = FName(TEXT("HorizontalMoveAxis"));
const FName UFlyingPlayerInput::VerticalMoveAxis = FName(TEXT("VerticalMoveAxis"));
const FName UFlyingPlayerInput::BoostAction = FName(TEXT("BoostAction"));

UFlyingPlayerInput::UFlyingPlayerInput()
	: Super()
{
	KeyboardInputConfig = nullptr;
	GamepadInputConfig = nullptr;
}

void UFlyingPlayerInput::PostInitProperties()
{
	Super::PostInitProperties();

	// Load the input configs from the input ini file
	KeyboardInputConfig = NewObject<UFlyingInputConfig>(GetPackage(), KeyboardInputConfigObjectName);
	GamepadInputConfig = NewObject<UFlyingInputConfig>(GetPackage(), GamepadInputConfigObjectName);

	// Apply the input configs
	ApplyInputMappings();
}

void UFlyingPlayerInput::ApplyInputMappings()
{
	// Reset the input mappings to default, mark the keymaps as needing to be rebuilt, then add our own input mappings
	const bool bRestoreDefaults = true;
	ForceRebuildingKeyMaps(bRestoreDefaults);
	
	if (KeyboardInputConfig)
	{
		ActionMappings.Append(KeyboardInputConfig->ActionMappings);
		AxisMappings.Append(KeyboardInputConfig->AxisMappings);
	}
	else
	{
		UE_LOG(LogFlyingPlayerInput, Error, TEXT("UFlyingPlayerInput::AppyInputMappings: KeyboardInputConfig is null"));
	}

	if(GamepadInputConfig)
	{
		ActionMappings.Append(GamepadInputConfig->ActionMappings);
		AxisMappings.Append(GamepadInputConfig->AxisMappings);
	}
	else
	{
		UE_LOG(LogFlyingPlayerInput, Error, TEXT("UFlyingPlayerInput::AppyInputMappings: GamepadInputConfig is null"));
	}
}

