#include "Player/Components/FlyingPlayerMovementComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogFlyingPlayerMovementComponent, Log, All)

UFlyingPlayerMovementComponent::UFlyingPlayerMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
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
		const float DeltaAngle = SteerAxis.X * MaxTurningAngularSpeed * DeltaTime;
		const FQuat InputRotation = FQuat::MakeFromEuler(FVector::UpVector * DeltaAngle);

		// Calculate change in pitch from input
		const float DeltaPitch = SteerAxis.Y * PitchAngularSpeed * DeltaTime;

		// Find our current heading
		const FVector CurrentForwardVector = UpdatedComponent->GetForwardVector();
		FVector NewHeading = FVector::VectorPlaneProject(CurrentForwardVector, FVector::UpVector).GetSafeNormal();

		// If the current heading is zero, use last frame's current heading
		CurrentHeading = NewHeading.IsNearlyZero() ? CurrentHeading : NewHeading;

		// Convert the current heading to a rotation
		const FQuat CurrentRotation = CurrentHeading.ToOrientationQuat();

                // Find the player's current pitch
		CurrentPitch = FMath::Clamp(FMath::RadiansToDegrees(FMath::Asin(CurrentForwardVector | FVector::UpVector)), -MaxPitch, MaxPitch);
        
		// Find the new rotation of the player
		FQuat NewRotation = FQuat::Identity;
		if (bLastMoveWasBlocked)
		{
			// Apply input to the goal heading
			const FQuat PostInputGoalRotation = InputRotation * PostHitGoalHeading.ToOrientationQuat();

			// Find the angular distance from the current heading to the goal heading
			const float CurrentToGoalAngularDistance = FMath::UnwindDegrees(FMath::RadiansToDegrees(CurrentRotation.AngularDistance(PostInputGoalRotation))) * FMath::Sign(CurrentRotation.GetRightVector() | PostInputGoalRotation.GetForwardVector());

			// Apply input to the goal pitch
			const float PostInputPitch = PostHitGoalPitch + DeltaPitch;

			// Find the angular distance from the current pitch to the goal pitch
			const float CurrentToGoalPitchAngularDistance = PostInputPitch - CurrentPitch;

                        // Weight the corrections by the time it would take to reach the goal
			const FVector2D GoalWeights = FVector2D(CurrentToGoalAngularDistance / MaxTurningAngularSpeed, CurrentToGoalPitchAngularDistance / PitchAngularSpeed).GetSafeNormal();

			// Move the player to their goal rotation, prevent from overshooting the goal
			const float GoalDeltaAngle = MaxTurningAngularSpeed * DeltaTime * GoalWeights.X;
			if (GoalDeltaAngle >= FMath::Abs(CurrentToGoalAngularDistance))
			{
				NewRotation = PostInputGoalRotation;
			}
			else
			{
				const FQuat RotationThisTick = FQuat::MakeFromEuler(FVector::UpVector * GoalDeltaAngle);
				NewRotation = RotationThisTick * CurrentRotation;
			}

			// Move the player to their goal pitch, prevent from overshooting the goal
			const float GoalDeltaPitch = PitchAngularSpeed * DeltaTime * GoalWeights.Y;
			if (GoalDeltaPitch > FMath::Abs(CurrentToGoalPitchAngularDistance))
			{
				CurrentPitch = PostInputPitch;
			}
			else
			{
				CurrentPitch += GoalDeltaPitch;
			}
		}
		else
		{
			// Apply turning input to the new heading
			NewRotation = InputRotation * CurrentRotation;

			// Apply diving input
			CurrentPitch += DeltaPitch;
		}

		CurrentPitch = FMath::Clamp(CurrentPitch, -MaxPitch, MaxPitch);
		NewRotation = NewRotation * FQuat::MakeFromEuler(FVector::RightVector*CurrentPitch);

		// Get the new forward vector for velocity calculations
		const FVector NewForwardVector = NewRotation.GetForwardVector();

		// Calculate new velocity
		CurrentSpeed = NewForwardVector | Velocity;

		if (CurrentSpeed < MaxForwardsFlyingSpeed)
		{
			CurrentSpeed += GetGravityZ() * DeltaTime * NewForwardVector.Z;

			if (CurrentSpeed < DefaultForwardsFlyingSpeed)
			{
				const float SpeedDeficit = DefaultForwardsFlyingSpeed - CurrentSpeed;
				const float SpeedCorrectionAcceleration = FMath::Min(SpeedDeficit*SpeedCorrectionCoefficient, MaxSpeedCorrectionAcceleration);

				CurrentSpeed = FMath::Min(DefaultForwardsFlyingSpeed, CurrentSpeed + SpeedCorrectionAcceleration * DeltaTime);
			}
		}

		if (CurrentSpeed > DefaultForwardsFlyingSpeed)
		{
			const float DragAcceleration = (DefaultForwardsFlyingSpeed - CurrentSpeed) * DragCoefficient;
			CurrentSpeed = FMath::Max(DefaultForwardsFlyingSpeed, CurrentSpeed + DragAcceleration * DeltaTime);
		}

		Velocity = CurrentSpeed * NewForwardVector;

		// Update position
		const FVector DeltaPosition = Velocity * DeltaTime;

		bool bCurrentMoveWasBlocked = false;

		FHitResult Hit(1.f);
		const bool bSweep = true;
		SafeMoveUpdatedComponent(DeltaPosition, NewRotation, bSweep, Hit);

		// Handle collision
		if (Hit.Time < 1.f)
		{
			bCurrentMoveWasBlocked = true;

			HandleImpact(Hit, DeltaTime, DeltaPosition);

			const float TimeLeft = 1.f - Hit.Time;
			const bool bHandleImpact = true;
			SlideAlongSurface(DeltaPosition, TimeLeft, Hit.Normal, Hit, bHandleImpact);
		}

		bLastMoveWasBlocked = bCurrentMoveWasBlocked;
	}
}

void UFlyingPlayerMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta)
{
	Super::HandleImpact(Hit, TimeSlice, MoveDelta);

	// Zero out velocity in the direction of the collision
	Velocity = FVector::VectorPlaneProject(Velocity, Hit.Normal);

	if (ensure(UpdatedComponent))
	{
		// Find the new rotation the player should move towards
		// First try using their heading to find the rotation
		if (!TrySetPostHitHeadingAndAngle(UpdatedComponent->GetForwardVector(), Hit.Normal))
		{
			// If the heading didn't work, try using the velocity
			if (!TrySetPostHitHeadingAndAngle(Velocity, Hit.Normal))
			{
				// If the velocity didn't work either, just use the component's right vector and current pitch
				PostHitGoalPitch = CurrentPitch;

				// Make sure we aren't forcing a goal heading direction change, as that could cause the player to get stuck in corners
				const FVector NewGoalHeading = UpdatedComponent->GetRightVector();
				const bool bReverseNewGoalHeading = bLastMoveWasBlocked && (PostHitGoalHeading | NewGoalHeading) < 0.f;
				PostHitGoalHeading = bReverseNewGoalHeading ? -NewGoalHeading : NewGoalHeading;
			}
		}
	}
}

bool UFlyingPlayerMovementComponent::TrySetPostHitHeadingAndAngle(const FVector& ForwardVector, const FVector& HitNormal)
{
	const FVector ProjectedForwardVector = FVector::VectorPlaneProject(ForwardVector, HitNormal).GetSafeNormal();
	if (ProjectedForwardVector.IsNearlyZero())
	{
		return false;
	}

	// Find the diving angle needed to move out of this collision
	const float NewGoalPitch = FMath::RadiansToDegrees(FMath::Asin(ProjectedForwardVector | FVector::UpVector));

	// If the diving angle is oscillating, force it to be zero
	const bool bForceZeroPitch = bLastMoveWasBlocked && (PostHitGoalPitch * NewGoalPitch) < 0.f;

	// If the diving angle is being forced to zero or if it exceeds the max diving angle, a horizontal heading change is required to move out from collisions
	const bool bRequiresHorizontalHeadingChange = bForceZeroPitch || FMath::Abs(NewGoalPitch) > MaxPitch;

	// Find the heading needed to move out of this collision
	FVector NewPostHitGoalHeading = FMath::IsNearlyEqual(FMath::Abs(NewGoalPitch), 90.f) ? CurrentHeading : FVector::VectorPlaneProject(ProjectedForwardVector, FVector::UpVector).GetSafeNormal();

	// Force a horizontal heading change is there is not already one and one is required
	if (bRequiresHorizontalHeadingChange)
	{
		const float HeadingCompareTolerance = .05f;
		const bool bHasHorizontalHeadingChange = !FMath::IsNearlyEqual(CurrentHeading | NewPostHitGoalHeading, 1.f, HeadingCompareTolerance);
		if (!bHasHorizontalHeadingChange)
		{
			NewPostHitGoalHeading = NewPostHitGoalHeading ^ FVector::UpVector;
		}
	}

	// Don't allow the goal heading to switch directions, otherwise the player may get stuck in corners
	const bool bReverseNewGoalHeading = bLastMoveWasBlocked && (NewPostHitGoalHeading | PostHitGoalHeading) < 0.f;

	// Set the goal pitch and heading
	PostHitGoalPitch = bForceZeroPitch ? 0.f : FMath::Clamp(NewGoalPitch, -MaxPitch, MaxPitch);
	PostHitGoalHeading = bReverseNewGoalHeading ? -NewPostHitGoalHeading : NewPostHitGoalHeading;

	return true;
}

void UFlyingPlayerMovementComponent::AddImpulse(FVector Impulse, bool bVelocityChange)
{
	if(bVelocityChange)
	{
		Velocity += Impulse;
	}
	else if(Mass > SMALL_NUMBER)
	{
		Velocity += Impulse / Mass;
	}
	else
	{
		UE_LOG(LogFlyingPlayerMovementComponent, Error, TEXT("UFlyingPlayerMovementComponent::AddImpulse: Trying to add non-velocity change impulse to a pawn with zero mass"));
	}
}