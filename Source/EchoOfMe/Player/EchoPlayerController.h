#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EchoPlayerController.generated.h"


class UInputMappingContext;
class AEchoPlayerCharacter;

class UInputAction;
class UInventoryWidget;

class UInventoryComponent;
class URecorderComponent;
class UMemoryComponent;
class UStoryPlayerComponent;


UCLASS(Abstract)
class ECHOOFME_API AEchoPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AEchoPlayerController();

public:
	// 게임 시작 시 호출
	virtual void BeginPlay() override;

	// 입력 매핑 컨텍스트 등록
	virtual void SetupInputComponent() override;

	// 컨트롤러 빙의 이벤트
	virtual void OnPossess(APawn* InPawn) override;


	//---
	// 입력
protected:
	// 기본 입력 매핑 컨텍스트 배열
	UPROPERTY(EditAnywhere, Category = "Input|Input Mapping")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Inventory")
	TObjectPtr<UInputAction> ToggleInventoryAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Interaction")
	TObjectPtr<UInputAction> InteractAction;


	//---
	// 리스폰
protected:
	/*	리스폰할 플레이어 캐릭터
		비워두면 OnPossess에서 현재 빙의한 캐릭터 클래스를 자동으로 캐싱 */
	UPROPERTY(EditAnywhere, Category = "Respawn")
	TSubclassOf<AEchoPlayerCharacter> CharacterClass;

	// 체크포인트로 돌아가기 전 대기 시간
	UPROPERTY(EditAnywhere, Category = "Respawn")
	float RespawnDelay = 1.0f;

	// 다시 스폰할 위치와 방향
	FTransform RespawnTransform;

	// 체크포인트가 지정되었는지 확인
	bool bHasRespawnTransform = false;

	// 중복 리스폰 방지용 플래그
	bool bIsRespawning = false;

	FTimerHandle RespawnTimerHandle;

public:
	// 체크포인트에서 호출 >> 리스폰 위치 업데이트
	UFUNCTION(BlueprintCallable, Category = "Respawn")
	void SetRespawnTransform(const FTransform& NewRespawn);

	// 외부에서 플레이어 사망 처리를 요청할 때 호출
	UFUNCTION(BlueprintCallable, Category = "Respawn")
	void KillPlayer();

	// 실제 캐릭터를 다시 생성하고 빙의
	UFUNCTION(BlueprintCallable, Category = "Respawn")
	void RespawnPlayer();

	UFUNCTION()
	void OnPawnDestroyed(AActor* DestroyActor);

private:
	void CachePawnClassIfNeeded(APawn* InPawn);
	void CacheInitialRespawnTransformIfNeeded(APawn* InPawn);
	void ScheduleRespawn();
	FTransform GetFallbackRespawnTransform() const;


	//---
	// 인벤토리
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UInventoryComponent> Inventory;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TObjectPtr<URecorderComponent> Recorder;

	UPROPERTY()
	TObjectPtr<UInventoryWidget> InventoryWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ToggleInventory();

public:
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractDistance = 250.f;

	// 확인용 임시 함수
	void HandleInteract();

private:
	bool bInventoryOpen = false;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UMemoryComponent> Memory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStoryPlayerComponent> StoryPlayer;
};
