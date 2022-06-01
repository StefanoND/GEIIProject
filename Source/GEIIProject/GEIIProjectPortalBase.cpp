// Fill out your copyright notice in the Description page of Project Settings.


#include "GEIIProjectPortalBase.h"

#include "EngineUtils.h"
#include "GEIIProjectCharacter.h"
#include "GEIIProjectFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

class UGEIIProjectFunctionLibrary;

// Sets default values
AGEIIProjectPortalBase::AGEIIProjectPortalBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = SceneComponent;
	
	PortalPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalPlane"));
	PortalPlane->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PortalPlane->SetupAttachment(RootComponent);
	PortalPlane->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	
	PortalRim = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalRim"));
	PortalRim->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PortalRim->SetupAttachment(RootComponent);
	PortalRim->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	
	SceneCaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent2D"));
	SceneCaptureComponent2D->SetupAttachment(RootComponent);
	SceneCaptureComponent2D->FOVAngle = 90.0f;
	SceneCaptureComponent2D->bOverride_CustomNearClippingPlane = true;

	SceneComponentForCameraManager = CreateDefaultSubobject<USceneComponent>(TEXT("BackFacingScene"));
	SceneComponentForCameraManager->SetupAttachment(RootComponent);
	SceneComponentForCameraManager->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	
	// Assigning OnBegin and OnEnd overlap events to our methods
	PortalPlane->OnComponentBeginOverlap.AddDynamic(this, &AGEIIProjectPortalBase::OnOverlapBegin);
	PortalPlane->OnComponentEndOverlap.AddDynamic(this, &AGEIIProjectPortalBase::OnOverlapEnd);
}

void AGEIIProjectPortalBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	if(bIsBluePortal)
	{
		if(BluePortalMaterial != nullptr && BluePortalRimMaterial != nullptr && BluePortalRenderTarget2D != nullptr)
		{
			PortalPlane->UMeshComponent::SetMaterial(0, DefaultBluePortalMaterial);
			PortalRim->UMeshComponent::SetMaterial(0, BluePortalRimMaterial);
			SceneCaptureComponent2D->TextureTarget = BluePortalRenderTarget2D;
		}
	}
	if(!bIsBluePortal)
	{
		if(RedPortalMaterial != nullptr && RedPortalRimMaterial != nullptr && RedPortalRenderTarget2D != nullptr)
		{
			PortalPlane->UMeshComponent::SetMaterial(0, DefaultRedPortalMaterial);
			PortalRim->UMeshComponent::SetMaterial(0, RedPortalRimMaterial);	
			SceneCaptureComponent2D->TextureTarget = RedPortalRenderTarget2D;
		}
	}
}

// Called when the game starts or when spawned
void AGEIIProjectPortalBase::BeginPlay()
{
	Super::BeginPlay();

	PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
}

// Called every frame
void AGEIIProjectPortalBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(LinkedPortal != nullptr) 
	{
		Viewport = GetWorld()->GetGameViewport();
		ViewSize = Viewport->Viewport->GetSizeXY();
		
		UGEIIProjectFunctionLibrary::ResizeRenderTarget(SceneCaptureComponent2D->TextureTarget, ViewSize.X, ViewSize.Y);
		
		USceneComponent* CameraTransformComponent = PlayerCameraManager->GetTransformComponent();
		FTransform PlayerCameraTransform = CameraTransformComponent->GetComponentTransform();
		FTransform SceneComponentTransform = SceneComponentForCameraManager->GetComponentTransform();
		
		FTransform RelativeTransform = UKismetMathLibrary::MakeRelativeTransform(PlayerCameraTransform, SceneComponentTransform);
		FVector RelativeLocation = RelativeTransform.GetLocation();
		FQuat RelativeRotation = RelativeTransform.GetRotation();
		
		LinkedPortal->SceneCaptureComponent2D->SetRelativeLocationAndRotation(RelativeLocation, RelativeRotation);

		float MinimumClippingPlane = CameraTransformComponent->GetComponentLocation().Distance(CameraTransformComponent->GetComponentLocation(), GetActorLocation()) + 1.0f;
		
		LinkedPortal->SceneCaptureComponent2D->CustomNearClippingPlane = MinimumClippingPlane;

		for (AActor* Player : PlayersInPortal)
		{
			AGEIIProjectCharacter* PlayerCharacter = Cast<AGEIIProjectCharacter>(Player);
			CheckIfPlayerShouldTeleport(PlayerCharacter);
		}
	}
}

void AGEIIProjectPortalBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	if(LinkedPortal != nullptr)
	{
		AGEIIProjectCharacter* PlayerCharacter = Cast<AGEIIProjectCharacter>(OtherActor);

		if(PlayerCharacter != nullptr)
		{
			if(OtherComp == PlayerCharacter->GetCapsuleComponent())
			{
				int PlayerIndex = PlayersInPortal.AddUnique(PlayerCharacter);
			
				if(PlayerIndex != -1)
				{
					const FName PortalPawn = "PortalPawn";
					PlayerCharacter->GetCapsuleComponent()->SetCollisionProfileName(PortalPawn, true);
				}
			}
		}	
	}
}

void AGEIIProjectPortalBase::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AGEIIProjectCharacter* PlayerCharacter = Cast<AGEIIProjectCharacter>(OtherActor);
	
	if(PlayerCharacter != nullptr)
	{
		if(OtherComp == PlayerCharacter->GetCapsuleComponent())
		{
			const FName Pawn = "Pawn";
			PlayersInPortal.Remove(PlayerCharacter);
			PlayerCharacter->GetCapsuleComponent()->SetCollisionProfileName(Pawn, true);
		}
	}
}

void AGEIIProjectPortalBase::SetPortal(bool bSetPortalIsBlue)
{
	bIsBluePortal = bSetPortalIsBlue;
}

bool AGEIIProjectPortalBase::GetIsBluePortal()
{
	return  bIsBluePortal;
}

void AGEIIProjectPortalBase::LinkPortals(AGEIIProjectPortalBase* Portal)
{
	if(Portal != nullptr)
	{
		LinkedPortal = Portal;
		
		if(LinkedPortal != nullptr)
		{
			if(bIsBluePortal)
			{
				LinkedPortal->PortalPlane->SetMaterial(0, BluePortalMaterial);
			}
			if(!bIsBluePortal)
			{
				LinkedPortal->PortalPlane->SetMaterial(0, RedPortalMaterial);
			}
		}
		else if(LinkedPortal == nullptr)
		{
			if(LinkedPortal->bIsBluePortal)
			{
				LinkedPortal->PortalPlane->SetMaterial(0, DefaultBluePortalMaterial);	
			}
			else if(!LinkedPortal->bIsBluePortal)
			{
				LinkedPortal->PortalPlane->SetMaterial(0, DefaultRedPortalMaterial);	
			}
		}
	}
}

void AGEIIProjectPortalBase::CheckIfPlayerShouldTeleport(AGEIIProjectCharacter* PlayerCharacter)
{
	FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	/*FVector PlayerVelocity = PlayerCharacter->GetVelocity() * UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
	FVector PlayerFutureLocation = PlayerLocation + PlayerVelocity;
	FVector FromPortalToPlayerLocation = PlayerFutureLocation - GetActorLocation();
	FVector Normalized = FromPortalToPlayerLocation.GetSafeNormal(0.0001f);*/

	// If Player is behind portal and is actively moving forwards
	/*if(FVector::DotProduct(Normalized, GetActorForwardVector()) <= ThresholdBehindPortal)*//* &&
	   FVector::DotProduct(PlayerCharacter->GetLastMovementInputVector().GetSafeNormal(0.0001f),
	   					   GetActorForwardVector()) < 0.0f)*/
	if(UGEIIProjectFunctionLibrary::CheckIsCrossing(PlayerLocation, this->GetActorLocation(),
										 this->GetActorForwardVector(), bLastInFront, LastPosition))
	{
		TeleportPlayer(PlayerCharacter);
	}
}

void AGEIIProjectPortalBase::TeleportPlayer(AGEIIProjectCharacter* PlayerCharacter)
{
	AController* PlayerController = PlayerCharacter->GetController();

	FVector Location = UGEIIProjectFunctionLibrary::ConvertLocation(PlayerCharacter->GetActorLocation(), this, LinkedPortal);
	FRotator Rotation = UGEIIProjectFunctionLibrary::ConvertRotation(PlayerController->GetControlRotation(), this, LinkedPortal); 
	
	PlayerCharacter->SetActorLocation(Location, false, nullptr, ETeleportType::TeleportPhysics);
	PlayerController->SetControlRotation(Rotation);
	
	FVector RelativePlayerVelocity = UKismetMathLibrary::InverseTransformDirection(PlayerCharacter->GetActorTransform(),
																			PlayerCharacter->GetVelocity());
	FTransform ControlPlayerTransform = UKismetMathLibrary::MakeTransform(PlayerCharacter->GetActorLocation(),
										PlayerController->GetControlRotation(), FVector::OneVector);
	FVector NewRelativeVelocity = UKismetMathLibrary::TransformDirection(ControlPlayerTransform, RelativePlayerVelocity);
	PlayerCharacter->GetMovementComponent()->Velocity = NewRelativeVelocity;
	/*AController* PlayerController = PlayerCharacter->GetController();
	UCameraComponent* FirstPersonCamera = PlayerCharacter->GetFirstPersonCameraComponent();

	FTransform PlayerTransform = PlayerCharacter->GetActorTransform();
	FVector PlayerVelocity = PlayerCharacter->GetVelocity();
	FVector RelativePlayerVelocity = UKismetMathLibrary::InverseTransformDirection(PlayerTransform, PlayerVelocity);
	
	FTransform BackFacingSceneTransform = SceneComponentForCameraManager->GetComponentTransform();
	FTransform RelativeTransform = UKismetMathLibrary::MakeRelativeTransform(FirstPersonCamera->GetComponentTransform(), BackFacingSceneTransform);
	FTransform MultipliedTransforms = UKismetMathLibrary::ComposeTransforms(RelativeTransform, LinkedPortal->GetActorTransform());

	FVector MultipliedLocation = MultipliedTransforms.GetLocation();
	FRotator MultipliedRotation = MultipliedTransforms.GetRotation().Rotator();
	
	FVector LinkedPortalForwardVector = LinkedPortal->GetActorForwardVector() * DistanceToPlaceActorAheadOfPortal;
	FVector NewCameraRelativeLocation = MultipliedLocation - FirstPersonCamera->GetRelativeLocation();
	FVector LinkedLocation = LinkedPortalForwardVector + NewCameraRelativeLocation;
	
	PlayerCharacter->SetActorLocation(LinkedLocation, false, nullptr, ETeleportType::TeleportPhysics);
	PlayerController->SetControlRotation(UKismetMathLibrary::MakeRotator(0.0f, MultipliedRotation.Pitch, MultipliedRotation.Yaw));

	FTransform ControlPlayerTransform = UKismetMathLibrary::MakeTransform(PlayerCharacter->GetActorLocation(), PlayerController->GetControlRotation(), FVector::OneVector);
	FVector NewRelativeVelocity = UKismetMathLibrary::TransformDirection(ControlPlayerTransform, RelativePlayerVelocity);
	
	PlayerCharacter->GetMovementComponent()->Velocity = NewRelativeVelocity;*/
}
