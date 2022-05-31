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

	UFUNCTION(BlueprintCallable, Category = "Portal")
	FVector ConvertLocation(FVector const& Location, AActor* Portal, AActor* Target);
	
	UFUNCTION(BlueprintCallable, Category = "Portal")
	FRotator ConvertRotation(FRotator const& Rotation, AActor* Portal, AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Portal")
	bool CheckIsInFront(FVector const& Point, FVector& PortalLocation, FVector const& PortalNormal);
	
	/*UFUNCTION(BlueprintCallable, Category = "Portal")
	bool CheckIsCrossing(FVector const& Point, FVector& PortalLocation, FVector const& PortalNormal, bool& out_LastInFront, FVector& out_LastPosition);*/
};
