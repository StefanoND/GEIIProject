// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GEIIProjectHUD.generated.h"

UCLASS()
class AGEIIProjectHUD : public AHUD
{
	GENERATED_BODY()

public:
	AGEIIProjectHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	UFUNCTION(BlueprintCallable, Category = "Portal")
	void SetOpaqueCrosshair();

	UFUNCTION(BlueprintCallable, Category = "Portal")
	void SetTransparentCrosshair();
	
private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;
	
	/** Crosshair asset pointer */
	class UTexture2D* OpaqueCrosshairTex;
	
	/** Crosshair asset pointer */
	class UTexture2D* TransparentCrosshairTex;

};

