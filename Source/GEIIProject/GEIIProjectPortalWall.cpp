// Fill out your copyright notice in the Description page of Project Settings.


#include "GEIIProjectPortalWall.h"

#include "GEIIProjectFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AGEIIProjectPortalWall::AGEIIProjectPortalWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = SceneComponent;
	
	Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
	Plane->SetupAttachment(RootComponent);
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

void AGEIIProjectPortalWall::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	Plane->SetWorldScale3D(FVector(1.0f, WallWidth / 100, WallHeight / 100));
}

AActor* AGEIIProjectPortalWall::TryAddPortal(FVector PortalOrigin, bool bIsBluePortal)
{
	//FVector ActorLocation = GetTransform().GetLocation();
	//FVector RelativePortalOrigin = GetActorTransform().InverseTransformPosition(RelativePortalOrigin);
	FVector RelativePortalOrigin = UKismetMathLibrary::InverseTransformLocation(GetActorTransform(), PortalOrigin);
	float ConstrainedY = ConstrainPortalToWallY(RelativePortalOrigin.Y);
	float ConstrainedZ = ConstrainPortalToWallZ(RelativePortalOrigin.Z);
	RelativePortalOrigin = FVector(RelativePortalOrigin.X, ConstrainedY, ConstrainedZ);

	UWorld* const World = GetWorld();
		
	if (World != nullptr)
	{
		AGEIIProjectPortalBase* PortalBlueprintReference;
		
		FTransform PortalTransform;
		PortalTransform.SetLocation(UKismetMathLibrary::TransformLocation(GetActorTransform(), RelativePortalOrigin));
		FRotator PortalRotation = GetActorRotation();
		PortalTransform.SetRotation(PortalRotation.Quaternion());

		if(HasRoomForNewPortal(RelativePortalOrigin.Y, RelativePortalOrigin.Z))
		{
			PortalBlueprintReference = World->SpawnActorDeferred<AGEIIProjectPortalBase>(PortalBaseReference, PortalTransform,
									   GetOwner(), GetInstigator(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			
			PortalBlueprintReference->SetPortal(bIsBluePortal);
			PortalBlueprintReference->FinishSpawning(PortalTransform);
			PortalsOnWall.Add(PortalBlueprintReference);
			
			PortalBlueprintReference->OnDestroyed.AddDynamic(this, &AGEIIProjectPortalWall::DestroyPortal);
			
			return PortalBlueprintReference;
		}
	}
	return nullptr;
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

	if(Point >= 0.0f)
	{
		Offset = -1.0f * Offset;
		return  Point - Offset;
	}
	
	return Point - Offset;
}

float AGEIIProjectPortalWall::PortalRadius(float Radius)
{
	return Radius / 2;
}

void AGEIIProjectPortalWall::DestroyPortal(AActor* Actor)
{
	PortalsOnWall.Remove(Actor);
}

bool AGEIIProjectPortalWall::HasRoomForNewPortal(float NewPortalY, float NewPortalZ)
{
	for (AActor* PortalOnWall : PortalsOnWall)
	{
		//FVector RelativePortalOrigin = GetActorTransform().InverseTransformPosition(RelativePortalOrigin);
		//FVector PortalLocation = UKismetMathLibrary::InverseTransformLocation(GetActorTransform(), PortalOnWall->GetActorLocation());
		FVector PortalLocation = GetActorTransform().InverseTransformPosition(PortalOnWall->GetActorLocation());
		FVector2D PortalLocation2D = FVector2D(PortalLocation.Y, PortalLocation.Z);
		FVector2D PortalRadius2D = FVector2D(PortalRadius(PortalRadiusY), PortalRadius(PortalRadiusZ));
		FVector2D NewPortalOrigin = FVector2D(NewPortalY, NewPortalZ);
		
		if(UGEIIProjectFunctionLibrary::RectangleToRectangleCollision(PortalLocation2D,
			PortalRadius2D, NewPortalOrigin, PortalRadius2D))
		{
			return false;
		}
	}
	return true;
}
