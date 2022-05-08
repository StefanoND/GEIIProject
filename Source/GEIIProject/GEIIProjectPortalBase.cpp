// Fill out your copyright notice in the Description page of Project Settings.


#include "GEIIProjectPortalBase.h"

#include "EngineUtils.h"
#include "ToolBuilderUtil.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

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
	
	PortalRim = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalRim"));
	PortalRim->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PortalRim->SetupAttachment(PortalPlane);
	
	SceneCaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent2D"));
	SceneCaptureComponent2D->SetupAttachment(RootComponent);
	SceneCaptureComponent2D->FOVAngle = 90.0f;
	
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetCollisionProfileName("Trigger");
	
	// Assigning OnBegin and OnEnd overlap events to our methods
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AGEIIProjectPortalBase::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AGEIIProjectPortalBase::OnOverlapEnd);
}

// Called when the game starts or when spawned
void AGEIIProjectPortalBase::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AGEIIProjectPortalBase::StaticClass(), Actors);

	uint8 ArrayLength = Actors.Num();
	
	for (int i = 0; i < ArrayLength; i++)
	{
		AGEIIProjectPortalBase* PortalClass = Cast<AGEIIProjectPortalBase>(Actors[i]);
		if(PortalClass != nullptr)
		{
			if(this != PortalClass && this->PortalNumber == PortalClass->PortalNumber)
			{
				PortalClass->Destroy();
			}
		}
	}
}

// Called every frame
void AGEIIProjectPortalBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGEIIProjectPortalBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "Overlap being");
}

void AGEIIProjectPortalBase::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "Overlap end");
}