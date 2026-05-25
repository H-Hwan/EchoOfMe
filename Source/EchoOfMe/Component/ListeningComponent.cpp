// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/ListeningComponent.h"

// Sets default values for this component's properties
UListeningComponent::UListeningComponent() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UListeningComponent::BeginPlay() {
	Super::BeginPlay();

	bIsListening = false;
}


// 듣기 시작
void UListeningComponent::StartListening() {
	if (bIsListening) return;
	bIsListening = true;

	UE_LOG(LogTemp, Log, TEXT("[Listening] 듣기 시작"));

	/*	[ToDo] 사운드 큐 재생
		- 환경음 필터링 시작
		- 공명 레이어 페이드인
		- MetaSound 연동 시 여기서 트리거	*/

	OnListeningChanged.Broadcast(true);
}


// 듣기 종료
void UListeningComponent::StopListening() {
	if (!bIsListening) return;
	bIsListening = false;

	UE_LOG(LogTemp, Log, TEXT("[Listening] 듣기 종료"));

	/*	[ToDo] 듣기 종료 처리
		- 환경음 필터링 복원
		- 공명 레이어 페이드아웃	*/

	OnListeningChanged.Broadcast(false);
}
