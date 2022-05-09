// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GEIIProjectPortalWall.h"
#include "Components/BoxComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/TriggerBox.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "GEIIProjectPortalBase.generated.h"

UCLASS()
class GEIIPROJECT_API AGEIIProjectPortalBase : public AActor
{
	GENERATED_BODY()
	
	/** Root Component */
	UPROPERTY(VisibleDefaultsOnly, Category = "Portal")
	USceneComponent* SceneComponent;
	
	/** Portal's Trigger Box so the player can teleport to the other portal */
	UPROPERTY(VisibleDefaultsOnly, Category = "Portal", meta = (AllowPrivateAccess = "True"))
	UBoxComponent* TriggerBox;
	
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

	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable, Category = "Portal")
	void SetPortal(bool bSetPortalIsBlue);

	UFUNCTION(BlueprintCallable, Category = "Portal")
	bool GetIsBluePortal();

	UFUNCTION(BlueprintCallable, Category = "Portal")
	void LinkPortals(AGEIIProjectPortalBase* Portal);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Rim Material of the Blue Portal */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* DefaultPortalMaterial;

	/** Rim Material of the Blue Portal */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* BluePortalMaterial;
	
	/** Rim Material of the Red Portal */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* RedPortalMaterial;

	/** Rim Material of the Blue Portal */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* BluePortalRimMaterial;
	
	/** Rim Material of the Red Portal */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* RedPortalRimMaterial;

	/** Rim Material of the Blue Portal */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	UTextureRenderTarget2D* BluePortalRenderTarget2D;
	
	/** Rim Material of the Red Portal */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	UTextureRenderTarget2D* RedPortalRenderTarget2D;

	/** True if it's the Blue portal, False if it's the Red Portal */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "True", ExposeOnSpawn))
	bool bIsBluePortal;

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AGEIIProjectPortalWall> PortalWallReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	AGEIIProjectPortalBase* LinkedPortal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	APlayerCameraManager* PlayerCameraManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	UGameViewportClient* Viewport;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	FIntPoint ViewSize;
	
	/** Will rotate the Texture Render Target 2D to the player location so
	 * it's not just showing a static image*/
	//void RotatePortalView();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Called when another actors starts overlapping with this actor */
	UFUNCTION(BlueprintCallable, Category = "Portal")
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Called when another actors stops overlapping with this actor */
	UFUNCTION(BlueprintCallable, Category = "Portal")
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
