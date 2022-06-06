// Fill out your copyright notice in the Description page of Project Settings.


#include "GEIIProjectGameState.h"


AGEIIProjectGameState::AGEIIProjectGameState()
{
}

void AGEIIProjectGameState::BeginPlay()
{
	PortalManager = GetWorld()->SpawnActor<AGEIIProjectPortalManager>(AGEIIProjectPortalManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
}

AGEIIProjectPortalManager* AGEIIProjectGameState::GetPortalManagerReference()
{
	return PortalManager;
}