// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/TriggerBox.h"
#include "GameFramework/Actor.h"
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
	
	/** Portal's number (1 or 2) */
	UPROPERTY(EditDefaultsOnly, category = "Portal", meta = (AllowPrivateAccess = "true"))
	int PortalNumber = FMath::Clamp(PortalNumber, 1,2);

	/** Scene Capture Component so the portals can show what they're seeing */
	UPROPERTY(VisibleDefaultsOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	USceneCaptureComponent2D* SceneCaptureComponent2D;
	
public:	
	// Sets default values for this actor's properties
	AGEIIProjectPortalBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

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
