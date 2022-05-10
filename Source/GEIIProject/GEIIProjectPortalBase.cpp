// Fill out your copyright notice in the Description page of Project Settings.


#include "GEIIProjectPortalBase.h"

#include "EngineUtils.h"
#include "GEIIProjectCharacter.h"
#include "GEIIProjectFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

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
	if(DefaultPortalMaterial != nullptr)
	{
		PortalPlane->SetMaterial(0, DefaultPortalMaterial);
	}
	
	PortalRim = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalRim"));
	PortalRim->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PortalRim->SetupAttachment(RootComponent);
	PortalRim->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	
	SceneCaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent2D"));
	SceneCaptureComponent2D->SetupAttachment(RootComponent);
	SceneCaptureComponent2D->FOVAngle = 90.0f;
	SceneCaptureComponent2D->bOverride_CustomNearClippingPlane = true;
	
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetCollisionProfileName("Trigger");

	SceneComponentForCameraManager = CreateDefaultSubobject<USceneComponent>(TEXT("BackFacingScene"));
	SceneComponentForCameraManager->SetupAttachment(RootComponent);
	SceneComponentForCameraManager->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	
	// Assigning OnBegin and OnEnd overlap events to our methods
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AGEIIProjectPortalBase::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AGEIIProjectPortalBase::OnOverlapEnd);
}

void AGEIIProjectPortalBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	if(bIsBluePortal)
	{
		if(BluePortalMaterial != nullptr && BluePortalRimMaterial != nullptr && BluePortalRenderTarget2D != nullptr)
		{
			//PortalPlane->UMeshComponent::SetMaterial(0, BluePortalMaterial);
			PortalRim->UMeshComponent::SetMaterial(0, BluePortalRimMaterial);
			SceneCaptureComponent2D->TextureTarget = BluePortalRenderTarget2D;
		}
	}
	if(!bIsBluePortal)
	{
		if(RedPortalMaterial != nullptr && RedPortalRimMaterial != nullptr && RedPortalRenderTarget2D != nullptr)
		{
			//PortalPlane->UMeshComponent::SetMaterial(0, RedPortalMaterial);
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
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "Overlap being");
	
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
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "Overlap end");

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
			UE_LOG(LogTemp, Warning, TEXT("Unlinked"));
			LinkedPortal->PortalPlane->SetMaterial(0, DefaultPortalMaterial);
		}
	}
}

void AGEIIProjectPortalBase::CheckIfPlayerShouldTeleport(AGEIIProjectCharacter* PlayerCharacter)
{
	FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	FVector PlayerVelocity = PlayerCharacter->GetVelocity() * GetWorld()->GetDeltaSeconds();
	FVector PlayerFutureLocation = PlayerLocation + PlayerVelocity;
	FVector FromPortalToPlayerLocation = PlayerFutureLocation - GetActorLocation();

	// If Player is behind portal and is actively moving forwards
	if(FVector::DotProduct(FromPortalToPlayerLocation, GetActorForwardVector()) <= 0.0f &&
	   FVector::DotProduct(PlayerCharacter->GetLastMovementInputVector(), GetActorForwardVector()) < 0.0f)
	{
		TeleportPlayer(PlayerCharacter);
	}
	
}

void AGEIIProjectPortalBase::TeleportPlayer(AGEIIProjectCharacter* PlayerCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("Teleport"));

	AController* PlayerController = PlayerCharacter->GetController();

	FTransform PlayerTransform = PlayerCharacter->GetActorTransform();
	FVector PlayerVelocity = PlayerCharacter->GetVelocity();
	
	FVector ConvertedLocation = UGEIIProjectFunctionLibrary::ConvertLocation(PlayerTransform.GetLocation(), , LinkedPortal);
	
	FVector RelativePlayerVelocity = UKismetMathLibrary::InverseTransformDirection(PlayerTransform, PlayerVelocity);

	FTransform BackFacingSceneTransform = SceneComponentForCameraManager->GetComponentTransform();
	FTransform PlayerCameraTransform = PlayerCharacter->GetFirstPersonCameraComponent()->GetComponentTransform();
	
	FTransform RelativeBackFacingSceneTransform = UKismetMathLibrary::MakeRelativeTransform(PlayerCameraTransform, BackFacingSceneTransform);
	
	FTransform LinkedPortalTransform = LinkedPortal->GetActorTransform();

	FTransform ComposedTransform = RelativeBackFacingSceneTransform * LinkedPortalTransform;

	FVector LinkedPortalForwardVector = LinkedPortal->GetActorForwardVector() * 10.0f;

	FVector CameraRelativeLocation = ComposedTransform.GetLocation() - PlayerCharacter->GetFirstPersonCameraComponent()->GetRelativeLocation();
	
	FVector NewPlayerLocation = LinkedPortalForwardVector + CameraRelativeLocation;
	
	FQuat NewPlayerRotation = ComposedTransform.GetRotation();

	FRotator PlayerRotator = UKismetMathLibrary::MakeRotator(0.0f, NewPlayerRotation.Y, NewPlayerRotation.Z);

	FTransform NewPlayerTransform = UKismetMathLibrary::MakeTransform(PlayerCharacter->GetActorLocation(), PlayerController->GetControlRotation(), FVector::OneVector);

	FVector NewRelativePlayerVelocity = UKismetMathLibrary::TransformDirection(NewPlayerTransform, RelativePlayerVelocity);
	
	PlayerCharacter->SetActorLocation(NewPlayerLocation, false, nullptr, ETeleportType::ResetPhysics);
	
	PlayerCharacter->GetMovementComponent()->Velocity = NewRelativePlayerVelocity;
	
	PlayerController->SetControlRotation(PlayerRotator);
}
