// Copyright Epic Games, Inc. All Rights Reserved.

#include "GEIIProjectCharacter.h"

#include "GEIIProjectProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AGEIIProjectCharacter

AGEIIProjectCharacter::AGEIIProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a mesh component that will be used when being viewed by everyone other than the owner
	 //= CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P"));
	CharacterMesh = GetMesh();
	CharacterMesh->SetOwnerNoSee(true);
	CharacterMesh->SetupAttachment(RootComponent);
	CharacterMesh->bCastDynamicShadow = true;
	CharacterMesh->CastShadow = true;
	CharacterMesh->SetRelativeRotation(FRotator(1.9f, -90.0f, 5.2f));
	CharacterMesh->SetRelativeLocation(FVector(-0.5f, -4.4f, -90.0f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	TP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TP_Gun"));
	TP_Gun->SetOwnerNoSee(true);
	TP_Gun->bCastDynamicShadow = true;
	TP_Gun->CastShadow = true;
	FName HandSocketName = TEXT("hand_r");
	TP_Gun->SetupAttachment(CharacterMesh, HandSocketName);
	//TP_Gun->AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "hand_r");

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	PortalComponent = CreateDefaultSubobject<UGEIIProjectPortalComponent>(TEXT("PortalComponent"));
}

void AGEIIProjectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	
	HUDReference = Cast<AGEIIProjectHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());

	// Helper to convert Enum as Byte so the LineTrace works properly with Object Types
	// Declaring a Object Type Query to a Enum as Byte and Assigning it to ObjectTypeQuery8 since it's the
	// Second Custom Object Channel in Project Settings>Collisions
	TraceObjectTypes = EObjectTypeQuery::ObjectTypeQuery8;
		
	// Add our ObjectTypeQuery8 into the array 
	ObjectsToTraceAsByte.Add(TraceObjectTypes);
}

void AGEIIProjectCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	FVector Location = FirstPersonCameraComponent->GetComponentLocation();
	FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
	EndLocation = Location + (ForwardVector * PortalComponent->GetMaxSpawnDistance());	

	// Returns if LineTrace has hit our ObjectTypeQuery8 or not
	bool bHit = GetWorld()->LineTraceSingleByObjectType(Hit, Location, EndLocation, ObjectsToTraceAsByte, TraceParams);
	if(bHit)
	{
		HUDReference->SetOpaqueCrosshair();
	}
	else if(!bHit)
	{
		HUDReference->SetTransparentCrosshair();
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGEIIProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("FireBluePortal", IE_Pressed, this, &AGEIIProjectCharacter::OnLeftClick);
	PlayerInputComponent->BindAction("FireRedPortal", IE_Pressed, this, &AGEIIProjectCharacter::OnRightClick);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AGEIIProjectCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGEIIProjectCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AGEIIProjectCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AGEIIProjectCharacter::LookUpAtRate);
}

void AGEIIProjectCharacter::OnFire(bool bIsBlueProjectile)
{
	//PortalComponent->TraceForward();

	if(bIsBlueProjectile)
	{
		ProjectileClass = BlueProjectileClass;
	}
	else if(!bIsBlueProjectile)
	{
		ProjectileClass = RedProjectileClass;
	}
	
	// try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			const FRotator SpawnRotation = GetControlRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			
			World->SpawnActor<AGEIIProjectProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}

	// try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void AGEIIProjectCharacter::OnLeftClick()
{
	OnFire(true);
	FVector Location = FirstPersonCameraComponent->GetComponentLocation();
	FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
	PortalComponent->SpawnPortalAlongVector(Location, ForwardVector, true);
}

void AGEIIProjectCharacter::OnRightClick()
{
	OnFire(false);
	FVector Location = FirstPersonCameraComponent->GetComponentLocation();
	FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
	PortalComponent->SpawnPortalAlongVector(Location, ForwardVector, false);
}

void AGEIIProjectCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AGEIIProjectCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AGEIIProjectCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AGEIIProjectCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}