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
	UFUNCTION(BlueprintCallable, Category = "Portal")
	static bool RectangleToRectangleCollision(FVector2D Rectangle1Origin, FVector2D Rectangle1Extents, FVector2D Rectangle2Origin, FVector2D Rectangle2Extents);

	UFUNCTION(BlueprintCallable, Category = "Portal")
	static void ResizeRenderTarget(UTextureRenderTarget2D* RenderTarget, float SizeX, float SizeY);
};
