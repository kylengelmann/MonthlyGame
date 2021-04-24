#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FlyingPlayerController.generated.h"

class AFlyingPlayerPawn;

/**
 * Flying game player controller
 */
UCLASS()
class FLYINGGAME_API AFlyingPlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()
	
public:
	/** React to input axes */
	virtual void PlayerTick(float DeltaTime) override;

	/** Getter for CachedFlyingPlayerPawn */
	AFlyingPlayerPawn* GetFlyingPlayerPawn() const { return CachedFlyingPlayerPawn.Get(); }

protected:
	/** Add flying game input bindings */
	virtual void SetupInputComponent() override;

	/** Cache the pawn as an AFlyingPlayerPawn in CachedFlyingPlayerPawn */
	virtual void OnPossess(APawn* PawnToPossess) override;

	/** Uncache the pawn, setting CachedFlyingPlayerPawn back to null */
	virtual void OnUnPossess() override;

private:
	/** Input handling */
	void OnHorizontalMoveInput(float AxisValue);
	void OnVerticalMoveInput(float AxisValue);
	void OnBoostInput();

	/** Cached pawn as an AFlyingPlayerPawn */
	TWeakObjectPtr<AFlyingPlayerPawn> CachedFlyingPlayerPawn;

	/** The current values of the horizontal and vertical axes */
	FVector2D CurrentMoveAxis;
};