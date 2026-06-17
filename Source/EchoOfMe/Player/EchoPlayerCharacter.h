#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EchoPlayerCharacter.generated.h"


class UInputAction;
class UAnimMontage;
class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;
class USceneComponent;

class UListeningComponent;
class UFlashlightComponent;
class UNoiseMakerComponent;
class UEquipmentComponent;

struct FInputActionValue;


UCLASS(Abstract)
class ECHOOFME_API AEchoPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDead);


public:
		AEchoPlayerCharacter();

protected:
	// 게임 시작 또는 스폰 시 호출
	virtual void BeginPlay() override;
	virtual void PreRegisterAllComponents() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	// 매 프레임 호출
	virtual void Tick(float DeltaTime) override;

	// 플레이어 컨트롤러 빙의 시 입력 바인딩
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	//---
	// 이동
public:

	// 이동 입력 바인딩
	void Move(const FInputActionValue& Value);
	// 이동 처리
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoMove(float Right, float Forward);

	UFUNCTION()
	void DoRunning();

	UFUNCTION()
	void StopRunning();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UNoiseMakerComponent> NoiseMaker;


	//---
	// 액션
public:
	// 이동 액션
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	// 시점 변경 액션
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	// 점프 액션
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	// 달리기 액션
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> RunningAction;

	// 손전등 On/Off 액션
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> FlashlightAction;

	// 듣기 액션
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ListenAction;

	// 손전등 장착 액션
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> EquipFlashlightAction;

	// 녹음기 장착 액션
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> EquipRecorderAction;


	//---
	// 시점
public:
	// 추적 카메라 컴포넌트 참조
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(EditAnywhere, Category="Components|Camera|Attachment", meta=(DisplayName="Camera Boom Parent Socket"))
	FName CameraBoomParentSocket = NAME_None;

	// 시선 변경 입력 바인딩
	void Look(const FInputActionValue& Value);

	// 시점 변경 처리
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoLook(float Yaw, float Pitch);

	// 아래 시선각
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (ClampMin = "-89.0", ClampMax = "0.0"))
	float ViewPitchMin = -55.f;

	// 위 시선각
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (ClampMin = "0.0", ClampMax = "89.0"))
	float ViewPitchMax = 50.f;


	//---
	// 점프
public:
	// 시작
	void DoJumpStart();

	// 종료
	void DoJumpEnd();

	// 착지 처리
	virtual void Landed(const FHitResult& Hit) override;


	//---
	// 듣기
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UListeningComponent> Listening;

	UFUNCTION()
	void HandleListeningChanged(bool bIsListening);

	void OnListenStarted();
	void OnListenCompleted();

	// 듣기 중 채도
	UPROPERTY(EditAnywhere, Category = "Listening|FX")
	float ListenSaturation = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Listening|FX")
	float SaturationFadeDuration = 0.4f;

	// 듣기 중 노출 보정 (음수 = 어둡게)
	UPROPERTY(EditAnywhere, Category = "Listening|FX")
	float ListenExposureBias = -1.5f;

private:
	float CurrentSturation = 1.f;
	float TargetSturation = 1.f;
	float CurrentExposureBias = 0.f;
	float TargetExposureBias = 0.f;
	FVector4 OriginalColorSaturation = FVector4(1.f, 1.f, 1.f, 1.f);
	float OriginalExposureBias = 0.f;
	bool bOriginalColorSaturationOverride = false;
	bool bOriginalExposureBiasOverride = false;
	bool bListeningVisualEffectActive = false;
	bool bListeningVisualEffectEnabled = false;



	//---
	// 손전등
public:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UFlashlightComponent> FlashLight;

	UPROPERTY(EditAnywhere, Category="Flashlight|Aim")
	bool bAimFlashlightAtCameraTrace = true;

	UPROPERTY(EditAnywhere, Category="Flashlight|Aim", meta=(ClampMin="100.0"))
	float FlashlightAimTraceDistance = 3000.f;

	UPROPERTY(EditAnywhere, Category="Flashlight|Aim")
	TEnumAsByte<ECollisionChannel> FlashlightAimTraceChannel = ECC_Visibility;

	UPROPERTY(EditAnywhere, Category="Flashlight|Aim", meta=(ClampMin="0.0"))
	float FlashlightAimInterpSpeed = 30.f;

	UPROPERTY(EditAnywhere, Category="Flashlight|Aim")
	FRotator FlashlightAimRotationOffset = FRotator::ZeroRotator;

	void OnFlashLightInput();


	//---
	// 장착
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEquipmentComponent> Equipment;

	UPROPERTY(VisibleAnywhere, Category="Components|Equipment")
	TObjectPtr<UStaticMeshComponent> FlashlightMesh;

	UPROPERTY(VisibleAnywhere, Category="Components|Equipment")
	TObjectPtr<UStaticMeshComponent> RecorderMesh;

	UPROPERTY(EditAnywhere, Category="Components|Equipment|Attachment", meta=(DisplayName="Flashlight Parent Socket"))
	FName FlashlightParentSocket = TEXT("hand_r_Flashlight");

	UPROPERTY(EditAnywhere, Category="Components|Equipment|Attachment", meta=(DisplayName="Recorder Parent Socket"))
	FName RecorderParentSocket = TEXT("hand_r_Flashlight");

	void OnEquipFlashlight();
	void OnEquipRecorder();

private:
	void ForceCharacterComponentMobility();
	void AttachComponentToCharacterMesh(USceneComponent* Component, FName ParentSocket);
	void UpdateFlashlightAim(float DeltaTime);

	//---
	// 캐릭터 상태 정보
public:
	UPROPERTY(EditAnywhere, Category = "status")
	float MaxSpeed = 500;

	UPROPERTY(EditAnywhere, Category = "status")
	float CurrentSpeed = 320;

	UPROPERTY(EditAnywhere, Category = "status")
	float Stamina = 20;

	bool bRunning = false;
};
