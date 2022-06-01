// Fill out your copyright notice in the Description page of Project Settings.


#include "GEIIProjectFunctionLibrary.h"

#include "Engine/TextureRenderTarget2D.h"

bool UGEIIProjectFunctionLibrary::RectangleToRectangleCollision(FVector2D Rectangle1Origin, FVector2D Rectangle1Extents,
                                                                FVector2D Rectangle2Origin, FVector2D Rectangle2Extents)
{
	// If right side of rect 1 is less than the left side of rect 2
	if(Rectangle1Origin.X + Rectangle1Extents.X < Rectangle2Origin.X - Rectangle2Extents.X) { return false; }
	
	// If left side of rect 1 is less than the right side of rect 2
	if(Rectangle1Origin.X - Rectangle1Extents.X > Rectangle2Origin.X + Rectangle2Extents.X) { return false; }
	
	// If top side of rect 1 is less than the bottom side of rect 2
	if(Rectangle1Origin.Y + Rectangle1Extents.Y < Rectangle2Origin.Y - Rectangle2Extents.Y) { return false; }
	
	// If bottom side of rect 1 is less than the top side of rect 2
	if(Rectangle1Origin.Y - Rectangle1Extents.Y > Rectangle2Origin.Y + Rectangle2Extents.Y) { return false; }

	// If all of the above are false
	return true;
}

void UGEIIProjectFunctionLibrary::ResizeRenderTarget(UTextureRenderTarget2D* RenderTarget, float SizeX, float SizeY)
{
	if(RenderTarget == nullptr) { return; }

	RenderTarget->ResizeTarget(SizeX, SizeY);
}

FVector UGEIIProjectFunctionLibrary::ConvertLocation(FVector const& CurrentLocation, AActor* OriginPortal, AActor* DestinationPortal)
{
	// Calculating Direction = Current Location - Destination Location
	FVector Direction = CurrentLocation - OriginPortal->GetActorLocation();
	FVector TargetLocation = DestinationPortal->GetActorLocation();

	FVector Dots;

	// Dot product to calculate which side the character's facing, returns a float value between 0 and 1
	// 0 = He's not facing that, direction, 1 = He's facing that direction, any value in between
	// Means he's facing that direction at an angle 
	Dots.X = FVector::DotProduct(Direction, OriginPortal->GetActorForwardVector());
	Dots.Y = FVector::DotProduct(Direction, OriginPortal->GetActorRightVector());
	Dots.Z = FVector::DotProduct(Direction, OriginPortal->GetActorUpVector());

	// Makes the actor face the Forward Vector of the portal
	FVector NewDirection = Dots.X * -DestinationPortal->GetActorForwardVector() +
						   Dots.Y * -DestinationPortal->GetActorRightVector() +
						   Dots.Z * DestinationPortal->GetActorUpVector();

	// Teleport to TargetLocation facing the NewDirection must call ConvertRotation after
	// for correct rotation
	return TargetLocation + NewDirection;
}

FRotator UGEIIProjectFunctionLibrary::ConvertRotation(FRotator const& CurrentRotation, AActor* OriginPortal, AActor* DestinationPortal)
{
	FVector RotationAdjustment(0.0f, 0.0f, -180.0f);
	FVector LocalAdjustment = FVector::ZeroVector;

	// Comparing floats, since they're not 100% accurate we'll check if their decimals (KINDA_SMALL_NUMBER)
	// are close enough to be considered that they have the "same" value
	if(FVector::DotProduct(OriginPortal->GetActorForwardVector(), FVector::UpVector) > KINDA_SMALL_NUMBER)
	{
		// Make sure that X's angle is between -180 and 180 degrees
		LocalAdjustment.X = FMath::UnwindDegrees(OriginPortal->GetTransform().GetRotation().Euler().X);
		LocalAdjustment.Y = 180.0f;
		RotationAdjustment.Z += LocalAdjustment.X;
	}
	else if(FVector::DotProduct(OriginPortal->GetActorForwardVector(), FVector::UpVector) < -KINDA_SMALL_NUMBER)
	{
		LocalAdjustment.X = FMath::UnwindDegrees(OriginPortal->GetTransform().GetRotation().Euler().X);
		LocalAdjustment.Y = -180.0f;
		RotationAdjustment.Z -= LocalAdjustment.X;
	}

	// Rotate in the Z axis
	FQuat QuatRotation = FQuat::MakeFromEuler(RotationAdjustment) * FQuat(CurrentRotation);
	// Translates rotation from World Space to Relative (Local) Space
	FQuat LocalQuat = FQuat::MakeFromEuler(LocalAdjustment) *
					  OriginPortal->GetActorTransform().GetRotation().Inverse() * QuatRotation;

	// Final rotation
	FQuat NewWorldQuat = DestinationPortal->GetActorTransform().GetRotation() * LocalQuat;

	return  NewWorldQuat.Rotator();
}

bool UGEIIProjectFunctionLibrary::CheckIsInFront(FVector const& ActorLocation, FVector const& PortalLocation,
                                           FVector const& PortalDirection)
{
	// Create imaginary plane at PortalLocation facing normalized PortalDirection
	// So it only cares the direction it's facing, ignoring lenght.
	FPlane PortalPlane = FPlane(PortalLocation, PortalDirection);

	// Returns true if ActorLocation's >= 0 (IS In Front) and false if < 0 (Is Behind)
	return PortalPlane.PlaneDot(ActorLocation) >= 0;
}

bool UGEIIProjectFunctionLibrary::CheckIsCrossing(FVector const& ActorLocation, FVector const& PortalLocation,
                                            FVector const& PortalDirection, bool& out_LastInFront, FVector& out_LastPosition)
{
	// Create imaginary plane at PortalLocation facing normalized PortalDirection
	// So it only cares the direction it's facing, ignoring lenght.
	FPlane PortalPlane = FPlane(PortalLocation, PortalDirection);
	FVector IntersectionPoint;

	// Draws a line from out_LastPosition to ActorLocation, return true if this line intersects the PortalPlane
	// If true, will set IntersectionPoint with the location (FVector) of the intersection location
	// Else will return false
	bool bIsIntersecting = FMath::SegmentPlaneIntersection(out_LastPosition, ActorLocation,
														   PortalPlane, IntersectionPoint);

	// Calls CheckIsInFront function to check if is in front (true) or behind (false)
	bool bIsInFront = CheckIsInFront(ActorLocation, PortalLocation, PortalDirection);

	// bIsCrossing defaults to false or else it would allow to transport the actor even when it shouldn't
	bool bIsCrossing = false;

	// If is intersecting and isn't in front 
	if(bIsIntersecting && !bIsInFront && out_LastInFront) { bIsCrossing = true; }

	out_LastInFront = bIsInFront;
	out_LastPosition = ActorLocation;

	return bIsCrossing;
}
