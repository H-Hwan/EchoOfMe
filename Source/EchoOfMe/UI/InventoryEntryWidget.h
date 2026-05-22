// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryEntryWidget.generated.h"


class UInventoryItemDefinition;
class UImage;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEntryClicked, UInventoryItemDefinition*, Item);


UCLASS()
class ECHOOFME_API UInventoryEntryWidget : public UUserWidget {
	GENERATED_BODY()

public:
	// 표시할 아이템 지정
	void SetItem(UInventoryItemDefinition* InItem);

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnEntryClicked OnEntryClicked;

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Name;

private:
	UPROPERTY()
	TObjectPtr<UInventoryItemDefinition> Item;
};
