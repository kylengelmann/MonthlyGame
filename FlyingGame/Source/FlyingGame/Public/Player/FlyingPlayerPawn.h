#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FlyingPlayerController.h"
#include "FlyingPlayerPawn.generated.h"

class UFlyingPlayerMovementComponent;
class USpringArmComponent;

/**
 * Flying game player pawn
 */
UCLASS()
class FLYINGGAME_API AFlyingPlayerPawn : public APawn
{
	GENERATED_UCLASS_BODY()

public:
	virtual void Tick(float DeltaSeconds) override;

	/** Cache the controller */
	virtual void PossessedBy(AController* NewController) override;

	/** Clear the cached controller */
	virtual void UnPossessed() override;
	
	/** Pass the move axis on to the movement component */
	void SetMoveAxis(const FVector2D& MoveAxis);
	
	/** Getters for MovementComponent */
	UFUNCTION(BlueprintPure, Category = "Flying Player Pawn")
	UFlyingPlayerMovementComponent* GetFlyingMovementComponent() const { return MovementComponent; }
	virtual UPawnMovementComponent* GetMovementComponent() const override;

	/** The strength of the player's boost. The X axis is the speed of the bird, normalized to UFlyingPlayerMovementComponent::MaxForwardsFlyingSpeed, while the Y axis is the velocity added, scaled by BoostStrength */
	UPROPERTY(EditAnywhere, Category = "Flying Player Pawn | Boost")
	UCurveFloat* BoostStrengthCurve;

	/** The strength of the player's boost. Scales the Y axis of BoostStrengthCurve */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flying Player Pawn | Boost")
	float BoostStrength = 100.f;

	/** The minimum time in seconds that must pass after the player boosts before they can boost again */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flying Player Pawn | Boost")
	float BoostCooldown = .75f;

	/** Triggers a boost if the boost is not in cooldown */
	void Boost();

	UFUNCTION(BlueprintPure, Category = FlyingPlayerPawn)
	AFlyingPlayerController* GetFlyingPlayerController() const { return FlyingPlayerController.Get(); }

private:
	/** This player's movement component */
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess="true"))
	UFlyingPlayerMovementComponent* MovementComponent;

	/** The time at which the last boost was triggered */
	float LastBoostTime;

	/** Cached AFlyingPlayerController */
	TSoftObjectPtr<AFlyingPlayerController> FlyingPlayerController;
};