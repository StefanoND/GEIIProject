// Fill out your copyright notice in the Description page of Project Settings.


#include "GEIIProjectPortalWall.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AGEIIProjectPortalWall::AGEIIProjectPortalWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = SceneComponent;
	
	Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
	//Plane->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Plane->SetupAttachment(RootComponent);
	//Plane->SetRelativeRotation(FRotator(0.0f, -90.0f, 90.0f));
	//Plane->SetRelativeScale3D(FVector(2.0f, 3.0f, 1.0f));
}

// Called when the game starts or when spawned
void AGEIIProjectPortalWall::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGEIIProjectPortalWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGEIIProjectPortalWall::TryAddPortal(FVector PortalOrigin, bool bIsBluePortal)
{
	//FVector ActorLocation = GetTransform().GetLocation();
	//FVector RelativePortalOrigin = GetActorTransform().InverseTransformPosition(RelativePortalOrigin);
	FVector RelativePortalOrigin = UKismetMathLibrary::InverseTransformLocation(GetActorTransform(), PortalOrigin);
	float ConstrainedY = ConstrainPortalToWallY(RelativePortalOrigin.Y);
	float ConstrainedZ = ConstrainPortalToWallZ(RelativePortalOrigin.Z);
	RelativePortalOrigin = FVector(RelativePortalOrigin.X, ConstrainedY, ConstrainedZ);
	
	DrawDebugSphere(GetWorld(), UKismetMathLibrary::TransformLocation(GetActorTransform(), RelativePortalOrigin), 20.0f, 12, FColor::White, false, 3.0f, 2.0f);
	
	// Todo Check Collision with other portals
}

float AGEIIProjectPortalWall::ConstrainPortalToWallY(float PortalY)
{
	float ClampedY = ClampPointToWall(PortalY, WallWidth, PortalRadius(PortalRadiusY));

	return ClampedY;
}

float AGEIIProjectPortalWall::ConstrainPortalToWallZ(float PortalZ)
{
	float ClampedZ = ClampPointToWall(PortalZ, WallHeight, PortalRadius(PortalRadiusZ));

	return ClampedZ;
}
	
float AGEIIProjectPortalWall::ClampPointToWall(float Point, float WallSize, float PortalRadius)
{
	float Offset = FMath::Clamp(WallSize / 2 - PortalRadius - FMath::Abs(Point), -9223372036854775808.0f, 0.0f);
	
	if(Point > 0)
	{
		Offset *= -1;
	}

	return Point - Offset;
}

float AGEIIProjectPortalWall::PortalRadius(float Radius)
{
	return Radius / 2;
}
