#include "Player/FlyingPlayerPawn.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/Components/FlyingPlayerMovementComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogFlyingPlayerPawn, Log, All)

AFlyingPlayerPawn::AFlyingPlayerPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MovementComponent = CreateDefaultSubobject<UFlyingPlayerMovementComponent>(FName(TEXT("MovementComponent")));
}

UPawnMovementComponent* AFlyingPlayerPawn::GetMovementComponent() const
{
	return MovementComponent;
}

void AFlyingPlayerPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (const AFlyingPlayerController* FPC = FlyingPlayerController.Get())
	{
		if (MovementComponent)
		{
			const FRotator CurrentControlRotation = GetControlRotation();
			const FVector2D SteerAxis = FVector2D(CurrentControlRotation.Yaw / FPC->MaxControlRotationYaw, CurrentControlRotation.Pitch / FPC->MaxControlRotationPitch);
			MovementComponent->SetSteerAxis(SteerAxis);
		}
	}
}

void AFlyingPlayerPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	FlyingPlayerController = Cast<AFlyingPlayerController>(NewController);

	if(!FlyingPlayerController.IsValid())
	{
		UE_LOG(LogFlyingPlayerPawn, Error, TEXT("AFlyingPlayerPawn::PossessedBy: NewController %s is either null or not an AFlyingPlayerController"), *NewController->GetName());
	}
}

void AFlyingPlayerPawn::UnPossessed()
{
	Super::UnPossessed();
	
	FlyingPlayerController.Reset();
}


void AFlyingPlayerPawn::SetMoveAxis(const FVector2D& MoveAxis)
{
	if(MovementComponent)
	{
		// Convert the move axis to 3D and make sure the axis never has a length greater than 1
		FVector MoveAxisClamped = FVector::ForwardVector * MoveAxis.Y + FVector::RightVector * MoveAxis.X;
		if(MoveAxisClamped.SizeSquared() > 1.f)
		{
			MoveAxisClamped.Normalize();
		}

		// Pass the axis on to the movement component
		MovementComponent->AddInputVector(MoveAxisClamped);
	}
}

void AFlyingPlayerPawn::Boost()
{
	if(!MovementComponent)
	{
		UE_LOG(LogFlyingPlayerPawn, Error, TEXT("AFlyingPlayerPawn::Boost: Null movement component"));
		return;
	}
	
	if(const UWorld* World = GetWorld())
	{
		// Check cooldown
		const float CurrentTime = World->GetTimeSeconds();
		if(World->GetTimeSeconds() - LastBoostTime < BoostCooldown)
		{
			UE_LOG(LogFlyingPlayerPawn, Verbose, TEXT("AFlyingPlayerPawn::Boost: Tried to boost before boost cooldown is has finished. Current Time: %.4f, LastBoostTime: %.4f, Cooldown: %f"), 
				CurrentTime, LastBoostTime, BoostCooldown);

			return;
		}

		// reset the cooldown
		LastBoostTime = CurrentTime;

		// Calculate how much velocity should be added to the player
		float CurrentBoostStrength = BoostStrength;
		if(BoostStrengthCurve)
		{
			const float CurrentSpeed = MovementComponent->Velocity | GetActorForwardVector();
			CurrentBoostStrength *= BoostStrengthCurve->GetFloatValue(CurrentSpeed / MovementComponent->MaxForwardsFlyingSpeed);
		}

		// zzzzzzooooooom
		const bool bVelocityChange = true;
		MovementComponent->AddImpulse(GetActorForwardVector() * CurrentBoostStrength, bVelocityChange);
	}
}