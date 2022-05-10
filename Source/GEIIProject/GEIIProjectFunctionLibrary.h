// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GEIIProjectFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class GEIIPROJECT_API UGEIIProjectFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Checks if the portal about to be spawned will collide with the one already in a wall */
	UFUNCTION(BlueprintCallable, Category = "Portal")
	static bool RectangleToRectangleCollision(FVector2D Rectangle1Origin, FVector2D Rectangle1Extents, FVector2D Rectangle2Origin, FVector2D Rectangle2Extents);

	/** Changes TextureRenderTarget Transforms so it's more realistic when the player's moving around */
	UFUNCTION(BlueprintCallable, Category = "Portal")
	static void ResizeRenderTarget(UTextureRenderTarget2D* RenderTarget, float SizeX, float SizeY);
};
