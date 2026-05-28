// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/NoiseMakerComponent.h"

#include "GameFramework/Pawn.h"


// Sets default values for this component's properties
UNoiseMakerComponent::UNoiseMakerComponent() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}


// 1회성 소리 ex) 문 열기, 충돌 등
void UNoiseMakerComponent::ReportNoise(float Loudness) {
	AActor* OwnerActor = Cast<AActor>(GetOwner());
	if (!OwnerActor) {
		UE_LOG(LogTemp, Warning, TEXT("[Noise] 참조 실패!!"));
		return;
	}

	OwnerActor->MakeNoise(Loudness, nullptr, OwnerActor->GetActorLocation());

	UE_LOG(LogTemp, Log, TEXT("[Noise] %s 소리 발생 (Loudness=%.2f)"), *OwnerActor->GetName(), Loudness);
}


// 이동 상태에 따른 발소리 >> 캐릭터에서 호출
void UNoiseMakerComponent::ReportFootstep(bool bSprinting) {
	ReportNoise(bSprinting ? SprintLoudness : WalkLoudness);
}
