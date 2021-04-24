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
		// Make sure the move axis never has a length greater than 1
		FVector MoveAxisClamped = FVector(MoveAxis, 0.f);
		if(MoveAxisClamped.SizeSquared() > 1.f)
		{
			MoveAxisClamped.Normalize();
		}
		
		MovementComponent->AddInputVector(MoveAxisClamped);
	}
}