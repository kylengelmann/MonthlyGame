#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "FlyingPlayerMovementComponent.generated.h"

/**
 * Component for moving FlyingGame players
 */
UCLASS(HideCategories=(Sockets, NavMovement, Velocity, PlanarMovement, Collision), meta = (BlueprintSpawnableComponent))
class FLYINGGAME_API UFlyingPlayerMovementComponent : public UPawnMovementComponent
{
	GENERATED_UCLASS_BODY()

public:
	/** Update movement */
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	/** Handle collisions by zeroing out our velocity in the direction of the hit's normal */
	virtual void HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta) override;

	/** The speed the player will fly forwards at when at equilibrium */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flying Player Movement Component", meta=(ClampMin="0.0"))
	float DefaultForwardsFlyingSpeed = 500.f;

	/** The maximum speed at which the player can fly at. Past this point, gravity will no longer add to the speed of the bird */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flying Player Movement Component", meta=(ClampMin="0.0"))
	float MaxForwardsFlyingSpeed = 1500.f;

	/** The max forwards acceleration that can be applied for speed correction purposes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flying Player Movement Component", meta = (ClampMin = "0.0"))
	float MaxSpeedCorrectionAcceleration = 100.f;

	/** The coefficient that will multiply the value of the current speed deficit to calculate the speed correction acceleration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flying Player Movement Component", meta = (ClampMin = "0.0"))
	float SpeedCorrectionCoefficient = 1.f;

	/** The coefficient of the drag force applied to the player when they exceed their goal speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flying Player Movement Component", meta = (ClampMin = "0.0"))
	float DragCoefficient = .1f;

	/** The max speed the player can turn at in degrees per seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flying Player Movement Component", meta = (ClampMin = "0.0"))
	float MaxTurningAngularSpeed = 100.f;

	/** The max angle the player can fly up and dive down at */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flying Player Movement Component", meta = (ClampMin = "0.0", ClampMax = "89.99"))
	float MaxPitch = 60.f;

	/** The rate at which the player's pitch changes, in degrees per seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flying Player Movement Component", meta = (ClampMin = "0.0"))
	float PitchAngularSpeed = 50.f;

	/** Mass of the player pawn, for when physics physics */
	UPROPERTY(Category = "Flying Player Movement Component", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float Mass = 5.f;

	UFUNCTION(BlueprintCallable, Category = "Flying Player Movement Component")
	void AddImpulse(FVector Impulse, bool bVelocityChange = false);

	UFUNCTION(BlueprintCallable, Category = "Flying Player Movement Component")
	void SetSteerAxis(const FVector2D& Steer) { SteerAxis = Steer; }

protected:

	/** This frame's input acceleration */
	FVector InputAcceleration;
	
	/** Tick flying movement */
	void TickFlying(float DeltaTime);

	/** The current heading of the player (Their forward vector, ignoring the z component) */
	FVector CurrentHeading;

	/** The player's current pitch */
	float CurrentPitch = 0.f;

	/** The player's current speed */
	float CurrentSpeed;

	/** Whether or not we hit something during our last move */
	bool bLastMoveWasBlocked;

	/** The heading that the player should move towards after hitting something */
	FVector PostHitGoalHeading;

	/** The pitch that the player should move towards after hitting something */
	float PostHitGoalPitch;

	/** Given a forward vector and a hit normal, tries to find the best goal heading and pitch and sets the respective values if so.
	    Returns whether or not it was successful in finding a good heading and pitch */
	bool TrySetPostHitHeadingAndAngle(const FVector& ForwardVector, const FVector& HitNormal);

	/** The axis used to steer the player, should be between -1 and 1 on all axes */
	FVector2D SteerAxis;
};
