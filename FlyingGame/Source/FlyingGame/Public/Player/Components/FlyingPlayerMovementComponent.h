#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "FlyingPlayerMovementComponent.generated.h"

/**
 * Component for moving FlyingGame players
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class FLYINGGAME_API UFlyingPlayerMovementComponent : public UPawnMovementComponent
{
	GENERATED_UCLASS_BODY()

public:
	/** Update movement */
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	/** The speed the player will fly forwards at with no input */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flying Player Movement Component | Movement Parameters")
	float DefaultForwardsFlyingSpeed = 500.f;

	/** The max speed the player can turn at in degrees per second */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flying Player Movement Component | Movement Parameters")
	float MaxTurningSpeed = 60.f;

	/** The max angle the player can dive down at */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flying Player Movement Component | Movement Parameters")
	float MaxDivingAngle = 60.f;

	/** The rate at which the player's diving angle changes, in degrees per second */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flying Player Movement Component | Movement Parameters")
	float DivingAngleSpeed = 30.f;

	/** The maximum speed at which the player can fly at */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flying Player Movement Component | Movement Parameters")
	float MaxSpeed = 1500.f;

protected:

	/** This frame's input acceleration */
	FVector InputAcceleration;
	
	/** Tick flying movement */
	void TickFlying(float DeltaTime);

	/** The current heading of the player (Their forward vector, ignoring the z component) */
	FVector CurrentHeading;

	/** The angle at which the player is diving */
	float CurrentDiveAngle = 0.f;

	/** The player's current speed */
	float CurrentSpeed;
};
