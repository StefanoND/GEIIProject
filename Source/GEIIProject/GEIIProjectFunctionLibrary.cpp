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
