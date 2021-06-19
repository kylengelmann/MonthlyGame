#include "Player/FlyingPlayerController.h"
#include "Input/FlyingPlayerInput.h"
#include "Player/FlyingPlayerPawn.h"
#include "Camera/CameraModifier_LerpyCamera.h"

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
		InputComponent->BindAxis(UFlyingPlayerInput::HorizontalLookAxis, this, &AFlyingPlayerController::OnHorizontalMoveInput);
		InputComponent->BindAxis(UFlyingPlayerInput::VerticalLookAxis, this, &AFlyingPlayerController::OnVerticalMoveInput);

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
	CurrentMoveAxis.X = AxisValue;
}

void AFlyingPlayerController::OnVerticalMoveInput(float AxisValue)
{
	CurrentMoveAxis.Y = AxisValue;
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

		if(CachedFlyingPlayerPawn.IsValid() && PlayerCameraManager)
		{
			CurrentLerpyCameraModifier = Cast<UCameraModifier_LerpyCamera>(PlayerCameraManager->AddNewCameraModifier(UCameraModifier_LerpyCamera::StaticClass()));
			if(CurrentLerpyCameraModifier.IsValid())
			{
				CurrentLerpyCameraModifier->LerpyCameraSettings = LerpyCameraSettings;
			}
		}
	}
}

void AFlyingPlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	CachedFlyingPlayerPawn.Reset();

	if(CurrentLerpyCameraModifier.IsValid() && PlayerCameraManager)
	{
		PlayerCameraManager->RemoveCameraModifier(CurrentLerpyCameraModifier.Get());
	}

	CurrentLerpyCameraModifier.Reset();
}