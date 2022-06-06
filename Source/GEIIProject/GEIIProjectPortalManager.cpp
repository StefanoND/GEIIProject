// Fill out your copyright notice in the Description page of Project Settings.


#include "GEIIProjectPortalManager.h"

#include "GEIIProjectPortalWall.h"

// Sets default values
AGEIIProjectPortalManager::AGEIIProjectPortalManager()
{
	// Defining default values
	TraceDistance = 2000.0f;
	MaxSpawnDistance = 10000.0f;
	PortalSpawnOffset = 1.0f;
}

void AGEIIProjectPortalManager::SpawnPortalAlongVector(FVector StartLocation, FVector Direction, bool bIsBluePortal,
	AController* PlayerController)
{
	if(PlayerController != nullptr)
	{
		FHitResult Hit;
		FVector EndLocation = StartLocation + (Direction * MaxSpawnDistance);
		FCollisionQueryParams TraceParams;

		// Helper to convert Enum as Byte so the LineTrace works properly with Object Types
		// Declaring a Object Type Query to a Enum as Byte and Assigning it to ObjectTypeQuery8 since it's the
		// Second Custom Object Channel in Project Settings>Collisions
		TEnumAsByte<EObjectTypeQuery> TraceObjectTypes = EObjectTypeQuery::ObjectTypeQuery8;

		// Array of Objects to be Queried in LineTrace
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectsToTraceAsByte;
		
		// Add our ObjectTypeQuery8 into the array 
		ObjectsToTraceAsByte.Add(TraceObjectTypes);

		// Returns if LineTrace has hit our ObjectTypeQuery8 or not
		bool bHit = GetWorld()->LineTraceSingleByObjectType(Hit, StartLocation, EndLocation, ObjectsToTraceAsByte, TraceParams);
		
		if(bHit)
		{		
			PortalWall = Cast<AGEIIProjectPortalWall>(Hit.Actor);

			FVector Trace = Hit.TraceStart - Hit.TraceEnd;
			
			//float NormalizedTrace = Trace.Normalize(0.0001f) * PortalSpawnOffset;
			FVector NormalizedTrace = Trace.GetSafeNormal(0.0001f) * PortalSpawnOffset;
			
			FVector PortalOrigin = Hit.Location + NormalizedTrace;
			
			AActor* NewPortal = PortalWall->TryAddPortal(PortalOrigin, bIsBluePortal);

			if(NewPortal != nullptr)
			{
				AGEIIProjectPortalBase* CreatedPortal = Cast<AGEIIProjectPortalBase>(NewPortal);
				
				if(CreatedPortal->GetIsBluePortal())
				{			
					SwapPortals(BluePortal, CreatedPortal);
					if(RedPortal != nullptr)
					{
						RedPortal->LinkPortals(BluePortal);	
					}
					BluePortal->LinkPortals(RedPortal);
				}
				else if(!CreatedPortal->GetIsBluePortal())
				{
					SwapPortals(RedPortal, CreatedPortal);
					if(BluePortal != nullptr)
					{
						BluePortal->LinkPortals(RedPortal);	
					}
					RedPortal->LinkPortals(BluePortal);
				}
			}
		}
	}
}

void AGEIIProjectPortalManager::SwapPortals(AGEIIProjectPortalBase* OldPortal, AGEIIProjectPortalBase* NewPortal)
{
	if(OldPortal != nullptr)
	{
		OldPortal->Destroy();
	}
	if(NewPortal->GetIsBluePortal())
	{
		BluePortal = NewPortal;
	}
	else if(!NewPortal->GetIsBluePortal())
	{
		RedPortal = NewPortal;
	}
}

float AGEIIProjectPortalManager::GetMaxSpawnDistance()
{
	return MaxSpawnDistance;
}
