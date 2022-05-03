// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GEIIProjectPortalBase.generated.h"

UCLASS()
class GEIIPROJECT_API AGEIIProjectPortalBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGEIIProjectPortalBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Will rotate the Texture Render Target 2D to the player location so
	 * it's not just showing a static image*/
	void RotatePortalView()
	{
		
	}

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
protected:
};
