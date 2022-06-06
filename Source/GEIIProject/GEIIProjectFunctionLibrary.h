// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GEIIProjectCharacter.h"
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

	// Calculates the Actor is teleport location
	UFUNCTION(BlueprintCallable, Category = "Portal")
	static FVector ConvertLocation(FVector const& CurrentLocation, AActor* OriginPortal, AActor* DestinationPortal);

	// Calculates the Actor is teleport rotation
	UFUNCTION(BlueprintCallable, Category = "Portal")
	static FRotator ConvertRotation(FRotator const& CurrentRotation, AActor* OriginPortal, AActor* DestinationPortal);
	
	UFUNCTION(BlueprintCallable, Category = "Porta")
	static FVector ConvertVelocity(AGEIIProjectCharacter* PlayerCharacter, FVector const& Location, FRotator const& Rotation);

	/** Checks if the Actor is in front of the portal */
	UFUNCTION(BlueprintCallable, Category = "Portal")
	static bool CheckIsInFront(FVector const& ActorLocation, FVector const& PortalLocation, FVector const& PortalDirection);
	
	UFUNCTION(BlueprintCallable, Category = "Portal")
	static bool CheckIsCrossing(FVector const& ActorLocation, FVector const& PortalLocation, FVector const& PortalDirection,
								bool& out_LastInFront, FVector& out_LastPosition);
};
