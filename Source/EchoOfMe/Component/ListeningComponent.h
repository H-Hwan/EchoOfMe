// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ListeningComponent.generated.h"


UENUM(BlueprintType)
enum class EListeningCue : uint8 {
	None		UMETA(DisplayName = "없음"),
	Memory		UMETA(DisplayName = "기억"),
	Echo		UMETA(DisplayName = "잔향"),
	Resonance  	UMETA(DisplayName = "공명")
};


// 듣기 상태 변화 방송
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnListeningChanged, bool, bIsListening);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCueChanged, EListeningCue, NewCue);


UCLASS(ClassGroup=(EchoOfMe), meta=(BlueprintSpawnableComponent))
class ECHOOFME_API UListeningComponent : public UActorComponent {
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UListeningComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	//---
	// 듣기
public:
	// 듣기 시작 >> 입력 Started에서 호출
	UFUNCTION(BlueprintCallable, Category = "Listening")
	void StartListening();

	// 듣기 종료 >> 입력 Completed에서 호출
	UFUNCTION(BlueprintCallable, Category = "Listening")
	void StopListening();

	// 듣기 상태 반환
	UFUNCTION(BlueprintPure, Category = "Listening")
	bool IsListening() const { return bIsListening; }

	UFUNCTION(BlueprintCallable, Category = "Listening")
	void SetCurrentCue(EListeningCue NewCue);

	UFUNCTION(BlueprintPure, Category = "Listening")
	EListeningCue GetCurrentCue() const { return CurrentCue; }

	UFUNCTION(BlueprintPure, Category = "Listening")
	float GetResonance() const { return Resonance; }


	//---
	// 방송
public:
	// 듣기 시작/종료 시 방송
	UPROPERTY(BlueprintAssignable, Category = "Listening")
	FOnListeningChanged OnListeningChanged;

	UPROPERTY(BlueprintAssignable, Category = "Listening")
	FOnCueChanged OnCueChanged;


	//---
protected:
	UPROPERTY(EditAnywhere, Category = "Listening|Resonance")
	float ResonanceGainPerSec = 12.f;

	UPROPERTY(EditAnywhere, Category = "Listening|Resonance")
	float ResonanceDecayPerSec = 3.f;

	UPROPERTY(EditAnywhere, Category = "Listening|Resonance")
	float ResonanceMax = 100.f;

	UPROPERTY(EditAnywhere, Category = "Listening|Input")
	float MinHoldDuration = 0.3f;

	//---
private:
	bool bIsListening = false;

	float Resonance = 0.f;
	EListeningCue  CurrentCue = EListeningCue ::None;

	// 듣기 시작 시간
	float ListenStartedAt = 0.f;

	// 최소 시간 도달 시점에 자동 종료할 타이머
	FTimerHandle PendingStopHandle;

	void PerformStop();
};
