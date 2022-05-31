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

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

