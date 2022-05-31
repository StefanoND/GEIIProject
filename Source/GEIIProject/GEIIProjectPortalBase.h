// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "GEIIProjectPortalBase.generated.h"

class AGEIIProjectCharacter;
class AGEIIProjectPortalWall;

UCLASS()
class GEIIPROJECT_API AGEIIProjectPortalBase : public AActor
{
	GENERATED_BODY()
	
	/** Root Component */
	UPROPERTY(VisibleDefaultsOnly, Category = "Portal")
	USceneComponent* SceneComponent;
	
	/** Portal's Mesh */
	UPROPERTY(VisibleDefaultsOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PortalPlane;
	
	/** Portal's Rim */
	UPROPERTY(VisibleDefaultsOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PortalRim;

	/** Scene Capture Component so the portals can show what they're seeing */
	UPROPERTY(VisibleDefaultsOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	USceneCaptureComponent2D* SceneCaptureComponent2D;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneComponentForCameraManager;
	
public:	
	// Sets default values for this actor's properties
	AGEIIProjectPortalBase();

	/** Construction Script */
	virtual void OnConstruction(const FTransform& Transform) override;

	/** Set if the portal is Blue or Red */
	UFUNCTION(BlueprintCallable, Category = "Portal")
	void SetPortal(bool bSetPortalIsBlue);

	/** Get if the portal is Blue or Red */
	UFUNCTION(BlueprintCallable, Category = "Portal")
	bool GetIsBluePortal();

	/** Link newly spawned portal to the other of oposite color
	 * deletes old of the same color */
	UFUNCTION(BlueprintCallable, Category = "Portal")
	void LinkPortals(AGEIIProjectPortalBase* Portal);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Default Material of the Portals */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* DefaultBluePortalMaterial;

	/** Default Material of the Portals */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* DefaultRedPortalMaterial;

	/** Material of the Blue Portal */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* BluePortalMaterial;
	
	/** Material of the Red Portal */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* RedPortalMaterial;

	/** Material of the Blue Portal Border */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* BluePortalRimMaterial;
	
	/** Material of the Red Portal Border */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* RedPortalRimMaterial;

	/** TextureRenderTarget of the Blue Portal */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	UTextureRenderTarget2D* BluePortalRenderTarget2D;
	
	/** TextureRenderTarget of the Red Portal */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	UTextureRenderTarget2D* RedPortalRenderTarget2D;

	/** True if it's the Blue portal, False if it's the Red Portal */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "True", ExposeOnSpawn))
	bool bIsBluePortal;

	/** Portal Wall Blueprint Reference */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AGEIIProjectPortalWall> PortalWallReference;

	/** Reference to the newly linked Portal */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	AGEIIProjectPortalBase* LinkedPortal;

	/** Place the actor ahead of the portal so it doesn't spawn on the wall */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	float DistanceToPlaceActorAheadOfPortal = 10.0f;

	/** Reference to the player's camera manager for properly showing
	 * TextureRenderTarget2D contents */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	APlayerCameraManager* PlayerCameraManager;

	/** Reference to the game's viewport for properly showing
	 * TextureRenderTarget2D contents */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	UGameViewportClient* Viewport;

	/** Size of the ViewPort */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	FIntPoint ViewSize;

	/** Array that contains information if the player is currently inside a portal
	 *  to avoid spam teleport */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> PlayersInPortal;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Called when another actors starts overlapping with this actor */
	UFUNCTION(BlueprintCallable, Category = "Portal")
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Called when another actors stops overlapping with this actor */
	UFUNCTION(BlueprintCallable, Category = "Portal")
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	/** Method that will check if the player can teleport */
	UFUNCTION(BlueprintCallable, Category = "Portal")
	void CheckIfPlayerShouldTeleport(AGEIIProjectCharacter* PlayerCharacter);

	/** Method that will teleport the player */
	UFUNCTION(BlueprintCallable, Category = "Portal")
	void TeleportPlayer(AGEIIProjectCharacter* PlayerCharacter);
};
