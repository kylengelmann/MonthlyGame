#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FlyingPlayerPawn.generated.h"

class UFlyingPlayerMovementComponent;

/**
 * Flying game player pawn
 */
UCLASS()
class FLYINGGAME_API AFlyingPlayerPawn : public APawn
{
	GENERATED_UCLASS_BODY()

public:
	/** Pass the move axis on to the movement component */
	void SetMoveAxis(const FVector2D& MoveAxis);
	
	/** Getters for MovementComponent */
	UFUNCTION(BlueprintPure, Category = "Flying Player Pawn")
	UFlyingPlayerMovementComponent* GetFlyingMovementComponent() const { return MovementComponent; }
	virtual UPawnMovementComponent* GetMovementComponent() const override;

private:
	/** This player's movement component */
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess="true"))
	UFlyingPlayerMovementComponent* MovementComponent;
};