// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GEIIProjectPortalWall.generated.h"

UCLASS()
class GEIIPROJECT_API AGEIIProjectPortalWall : public AActor
{
	GENERATED_BODY()
	
	/** Root Component */
	UPROPERTY(VisibleDefaultsOnly, Category = "Portal")
	USceneComponent* SceneComponent;
	
	/** Portal's Output image */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Plane;
	
public:	
	// Sets default values for this actor's properties
	AGEIIProjectPortalWall();

	UFUNCTION(BlueprintCallable, Category = "Portal")
	void TryAddPortal(FVector PortalOrigin, bool bIsBluePortal);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION(Category = "Portal")
	float ConstrainPortalToWallY(float PortalY);
	float ConstrainPortalToWallZ(float PortalZ);

	UFUNCTION(Category = "Portal")
	float ClampPointToWall(float Point, float WallSize, float PortalRadius);

	UFUNCTION(Category = "Portal")
	float PortalRadius(float Radius);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	float WallWidth = 100.0f;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	float WallHeight = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "True"))
	float PortalRadiusY = 249.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "True"))
	float PortalRadiusZ = 180.0f;
};
