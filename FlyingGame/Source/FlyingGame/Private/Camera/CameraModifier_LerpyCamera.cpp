#include "Camera/CameraModifier_LerpyCamera.h"

void UCameraModifier_LerpyCamera::AddedToCamera(APlayerCameraManager* Camera)
{
	Super::AddedToCamera(Camera);

	PreviousCameraLocation = Camera->GetCameraLocation();
	PreviousCameraRotation = Camera->GetCameraRotation().Quaternion();
}

void UCameraModifier_LerpyCamera::ModifyCamera(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV)
{
	Super::ModifyCamera(DeltaTime, ViewLocation, ViewRotation, FOV, NewViewLocation, NewViewRotation, NewFOV);

	// Handle transitioning into and out of the modifier
	const float CurrentLocationLerpStrength = FMath::Lerp(1.f, LerpyCameraSettings.CameraLocationLerpStrength, Alpha);
	const float CurrentRotationSlerpStrength = FMath::Lerp(1.f, LerpyCameraSettings.CameraRotationSlerpStrength, Alpha);

	// Lerp and Slerp the camera
	NewViewLocation = PreviousCameraLocation = FMath::Lerp(PreviousCameraLocation, ViewLocation, CurrentLocationLerpStrength * DeltaTime);
	NewViewRotation = (PreviousCameraRotation = FQuat::Slerp(PreviousCameraRotation, ViewRotation.Quaternion(), CurrentRotationSlerpStrength * DeltaTime)).Rotator();
}