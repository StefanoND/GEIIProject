// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GEIIProjectPortalManager.h"
#include "GEIIProjectPortalWall.h"
#include "GameFramework/GameStateBase.h"
#include "GEIIProjectGameState.generated.h"

/**
 * 
 */
UCLASS()
class GEIIPROJECT_API AGEIIProjectGameState : public AGameStateBase
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	AGEIIProjectGameState();

	virtual void BeginPlay() override;
	
public:	

	AGEIIProjectPortalManager* GetPortalManagerReference();

protected:
	class AGEIIProjectPortalManager* PortalManager;
};
