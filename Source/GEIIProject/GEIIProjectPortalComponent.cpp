// Fill out your copyright notice in the Description page of Project Settings.


#include "GEIIProjectPortalComponent.h"

#include "GEIIProjectPortalWall.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/EngineTypes.h"

// Sets default values for this component's properties
UGEIIProjectPortalComponent::UGEIIProjectPortalComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Defining default values
	TraceDistance = 2000.0f;
	MaxSpawnDistance = 10000.0f;
	PortalSpawnOffset = 1.0f;
	// ...
}


// Called when the game starts
void UGEIIProjectPortalComponent::BeginPlay()
{
	Super::BeginPlay();

	// Getting reference to the player controller
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	// ...
	
}

// Called every frame
void UGEIIProjectPortalComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGEIIProjectPortalComponent::SpawnPortalAlongVector(FVector StartLocation, FVector Direction, bool bIsBluePortal)
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
			float NormalizedTrace = Trace.Normalize(0.0001f) * PortalSpawnOffset;
			
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

void UGEIIProjectPortalComponent::SwapPortals(AGEIIProjectPortalBase* OldPortal, AGEIIProjectPortalBase* NewPortal)
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