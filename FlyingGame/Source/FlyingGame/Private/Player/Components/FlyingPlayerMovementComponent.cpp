#include "Player/Components/FlyingPlayerMovementComponent.h"

UFlyingPlayerMovementComponent::UFlyingPlayerMovementComponent(const FObjectInitializer& ObjectInitializer)
{
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
		FVector RightVector = UpdatedComponent->GetRightVector();
		float DeltaAngle = FVector::DotProduct(RightVector, InputAcceleration) * MaxTurningSpeed * DeltaTime;

		// Calculate new rotation and forward vector
		FQuat NewRotation = UpdatedComponent->GetComponentRotation().Quaternion() * FQuat::MakeFromEuler(FVector::UpVector * DeltaAngle);
		FVector NewForwardVector = NewRotation.RotateVector(FVector::ForwardVector);

		// Calculate new velocity
		Velocity = DefaultForwardsFlyingSpeed * NewForwardVector;

		// Update position
		FVector DeltaPosition = Velocity * DeltaTime;

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