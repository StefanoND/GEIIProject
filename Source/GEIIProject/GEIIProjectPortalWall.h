// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GEIIProjectPortalBase.h"
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

	/** Method that will try to add a portal if possible */
	UFUNCTION(BlueprintCallable, Category = "Portal")
	AActor* TryAddPortal(FVector PortalOrigin, bool bIsBluePortal);

	/** Event (OnDestroyed) that gets called when portal gets destroyed */
	UFUNCTION(BlueprintCallable, Category = "Portal")
	void DestroyPortal(AActor* Actor);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Method that will stop the portal spawning with it's edges outside walls or
	 * on top of another portal*/
	UFUNCTION(Category = "Portal")
	float ConstrainPortalToWallY(float PortalY);
	
	UFUNCTION(Category = "Portal")
	float ConstrainPortalToWallZ(float PortalZ);

	/** Method that will be used by ConstrainPortalToWall methods for checking the portal's transform */
	UFUNCTION(Category = "Portal")
	float ClampPointToWall(float Point, float WallSize, float PortalRadius);

	/** Method that will return half of the portal's radius */
	UFUNCTION(Category = "Portal")
	float PortalRadius(float Radius);

	/** Will check if a portal can be spawned */
	UFUNCTION(BlueprintCallable, Category = "Portal")
	bool HasRoomForNewPortal(float NewPortalY, float NewPortalZ);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Construction script */
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	/** Size of the wall's Width, must change instanced walls */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	float WallWidth = 100.0f;
	
	/** Size of the wall's Width, must change instanced walls */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	float WallHeight = 100.0f;

	/** Portal height. Must be the same as the Mesh's height */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "True"))
	float PortalRadiusY = 180.0f;

	/** Portal Width. Must be the same as the Mesh's width */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "True"))
	float PortalRadiusZ = 249.0f;

	/** Reference to the Portal Blueprint */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "True"))
	TSubclassOf<class AGEIIProjectPortalBase> PortalBaseReference;

	/** Array that will contain the portals spawned on it */
	TArray<AActor*> PortalsOnWall;
};
