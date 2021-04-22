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

protected:

	/** This frame's input acceleration */
	FVector InputAcceleration;
	
	/** Tick flying movement */
	void TickFlying(float DeltaTime);
};
