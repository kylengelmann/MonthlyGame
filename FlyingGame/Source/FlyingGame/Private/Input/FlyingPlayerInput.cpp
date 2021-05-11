#include "Input/FlyingPlayerInput.h"

DEFINE_LOG_CATEGORY_STATIC(LogFlyingPlayerInput, Log, Warning)

/** File specific consts */
static const FName DefaultKeyboardInputConfigObjectName = FName(TEXT("DefaultKeyboardInputConfig"));
static const FName KeyboardInputConfigObjectName = FName(TEXT("KeyboardInputConfig"));

static const FName DefaultGamepadInputConfigObjectName = FName(TEXT("DefaultGamepadInputConfig"));
static const FName GamepadInputConfigObjectName = FName(TEXT("GamepadInputConfig"));

/** Class statics */
const FName UFlyingPlayerInput::HorizontalMoveAxis = FName(TEXT("HorizontalMoveAxis"));
const FName UFlyingPlayerInput::VerticalMoveAxis = FName(TEXT("VerticalMoveAxis"));

const FName UFlyingPlayerInput::HorizontalLookAxis = FName(TEXT("HorizontalLookAxis"));
const FName UFlyingPlayerInput::VerticalLookAxis = FName(TEXT("VerticalLookAxis"));

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
	// TODO: Use not default and populate missing axes and actions from default
	KeyboardInputConfig = NewObject<UFlyingInputConfig>(GetPackage(), DefaultKeyboardInputConfigObjectName);
	GamepadInputConfig = NewObject<UFlyingInputConfig>(GetPackage(), DefaultGamepadInputConfigObjectName);

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

