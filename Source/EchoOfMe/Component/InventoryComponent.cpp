#include "Component/InventoryComponent.h"

#include "Data/InventoryItemDefinition.h"


UInventoryComponent::UInventoryComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}


bool UInventoryComponent::AddItem(UInventoryItemDefinition* Item) {
	// 중복 방지
	if (!Item || HasItem(Item->ItemID)) return false;

	Items.Add(Item);
	UE_LOG(LogTemp, Warning, TEXT("[Inventory] AddItem 성공, 총 %d"), Items.Num());
	OnInventoryChanged.Broadcast();
	UE_LOG(LogTemp, Warning, TEXT("[Inventory] AddItem 방송, this=%p"), this);

	return true;
}


bool UInventoryComponent::HasItem(FName ItemID) const {
	return Items.ContainsByPredicate(
		[ItemID](const UInventoryItemDefinition* It) {return It && It->ItemID == ItemID; }
	);
}


void UInventoryComponent::UseItem(UInventoryItemDefinition* Item) {
	if (Item && Item->bUsable && HasItem(Item->ItemID)) OnItemUsed.Broadcast(Item);
}
