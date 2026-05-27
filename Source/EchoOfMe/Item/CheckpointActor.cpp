// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/CheckpointActor.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"

#include "Player/EchoPlayerController.h"


// Sets default values
ACheckpointActor::ACheckpointActor() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	SetRootComponent(TriggerBox);
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->SetGenerateOverlapEvents(true);

	RespawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("RespawnPoint"));
	RespawnPoint->SetupAttachment(TriggerBox);
}


// Called when the game starts or when spawned
void ACheckpointActor::BeginPlay() {
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ACheckpointActor::OnTriggerBegin);
}


void ACheckpointActor::OnTriggerBegin(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp,
	int32 BodyIndex, bool bFromSweep, const FHitResult& Sweep) {
	// 이미 발동된 상태라면 무시
	if (bActivated) return;

	const APawn* Pawn = Cast<APawn>(Other);
	if (!Pawn) return;

	AEchoPlayerController* PC = Cast<AEchoPlayerController>(Pawn->GetController());
	if (!PC) return;

	bActivated = true;
	PC->SetRespawnTransform(RespawnPoint->GetComponentTransform());

	UE_LOG(LogTemp, Log, TEXT("[CheckPoint] 활성화: %s | 리스폰: %s"), *GetName(), *RespawnPoint->GetComponentTransform().GetLocation().ToString());

	/*	ToDo
		사운드/연출: 체크포인트 도달 피드백	*/
}
