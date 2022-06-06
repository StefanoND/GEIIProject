// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GEIIProjectPortalBase.h"
#include "GameFramework/Actor.h"
#include "GEIIProjectPortalManager.generated.h"

UCLASS()
class GEIIPROJECT_API AGEIIProjectPortalManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGEIIProjectPortalManager();

	/** Spawn portal method */
	UFUNCTION(BlueprintCallable, Category = "Portal")
	void SpawnPortalAlongVector(FVector StartLocation, FVector Direction, bool bIsBluePortal, AController* PlayerController);

protected:
	/** Will spawn new portals if none are spawned, and swap old portals for the new ones */
	UFUNCTION(BlueprintCallable, Category = "Portal")
	void SwapPortals(AGEIIProjectPortalBase* OldPortal, AGEIIProjectPortalBase* NewPortal);

public:
	float GetMaxSpawnDistance();

protected:
	/** How far the linetrace should go for checking walls that can spawn portals */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	float TraceDistance;

	/** The maximum distance the player can be to the wall to spawn a portal */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	float MaxSpawnDistance;

	/** Offset the portal forwards so it doesn't flicker (ZFight) with the wall it spawned in */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	float PortalSpawnOffset;

	/** Reference to the Portal Wall */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	AGEIIProjectPortalWall* PortalWall;

	/** Reference to the Blue Portal */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	AGEIIProjectPortalBase* BluePortal;

	/** Reference to the Red Portal */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	AGEIIProjectPortalBase* RedPortal;

};
