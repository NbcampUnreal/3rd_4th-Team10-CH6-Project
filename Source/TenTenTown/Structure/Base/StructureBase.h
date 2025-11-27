#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structure/Interaction/InteractionInterface.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "Character/PS/TTTPlayerState.h"
#include "Structure/Data/StructureData.h"
#include "StructureBase.generated.h"

class ATTTPlayerState;

UCLASS()
class TENTENTOWN_API AStructureBase : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	AStructureBase();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// --- 인터페이스 구현 ---
	virtual void OnInteract_Implementation(ACharacter* InstigatorCharacter) override;
	// -----------------------

	// 데이터 테이블 로우 핸들 (에디터에서 지정)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Structure Settings")
	FDataTableRowHandle StructureDataHandle;

	// 현재 레벨 (Replicated)
	UPROPERTY(ReplicatedUsing=OnRep_CurrentLevel, VisibleAnywhere, BlueprintReadOnly, Category = "Structure Status")
	int32 CurrentLevel = 0;

	UFUNCTION()
	void OnRep_CurrentLevel();

	// 업그레이드 요청 (Blueprint에서 위젯 버튼 클릭 시 호출)
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_TryUpgrade(ATTTPlayerState* RequestorPS);

	// 파괴(판매) 요청
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_TrySell(ATTTPlayerState* RequestorPS);

	UFUNCTION(BlueprintPure, Category = "Structure Status")
	int32 GetUpgradeCost() const;

	UFUNCTION(BlueprintPure, Category = "Structure Status")
	bool CanUpgrade() const;

	UFUNCTION(BlueprintPure, Category = "Structure Status")
	int32 GetSellRefund() const;

protected:
	virtual void BeginPlay() override;

	// 실제 데이터 캐싱
	FStructureData CachedStructureData;

	// 메시 등 시각적 요소
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComp;
};
