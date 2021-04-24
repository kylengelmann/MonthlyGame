#include "Player/FlyingPlayerController.h"
#include "Input/FlyingPlayerInput.h"
#include "Player/FlyingPlayerPawn.h"

AFlyingPlayerController::AFlyingPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AFlyingPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if(InputComponent)
	{
		// Bind movement
		InputComponent->BindAxis(UFlyingPlayerInput::HorizontalMoveAxis, this, &AFlyingPlayerController::OnHorizontalMoveInput);
		InputComponent->BindAxis(UFlyingPlayerInput::VerticalMoveAxis, this, &AFlyingPlayerController::OnVerticalMoveInput);

		// Bind actions
		InputComponent->BindAction(UFlyingPlayerInput::BoostAction, IE_Pressed, this, &AFlyingPlayerController::OnBoostInput);
	}
}

void AFlyingPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	
	if(AFlyingPlayerPawn* PlayerPawn = GetFlyingPlayerPawn())
	{
		PlayerPawn->SetMoveAxis(CurrentMoveAxis);
	}
}

void AFlyingPlayerController::OnHorizontalMoveInput(float AxisValue)
{
	// UE4 uses X as forward and Y as right :((((((
	CurrentMoveAxis.Y = AxisValue;
}

void AFlyingPlayerController::OnVerticalMoveInput(float AxisValue)
{
	// UE4 uses X as forward and Y as right :((((((
	CurrentMoveAxis.X = AxisValue;
}

void AFlyingPlayerController::OnBoostInput()
{
	
}

void AFlyingPlayerController::OnPossess(APawn* PawnToPossess)
{
	if(PawnToPossess)
	{
		Super::OnPossess(PawnToPossess);

		CachedFlyingPlayerPawn = GetPawn<AFlyingPlayerPawn>();
	}
}

void AFlyingPlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	CachedFlyingPlayerPawn.Reset();
}