#include "Item/CheckpointActor.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"

#include "Player/EchoPlayerController.h"


ACheckpointActor::ACheckpointActor() {
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


// 게임 시작 또는 스폰 시 호출
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
