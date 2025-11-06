#include "GA_Mage_Fireball.h"

#include "GameFramework/Character.h"
#include "FireballProjectile.h"
#include "Engine/World.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/Characters/Mage/MageCharacter.h"
#include "Components/StaticMeshComponent.h"

UGA_Mage_Fireball::UGA_Mage_Fireball()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Mage.Fireball")));
}

void UGA_Mage_Fireball::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}
	
	if (FireballMontage)
	{
		PlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			FireballMontage,
			1.f,
			NAME_None,
			false
		);
		
		PlayTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayTask->ReadyForActivation();

		WaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ShootTag, nullptr, true, true);
		WaitTask->EventReceived.AddDynamic(this, &UGA_Mage_Fireball::OnShootEvent);
		WaitTask->ReadyForActivation();
		
	}
}

void UGA_Mage_Fireball::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_Mage_Fireball::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_Mage_Fireball::OnShootEvent(const FGameplayEventData Payload)
{
	AMageCharacter* Mage = Cast<AMageCharacter>(GetAvatarActorFromActorInfo());

	if (!Mage || !ProjectileClass) return;

	//파이어볼 스폰 위치: 소켓 (없으면 캐릭터)
	FTransform MuzzleXf = Mage->GetActorTransform();
	if (Mage->WandMesh && Mage->WandMesh->DoesSocketExist(MuzzleSocketName))
	{
		MuzzleXf = Mage->WandMesh->GetSocketTransform(MuzzleSocketName, RTS_World);
	}
	const FVector SocketLoc = MuzzleXf.GetLocation();

	UE_LOG(LogTemp, Log, TEXT("[FB] SocketLoc=%s  SocketRot=%s"),
	*MuzzleXf.GetLocation().ToString(),
	*MuzzleXf.Rotator().ToString());

	FVector EyeLoc;
	FRotator EyeRot;
	if (APlayerController* PC = Cast<APlayerController>(Mage->GetController()))
	{
		PC->GetPlayerViewPoint(EyeLoc, EyeRot);
	}
	else
	{
		Mage->GetActorEyesViewPoint(EyeLoc, EyeRot);
	}
	
	const FVector AimDir = EyeRot.Vector();
	const FVector TraceEnd = EyeLoc + AimDir * 10000.f;
	FHitResult Hit;
	FCollisionQueryParams Q(SCENE_QUERY_STAT(FB_Aim), true, Mage);
	Q.AddIgnoredActor(Mage);

	if (Mage->WandMesh) Q.AddIgnoredComponent(Mage->WandMesh);

	Mage->GetWorld()->LineTraceSingleByChannel(Hit, EyeLoc, TraceEnd, ECC_Visibility, Q);

	const FVector AimPoint = Hit.bBlockingHit ? Hit.ImpactPoint : TraceEnd;

	FVector Dir = (AimPoint - SocketLoc);
	if (!Dir.Normalize()) Dir = AimDir;
		
	const FVector Loc = SocketLoc + Dir * 12.f;
	const FRotator Rot = (AimPoint - Loc).Rotation();

	const FVector ActorLoc  = Mage->GetActorLocation();
	const FVector WandLoc   = Mage->WandMesh ? Mage->WandMesh->GetComponentLocation() : FVector::ZeroVector;
	
	if (CurrentActorInfo->IsNetAuthority())
	{
		SpawnFireball(Loc, Rot);
	}
	else
	{
		ServerSpawnProjectile(Loc, Rot);
	}
}

void UGA_Mage_Fireball::ServerSpawnProjectile_Implementation(const FVector& SpawnLoc, const FRotator& SpawnRot)
{
	UE_LOG(LogTemp, Warning, TEXT("[FB] Spawn try (server only)"));
	SpawnFireball(SpawnLoc, SpawnRot);
}

void UGA_Mage_Fireball::SpawnFireball(const FVector& SpawnLoc, const FRotator& ViewRot)
{
	UE_LOG(LogTemp, Warning, TEXT("[FB] Spawn try"));
	ACharacter* Char = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	AActor* OwnerActor = GetOwningActorFromActorInfo();
	if (!Char || !ProjectileClass || !Char->HasAuthority()) return;
	
	FActorSpawnParameters P;
	P.Owner = OwnerActor;
	P.Instigator = Char;
	P.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (AFireballProjectile* Proj = Char->GetWorld()->SpawnActor<AFireballProjectile>(ProjectileClass, SpawnLoc, ViewRot, P))
	{
		Proj->InitVelocity(ViewRot.Vector(), MuzzleSpeed);

		Char->MoveIgnoreActorAdd(Proj);
	}
}
