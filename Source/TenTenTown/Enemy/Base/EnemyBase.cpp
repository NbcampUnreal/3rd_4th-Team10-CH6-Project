// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyBase.h"
#include "Enemy/Base/EnemyBase.h"
#include "GameFramework/Character.h"
#include "StateTreeModule.h"
#include "GameplayStateTreeModule/Public/Components/StateTreeComponent.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "TTTGamePlayTags.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Animation/AnimInstance.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "Components/SplineComponent.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "Components/SplineComponent.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "Enemy/Route/SplineActor.h"
#include "Enemy/TestEnemy/TestGold.h"
#include "Net/UnrealNetwork.h"
#include "Structure/Crossbow/CrossbowStructure.h"
#include "UI/Enemy/EnemyHealthBarWidget.h"


AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetNetUpdateFrequency(30.f);
	
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	if (ASC)
	{
		ASC->SetIsReplicated(true);
		ASC->SetReplicationMode(EGameplayEffectReplicationMode::Full);
	}

	StateTree = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTree"));
	StateTree->SetAutoActivate(false);

	DetectComponent = CreateDefaultSubobject<USphereComponent>(TEXT("DetectComponent"));
	if (RootComponent && DetectComponent)
	{
		DetectComponent->SetupAttachment(RootComponent);
	}

	GetMesh()->SetIsReplicated(true);
	AutoPossessAI = EAutoPossessAI::Disabled;
	AIControllerClass = AAIController::StaticClass();



	// UWidgetComponent 생성 및 부착
	HealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidgetComponent"));
	HealthWidgetComponent->SetupAttachment(RootComponent);

	// 월드 공간 설정
	HealthWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	// HealthWidgetComponent->SetDrawSize(FVector2D(200.0f, 30.0f)); 
	// HealthWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f)); // 머리 위로 위치 조정

}

void AEnemyBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemyBase, MovedDistance);
	DOREPLIFETIME(AEnemyBase, DistanceOffset);
	DOREPLIFETIME(AEnemyBase, SplineActor);
}

void AEnemyBase::InitializeEnemy()
{
	if (ASC)
	{
		ASC->InitAbilityActorInfo(this, this);

		DetectComponent->SetSphereRadius(
			ASC->GetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetAttackRangeAttribute())
		);
		DetectComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		AddDefaultAbility();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ASC is null"));
	}
}

void AEnemyBase::ResetEnemy()
{
    if (!ASC) return;

    TArray<FActiveGameplayEffectHandle> AllEffects = ASC->GetActiveEffects(FGameplayEffectQuery());
    for (const FActiveGameplayEffectHandle& Handle : AllEffects)
    {
        ASC->RemoveActiveGameplayEffect(Handle);
    }

    ASC->RemoveLooseGameplayTags(ASC->GetOwnedGameplayTags());
	ASC->ClearAllAbilities();


    if (StateTree)
    {
        StateTree->StopLogic("Reset");
        StateTree->Cleanup();
    }

    MovedDistance = 0.f;
    DistanceOffset = 0.f;

    OverlappedPawns.Empty();
    if (DetectComponent)
    {
        DetectComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        DetectComponent->UpdateOverlaps();
    }

    SetActorLocation(FVector(0.f, 0.f, -10000.f));
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
    SetActorTickEnabled(false);
}


void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	if (ASC) // 가정: 몬스터도 ASC를 가짐
	{
		// 체력 (Health) Attribute 변경 시 HealthChanged 함수 호출하도록 바인딩
		HealthChangeDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(
			UAS_EnemyAttributeSetBase::GetHealthAttribute() // 예시: UAS_CharacterBase::Health() 대신 실제 Getter 사용
		).AddUObject(this, &AEnemyBase::HealthChanged);
		UpdateHealthBar_Initial();
	}
}

/*
void AEnemyBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (ASC)
	{
		ASC->InitAbilityActorInfo(this, this);

		DetectComponent->SetSphereRadius(ASC->GetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetAttackRangeAttribute()));
		DetectComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		AddDefaultAbility();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ASC is null"));
	}
}*/

void AEnemyBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	LogTimer += DeltaSeconds;

	if (LogTimer >= 3.0f)
	{
		LogAttributeAndTags();
        
		LogTimer = 0.0f;
	}
}

void AEnemyBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	DetectComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::OnDetection);
	DetectComponent->OnComponentEndOverlap.AddDynamic(this, &AEnemyBase::EndDetection);
}


void AEnemyBase::OnDetection(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                             int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!ASC || ASC->HasMatchingGameplayTag(GASTAG::Enemy_State_Dead))
	{
		return; 
	}

	bool bIsTargetType = false;
	
	if (OtherActor && OtherActor != this)
	{
		if(OtherActor->IsA<ABaseCharacter>())
		{
			bIsTargetType = true;
		}
		else if (OtherActor->IsA<ACrossbowStructure>())
		{
			if (ACrossbowStructure* Tower = Cast<ACrossbowStructure>(OtherActor))
			{
				if (Tower && (OtherComp == (UPrimitiveComponent*)(Tower->BaseMesh)) || OtherComp == (UPrimitiveComponent*)(Tower->TurretMesh))
				{
					bIsTargetType = true;
				}
			}
		}
	}

	if (bIsTargetType)
	{
		if (!OverlappedPawns.Contains(OtherActor))
		{
			OverlappedPawns.Add(OtherActor);
			
			SetCombatTagStatus(true);
		}
	}
	
}

void AEnemyBase::EndDetection(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                              int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor != this && OtherActor->IsA<APawn>())
	{
		OverlappedPawns.Remove(OtherActor);

		if (OverlappedPawns.Num() == 0)
		{
			SetCombatTagStatus(false);
		}
	}
}

void AEnemyBase::SetCombatTagStatus(bool IsCombat)
{
	if (ASC)
	{
		FGameplayTag CombatTag = GASTAG::Enemy_State_Combat; 

		if (IsCombat)
		{
			if (!ASC->HasMatchingGameplayTag(CombatTag))
			{
				ASC->AddLooseGameplayTag(CombatTag);
			}
		}
		else
		{
			if (ASC->HasMatchingGameplayTag(CombatTag))
			{
				ASC->RemoveLooseGameplayTag(CombatTag);
			}
		}
	}
}




UAbilitySystemComponent* AEnemyBase::GetAbilitySystemComponent() const
{
	return ASC;
}


void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyBase::StartTree()
{
	if (StateTree)
	{
		StateTree->StartLogic();
	}
}

void AEnemyBase::LogAttributeAndTags()
{

	float CurrentHealth = ASC->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetHealthAttribute());
	float MaxHealth = ASC->GetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetMaxHealthAttribute());
	float Attack = ASC->GetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetAttackAttribute());

	UE_LOG(LogTemp, Display, TEXT("===== Enemy Log: %s ====="), *GetName());
	UE_LOG(LogTemp, Display, TEXT("Health: %.1f / %.1f"), CurrentHealth, MaxHealth);
	UE_LOG(LogTemp, Display, TEXT("Attack: %.1f"), Attack);
	
	FGameplayTagContainer OwnedTags;
	ASC->GetOwnedGameplayTags(OwnedTags);

	FString TagsString;
	for (const FGameplayTag& Tag : OwnedTags)
	{
		TagsString.Appendf(TEXT("%s, "), *Tag.GetTagName().ToString());
	}

	if (TagsString.IsEmpty())
	{
		TagsString = TEXT("NONE");
	}
	else
	{
		TagsString.RemoveAt(TagsString.Len() - 2); 
	}

	UE_LOG(LogTemp, Display, TEXT("Current Tags: %s"), *TagsString);
	UE_LOG(LogTemp, Display, TEXT("=========================="));
}


void AEnemyBase::AddDefaultAbility()
{
	for (TSubclassOf<UGameplayAbility> Ability : DefaultAbilities)
	{
		FGameplayAbilitySpec AbilitySpec(Ability, 1, INDEX_NONE, this);
		ASC->GiveAbility(AbilitySpec);
	}
}


float AEnemyBase::PlayMontage(UAnimMontage* MontageToPlay, FMontageEnded Delegate, float InPlayRate)
{

	if (!MontageToPlay || !GetMesh())
	{
		return 0.0f;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return 0.0f;
	}

	const float Duration = AnimInstance->Montage_Play(MontageToPlay, InPlayRate);
	if (Duration <= 0.f)
	{
		return 0.f;
	}

	FOnMontageEnded MontageEndedDelegate;
	if (Delegate.IsBound())
	{
		MontageEndedDelegate.BindUFunction(Delegate.GetUObject(), Delegate.GetFunctionName());
	}
	
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);
	
	return Duration;
}

void AEnemyBase::DropGoldItem()
{
	if (this->HasAuthority())
	{
		const float GoldAmount = ASC->GetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetGoldAttribute());
		const FVector GoldLocation = GetActorLocation();

		const float MinImpulse = 100.0f;              
		const float MaxImpulse = 200.0f;

		if (!GoldItem) return;
		
		ATestGold* GoldItemCDO = Cast<ATestGold>(GoldItem->GetDefaultObject());

		if (!GoldItemCDO) return;

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnParams.Template = GoldItemCDO;
		
		// if (GoldAmount <= 0.0f || !GoldItem)
		// {
		// 	return;	
		// }

		for (int32 i = 0; i < GoldAmount; ++i)
		{
			FVector SpawnLocation = GoldLocation + FVector(0, 0, 20.f);
			
			ATestGold* SpawnedGold = GetWorld()->SpawnActor<ATestGold>(GoldItem, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
			if (SpawnedGold)
			{
				if (UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(SpawnedGold->GetRootComponent()))
				{
					PrimitiveComp->SetSimulatePhysics(true);
					
					FVector RandomDirection = FMath::VRand(); 
					RandomDirection.Z = FMath::Max(0.2f, RandomDirection.Z);
					RandomDirection.Normalize();

					float RandomImpulse = FMath::RandRange(MinImpulse, MaxImpulse);
                
					PrimitiveComp->AddImpulse(RandomDirection * RandomImpulse * PrimitiveComp->GetMass(), NAME_None, true);
				}
			}
		}
	}
}

void AEnemyBase::ApplySplineMovementCorrection()
{
	if (!SplineActor || !SplineActor->SplineActor) return;

	USplineComponent* SplineComp = SplineActor->SplineActor;

	float ClampedDistance = FMath::Clamp(MovedDistance, 0.f, SplineComp->GetSplineLength());

	FVector SplineLocation = SplineComp->GetLocationAtDistanceAlongSpline(
		ClampedDistance, ESplineCoordinateSpace::World);

	FVector Direction = SplineComp->GetDirectionAtDistanceAlongSpline(
		ClampedDistance, ESplineCoordinateSpace::World);

	Direction.Normalize();
	const FVector RightVector = FVector::CrossProduct(Direction, FVector::UpVector);

	FVector OffsetVector = RightVector * DistanceOffset;

	FVector NewLocation = SplineLocation + OffsetVector;
	FRotator NewRotation = FRotationMatrix::MakeFromX(Direction).Rotator();

	SetActorLocationAndRotation(NewLocation, NewRotation, false, nullptr, ETeleportType::TeleportPhysics);
}

void AEnemyBase::OnRep_MovedDistance()
{
	ApplySplineMovementCorrection();
}

void AEnemyBase::OnRep_DistanceOffset()
{
	ApplySplineMovementCorrection();
}


void AEnemyBase::Multicast_PlayMontage_Implementation(UAnimMontage* MontageToPlay, float InPlayRate)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		PlayMontage(MontageToPlay, FMontageEnded(), InPlayRate);
	}
}

#pragma region UI_Region
void AEnemyBase::HealthChanged(const FOnAttributeChangeData& Data)
{
	// 1. 새로운 체력 값 가져오기
	float NewHealth = Data.NewValue;
	float MaxHealth = GetAbilitySystemComponent()->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetMaxHealthAttribute());

	// 2. 위젯 인스턴스 가져오기 (WBP_EnemyHealthBar의 C++ 부모 클래스 필요)
	if (HealthWidgetComponent->GetUserWidgetObject())
	{
		// WBP_EnemyHealthBar의 C++ 부모 클래스 (예: UEnemyHealthBarWidget)로 캐스팅
		if (UEnemyHealthBarWidget* HealthBar = Cast<UEnemyHealthBarWidget>(HealthWidgetComponent->GetUserWidgetObject()))
		{
			// 3. 위젯의 C++ 함수를 호출하여 값 전달
			HealthBar->UpdateHealth(NewHealth, MaxHealth);
		}
	}

	// 4. (선택적) 체력이 0 이하면 위젯 숨김
	if (NewHealth <= 0.0f)
	{
		HealthWidgetComponent->SetVisibility(false);
	}
}

void AEnemyBase::UpdateHealthBar_Initial()
{
	if (!ASC) return;

	// 현재 Health와 Max Health 값을 ASC에서 직접 가져옵니다.
	float CurrentHealth = ASC->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetHealthAttribute());
	float MaxHealth = ASC->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetMaxHealthAttribute());

	// UWidgetComponent를 통해 위젯 인스턴스를 가져와 C++ 함수를 호출합니다.
	if (HealthWidgetComponent->GetUserWidgetObject())
	{
		if (UEnemyHealthBarWidget* HealthBar = Cast<UEnemyHealthBarWidget>(HealthWidgetComponent->GetUserWidgetObject()))
		{
			// 이 시점에 위젯에 초기 값을 전달합니다. (예: 100/100)
			HealthBar->UpdateHealth(CurrentHealth, MaxHealth);
		}
	}

	// (선택 사항: 만약 Health가 0인 상태로 시작한다면)
	if (CurrentHealth <= 0.0f)
	{
		HealthWidgetComponent->SetVisibility(false);
	}
}
#pragma endregion
