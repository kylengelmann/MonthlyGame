#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraModifier.h"
#include "CameraModifier_LerpyCamera.generated.h"

USTRUCT(BlueprintType)
struct FLerpyCameraSettings
{
	GENERATED_BODY()
	
public:
	/** The strength of the lerp to the camera's desired position */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraLocationLerpStrength = 5.f;

	/** The strength of the lerp to the camera's desired rotation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraRotationSlerpStrength = 8.f;

	void operator=(const FLerpyCameraSettings& Other)
	{
		CameraLocationLerpStrength = Other.CameraLocationLerpStrength;
		CameraRotationSlerpStrength = Other.CameraRotationSlerpStrength;
	}
};

/*
 * Lerps the camera to its new location and rotation rather than just snapping to it
 */
UCLASS()
class FLYINGGAME_API UCameraModifier_LerpyCamera : public UCameraModifier
{
	GENERATED_BODY()

public:
	/** Set the last frame's locations and rotation with the current rotation and position */
	virtual void AddedToCamera(APlayerCameraManager* Camera) override;

	/** The settings that describe this LerpyCamera's behavior */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Lerpy Camera")
	FLerpyCameraSettings LerpyCameraSettings;

protected:
	/** Lerp towards the new view location and rotation rather than snap to them */
	virtual void ModifyCamera(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV) override;

private:
	/** The camera's previous location that the camera will be lerped from */
	FVector PreviousCameraLocation;

	/** The camera's previous rotation that the camera will be lerped from */
	FQuat PreviousCameraRotation;
};