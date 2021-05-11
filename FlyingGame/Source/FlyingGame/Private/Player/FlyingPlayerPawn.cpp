#include "Player/FlyingPlayerPawn.h"
#include "Player/Components/FlyingPlayerMovementComponent.h"

AFlyingPlayerPawn::AFlyingPlayerPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MovementComponent = CreateDefaultSubobject<UFlyingPlayerMovementComponent>(FName(TEXT("MovementComponent")));
}

UPawnMovementComponent* AFlyingPlayerPawn::GetMovementComponent() const
{
	return MovementComponent;
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