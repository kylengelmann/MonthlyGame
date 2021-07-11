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
		
		InputComponent->BindAxis(UFlyingPlayerInput::HorizontalLookAxis, this, &AFlyingPlayerController::OnHorizontalLookInput);
		InputComponent->BindAxis(UFlyingPlayerInput::VerticalLookAxis, this, &AFlyingPlayerController::OnVerticalLookInput);
		InputComponent->BindAxis(UFlyingPlayerInput::HorizontalLookAxis_Gamepad, this, &AFlyingPlayerController::OnHorizontalLookInput_Gamepad);
		InputComponent->BindAxis(UFlyingPlayerInput::VerticalLookAxis_Gamepad, this, &AFlyingPlayerController::OnVerticalLookInput_Gamepad);

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

		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 10.f, FColor::Emerald, FString::Printf(TEXT("Gamepad look axis: %.2f, %.2f"), CurrentLookAxis_Gamepad.X, CurrentLookAxis_Gamepad.Y));
		}

		// Figure out whether to use the gamepad look or the mouse look
		const bool bMouseUpdated = CurrentLookAxis.SizeSquared() > SMALL_NUMBER;
		const bool bGamepadUpdated = CurrentLookAxis_Gamepad.SizeSquared() > SMALL_NUMBER;

		bIsLookFromGamepad = (bMouseUpdated ^ bGamepadUpdated) ? bGamepadUpdated : bIsLookFromGamepad;

		if(bIsLookFromGamepad)
		{
			CurrentSteerAxis = CurrentLookAxis_Gamepad;

			PlayerPawn->SetSteerAxis(CurrentSteerAxis);
		}
		else
		{
			CurrentSteerAxis.X = FMath::Clamp(CurrentSteerAxis.X + (CurrentLookAxis.X * YawLookScale * DeltaTime) / MaxControlRotationYaw, -1.f, 1.f);
			CurrentSteerAxis.Y = FMath::Clamp(CurrentSteerAxis.Y + (CurrentLookAxis.Y * PitchLookScale * DeltaTime) / MaxControlRotationPitch, -1.f, 1.f);

			const float CurrentSteerAxisMagnitude = CurrentSteerAxis.Size();
			const float Sqrt2 = FMath::Sqrt(2.f);
			const float SteerAxisMagnitudeDeadzoneApplied = FMath::GetMappedRangeValueClamped(FVector2D(SteerDeadzoneSize, Sqrt2), FVector2D(0.f, Sqrt2), CurrentSteerAxisMagnitude);

			FVector2D SteerAxisDeadzoneApplied = CurrentSteerAxis.GetSafeNormal() * SteerAxisMagnitudeDeadzoneApplied;
			PlayerPawn->SetSteerAxis(SteerAxisDeadzoneApplied);

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(3, 10.f, FColor::Emerald, FString::Printf(TEXT("SteerAxis: %s, SteerAxisDeadzone: %s"), *CurrentSteerAxis.ToString(), *SteerAxisDeadzoneApplied.ToString()));
			}
		}

		const FQuat ControlRotationQuat = FQuat::MakeFromEuler(FVector(0.f, 0.f, CurrentSteerAxis.X * MaxControlRotationYaw)) * FQuat::MakeFromEuler(FVector(0.f, CurrentSteerAxis.Y * MaxControlRotationPitch, 0.f));
		SetControlRotation(ControlRotationQuat.Rotator());
	}
}

void AFlyingPlayerController::OnHorizontalMoveInput(float AxisValue)
{
	CurrentMoveAxis.X = AxisValue;
}

void AFlyingPlayerController::OnVerticalMoveInput(float AxisValue)
{
	CurrentMoveAxis.Y = AxisValue;
}

void AFlyingPlayerController::OnHorizontalLookInput(float AxisValue)
{
	CurrentLookAxis.X = AxisValue;
}

void AFlyingPlayerController::OnVerticalLookInput(float AxisValue)
{
	CurrentLookAxis.Y = AxisValue;
}

void AFlyingPlayerController::OnHorizontalLookInput_Gamepad(float AxisValue)
{
	CurrentLookAxis_Gamepad.X = AxisValue;
}

void AFlyingPlayerController::OnVerticalLookInput_Gamepad(float AxisValue)
{
	CurrentLookAxis_Gamepad.Y = AxisValue;
}

void AFlyingPlayerController::OnBoostInput()
{
	if(AFlyingPlayerPawn* PlayerPawn = GetFlyingPlayerPawn())
	{
		PlayerPawn->Boost();
	}
}

void AFlyingPlayerController::OnPossess(APawn* PawnToPossess)
{
	if(PawnToPossess)
	{
		Super::OnPossess(PawnToPossess);

		SetControlRotation(FRotator::ZeroRotator);
		
		CachedFlyingPlayerPawn = GetPawn<AFlyingPlayerPawn>();
	}
}

void AFlyingPlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	CachedFlyingPlayerPawn.Reset();
}

