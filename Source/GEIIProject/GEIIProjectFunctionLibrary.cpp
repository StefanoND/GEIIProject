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

FVector UGEIIProjectFunctionLibrary::ConvertLocation(FVector const& Location, AActor* Portal, AActor* Target)
{
	FVector Direction = Location - Portal->GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();

	FVector Dots;

	Dots.X = FVector::DotProduct(Direction, Portal->GetActorForwardVector());
	Dots.Y = FVector::DotProduct(Direction, Portal->GetActorRightVector());
	Dots.Z = FVector::DotProduct(Direction, Portal->GetActorUpVector());

	FVector NewDirection = Dots.X * -Target->GetActorForwardVector() +
						   Dots.Y * -Target->GetActorRightVector() +
						   Dots.Z * Target->GetActorUpVector();

	return TargetLocation + NewDirection;
}

FRotator UGEIIProjectFunctionLibrary::ConvertRotation(FRotator const& Rotation, AActor* Portal, AActor* Target)
{
	FVector RotationAdjustment(0.0f, 0.0f, -180.0f);
	FVector LocalAdjustment = FVector::ZeroVector;

	if(FVector::DotProduct(Portal->GetActorForwardVector(), FVector::UpVector) > KINDA_SMALL_NUMBER)
	{
		LocalAdjustment.X = FMath::UnwindDegrees(Portal->GetTransform().GetRotation().Euler().X);
		LocalAdjustment.Y = 180.0f;
		RotationAdjustment.Z += LocalAdjustment.X;
	}
	else if(FVector::DotProduct(Portal->GetActorForwardVector(), FVector::UpVector) < -KINDA_SMALL_NUMBER)
	{
		LocalAdjustment.X = FMath::UnwindDegrees(Portal->GetTransform().GetRotation().Euler().X);
		LocalAdjustment.Y = -180.0f;
		RotationAdjustment.Z += LocalAdjustment.X;
	}

	FQuat QuatRotation = FQuat::MakeFromEuler(RotationAdjustment) * FQuat(Rotation);
	FQuat LocalQuat = FQuat::MakeFromEuler(LocalAdjustment) *
		Portal->GetActorTransform().GetRotation().Inverse() * QuatRotation;

	FQuat NewWorldQuat = Target->GetActorTransform().GetRotation() * LocalQuat;
	
	return NewWorldQuat.Rotator();
}

bool UGEIIProjectFunctionLibrary::CheckIsInFront(FVector const& Point, FVector& PortalLocation, FVector const& PortalNormal)
{
	FPlane PortalPlane = FPlane(PortalLocation, PortalNormal);
	
	return PortalPlane.PlaneDot(Point) >= 0;
}

/*bool UGEIIProjectFunctionLibrary::CheckIsCrossing(FVector const& Point, FVector& PortalLocation, FVector const& PortalNormal, bool& out_LastInFront, FVector& out_LastPosition)
{
	FPlane PortalPlane = FPlane(PortalLocation, PortalNormal);
	FVector IntersectionPoint;

	bool IsIntersecting = FMath::SegmentPlaneIntersection(out_last)
}*/
