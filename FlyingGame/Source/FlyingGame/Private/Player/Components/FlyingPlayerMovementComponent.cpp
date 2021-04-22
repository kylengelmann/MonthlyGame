#include "Player/Components/FlyingPlayerMovementComponent.h"

UFlyingPlayerMovementComponent::UFlyingPlayerMovementComponent(const FObjectInitializer& ObjectInitializer)
{
	CurrentHeading = FVector::ForwardVector;
}

void UFlyingPlayerMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	InputAcceleration = ConsumeInputVector();

	TickFlying(DeltaTime);
}

void UFlyingPlayerMovementComponent::TickFlying(float DeltaTime)
{
	if (UpdatedComponent)
	{
		// Calculate change in rotation from input
		const FVector RightVector = UpdatedComponent->GetRightVector();
		const float DeltaAngle = (RightVector | InputAcceleration) * MaxTurningSpeed * DeltaTime;

		// Find our current heading
		const FVector CurrentForwardVector = UpdatedComponent->GetForwardVector();
		FVector NewHeading = FVector(CurrentForwardVector.X, CurrentForwardVector.Y, 0.f).GetSafeNormal();

		// If the current heading is zero, use last frame's current heading
		NewHeading = NewHeading.IsNearlyZero() ? CurrentHeading : NewHeading;
		
		// Apply turning input to the new heading
		NewHeading = FQuat::MakeFromEuler(FVector::UpVector * DeltaAngle).RotateVector(NewHeading);

		// Find the new rotation of the player from their new heading
		FQuat NewRotation = NewHeading.ToOrientationQuat();

		// Apply diving input
		const float DeltaDiveAngle = (CurrentForwardVector | InputAcceleration) * DivingAngleSpeed * DeltaTime;
		CurrentDiveAngle = FMath::Clamp(CurrentDiveAngle + DeltaDiveAngle, -MaxDivingAngle, MaxDivingAngle);

		NewRotation = NewRotation * FQuat::MakeFromEuler(FVector::RightVector*-CurrentDiveAngle);

		const FVector NewForwardVector = NewRotation.GetForwardVector();

		// Calculate new velocity
		Velocity = DefaultForwardsFlyingSpeed * NewForwardVector;

		// Update position
		const FVector DeltaPosition = Velocity * DeltaTime;

		FHitResult Hit(1.f);
		const bool bSweep = true;
		SafeMoveUpdatedComponent(DeltaPosition, NewRotation, bSweep, Hit);

		// Handle collision
		if (Hit.Time < 1.f)
		{
			HandleImpact(Hit, DeltaTime, DeltaPosition);

			const float TimeLeft = 1.f - Hit.Time;
			const bool bHandleImpact = true;
			SlideAlongSurface(DeltaPosition, TimeLeft, Hit.Normal, Hit, bHandleImpact);
		}
	}
}