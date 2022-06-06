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
	//SceneCaptureComponent2D->bUseCustomProjectionMatrix = true;
	SceneCaptureComponent2D->bCameraCutThisFrame = false;

	SceneComponentForCameraManager = CreateDefaultSubobject<USceneComponent>(TEXT("BackFacingScene"));
	SceneComponentForCameraManager->SetupAttachment(RootComponent);
	SceneComponentForCameraManager->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoxComponent->SetupAttachment(RootComponent);
	
	// Assigning OnBegin and OnEnd overlap events to our methods
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGEIIProjectPortalBase::OnBoxBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AGEIIProjectPortalBase::OnBoxEndOverlap);
}

void AGEIIProjectPortalBase::OnConstruction(const FTransform& Transform)
{
	//Super::OnConstruction(Transform);
	
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

void AGEIIProjectPortalBase::PostEditMove(bool bFinished)
{
	OnConstruction(GetTransform());
}


// Called when the game starts or when spawned
void AGEIIProjectPortalBase::BeginPlay()
{
	Super::BeginPlay();

	PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	
	FVector Location = GetActorForwardVector() * 40.0f;
	RootComponent->SetRelativeLocation(GetActorLocation() - Location);
	
	FTimerHandle TimerHandle;
	//GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGEIIProjectPortalBase::MoveActor, 1)
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AGEIIProjectPortalBase::MoveActor, 1, false, DelayTimer);
	
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
		
		FTransform RelativeTransform = PlayerCameraTransform.GetRelativeTransform(SceneComponentTransform);
		FVector Subtract = FVector::ZeroVector;
		Subtract = PlayerCameraManager->GetActorForwardVector() * CameraDistance;
		FVector RelativeLocation = RelativeTransform.GetLocation() - Subtract;
		FQuat RelativeRotation = RelativeTransform.GetRotation();
		
		LinkedPortal->SceneCaptureComponent2D->SetRelativeLocationAndRotation(RelativeLocation, RelativeRotation);

		float MinimumClippingPlane = CameraTransformComponent->GetComponentLocation().Distance(CameraTransformComponent->GetComponentLocation(), GetActorLocation()) + 1.0f;
		
		LinkedPortal->SceneCaptureComponent2D->CustomNearClippingPlane = MinimumClippingPlane;

		if(bIsOverlapping)
		{
			// Commented out since it was giving "Array changed during ranged-for iteration" errors
			//for (AActor* Player : PlayersInPortal)
			for (int32 i = 0; i < PlayersInPortal.Num() ; i++)
			{
				AActor* Player = PlayersInPortal[i];
				PlayerCharacterReference = Cast<AGEIIProjectCharacter>(Player);
				CheckIfPlayerShouldTeleport(PlayerCharacterReference);
			}
		}
	}
}

void AGEIIProjectPortalBase::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(LinkedPortal != nullptr)
	{
		PlayerCharacterReference = Cast<AGEIIProjectCharacter>(OtherActor);

		if(PlayerCharacterReference != nullptr && !PlayersInPortal.Contains(PlayerCharacterReference))
		{
			if(OtherComp == PlayerCharacterReference->GetCapsuleComponent())
			{
				int PlayerIndex = PlayersInPortal.AddUnique(PlayerCharacterReference);	
			
				if(PlayerIndex != -1)
				{
					bIsOverlapping = true;
					const FName PortalPawn = "PortalPawn";
					PlayerCharacterReference->GetCapsuleComponent()->SetCollisionProfileName(PortalPawn, true);
				}
			}
		}	
	}
}

void AGEIIProjectPortalBase::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	PlayerCharacterReference = Cast<AGEIIProjectCharacter>(OtherActor);
	
	if(PlayerCharacterReference != nullptr)
	{
		if(OtherComp == PlayerCharacterReference->GetCapsuleComponent())
		{
			if(PlayersInPortal.Contains(PlayerCharacterReference))
			{
				PlayersInPortal.Remove(PlayerCharacterReference);
			}
			if(PlayersInPortal.Num() <= 0)
			{
				bIsOverlapping = false;
			}
			
			const FName Pawn = "Pawn";
			PlayerCharacterReference->GetCapsuleComponent()->SetCollisionProfileName(Pawn, true);
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
	if(bIsOverlapping)
	{
		TeleportPlayer(PlayerCharacter);
	}
}

void AGEIIProjectPortalBase::TeleportPlayer(AGEIIProjectCharacter* PlayerCharacter)
{
	if(UGEIIProjectFunctionLibrary::CheckIsCrossing(PlayerCharacter->GetActorLocation(),
		this->GetActorLocation(), this->GetActorForwardVector(), bLastInFront,
		LastPosition))
	{
	AController* PlayerController = PlayerCharacter->GetController();
	
	FRotator Rotation = UGEIIProjectFunctionLibrary::ConvertRotation(PlayerController->GetControlRotation(), this, LinkedPortal);
	FVector Location = UGEIIProjectFunctionLibrary::ConvertLocation(LastPosition, this, LinkedPortal);
		
	PlayerCharacter->SetActorLocation(Location, false, nullptr, ETeleportType::TeleportPhysics);
	PlayerController->SetControlRotation(FRotator(Rotation.Pitch, Rotation.Yaw, 0.0f));
	
	FVector Velocity = UGEIIProjectFunctionLibrary::ConvertVelocity(PlayerCharacter, Location, Rotation);

	PlayerCharacter->GetMovementComponent()->Velocity = Velocity;
	}
}

void AGEIIProjectPortalBase::MoveActor()
{
	FVector Location = GetActorForwardVector() * 40.0f;
	RootComponent->SetRelativeLocation(GetActorLocation() + Location);
}
