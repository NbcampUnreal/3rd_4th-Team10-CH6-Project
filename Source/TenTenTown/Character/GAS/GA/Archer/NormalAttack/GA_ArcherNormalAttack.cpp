#include "GA_ArcherNormalAttack.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Animation/AnimMontage.h"
#include "Character/Characters/Archer/ArcherCharacter/ArcherCharacter.h"
#include "Character/Characters/Archer/Arrow/Archer_Arrow.h"
#include "Character/Characters/Archer/Bow/ArcherBow.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PlayerController.h" 

void UGA_ArcherNormalAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                             const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    StartTime = GetWorld()->GetTimeSeconds();
    
    ASC = GetAbilitySystemComponentFromActorInfo();
    ArcherCharacter = Cast<AArcherCharacter>(GetAvatarActorFromActorInfo());
    if (ArcherCharacter)
    {
        Bow = ArcherCharacter->GetEquippedBow();
        if (ArcherCharacter->GetMesh())
        {
            AnimInstance = ArcherCharacter->GetMesh()->GetAnimInstance();
        }
    }
    
    if (!ASC || !ArcherCharacter || !Bow)
    {
       UE_LOG(LogTemp, Warning, TEXT("Validation Failed in ActivateAbility"));
       EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
       return;
    }
    
    UAbilityTask_PlayMontageAndWait* AMTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
       this, FName("None"), AttackMontage, 0.5f, NAME_None, true, 1.f, 0.f);

    AMTask->OnInterrupted.AddUniqueDynamic(this, &ThisClass::OnMontageEnd);
    AMTask->OnCancelled.AddUniqueDynamic(this, &ThisClass::OnMontageEnd);
    AMTask->OnCompleted.AddUniqueDynamic(this, &ThisClass::OnMontageEnd);
    
    AMTask->ReadyForActivation();
    
    ASC->ExecuteGameplayCue(GASTAG::GameplayCue_Archer_NormalAttackStart);
    
    if (HasAuthority(&ActivationInfo)) 
    {
        if (ArrowClass)
        {
           FVector ArrowSpawnPosition = Bow->GetMesh()->GetSocketLocation(FName("ArrowSocket"));
           FRotator ArrowSpawnRotation = Bow->GetMesh()->GetSocketRotation(FName("ArrowSocket"));
        
           FTransform SpawnTransform(ArrowSpawnRotation, ArrowSpawnPosition);
        
           Arrow = GetWorld()->SpawnActorDeferred<AArcher_Arrow>(
              ArrowClass,
              SpawnTransform,
              ArcherCharacter,
              ArcherCharacter,
              ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
           );

           if (Arrow)
           {
               Arrow->SetIgnoreActor(ArcherCharacter);
               Arrow->SetIgnoreActor(Bow);
               
               Arrow->SetSetByCallerClass(SetByCallerClass);
               Arrow->SetDamage(Damage);
               
               Arrow->FinishSpawning(SpawnTransform);
               
               Arrow->AttachToComponent(Bow->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("ArrowSocket"));
           }
        }
    }
    
    ASC->ForceReplication(); 
}

void UGA_ArcherNormalAttack::InputReleased(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    Super::InputReleased(Handle, ActorInfo, ActivationInfo);
    
    EndTime = GetWorld()->GetTimeSeconds();
    HoldTime = EndTime - StartTime;

    float HoldTimeRatio = 0.f;
    
    if (AnimInstance && CurrentMontage)
    {
        float PlayRate = AnimInstance->Montage_GetPlayRate(CurrentMontage);
    
       
        int32 SectionIndex = CurrentMontage->GetSectionIndex(FName("Release"));
    
     
        float TimeUntilRelease = CurrentMontage->CompositeSections[SectionIndex].GetTime();

        if (TimeUntilRelease > 0.f && PlayRate > 0.f)
        {
          
            float RealDuration = TimeUntilRelease / PlayRate;
            
            HoldTimeRatio = HoldTime / RealDuration;
            
            HoldTimeRatio = FMath::Clamp(HoldTimeRatio, 0.f, 1.f);
        }
    }
    
    if (ASC)
    {
       ASC->CurrentMontageJumpToSection("Release");
        ASC->CurrentMontageSetPlayRate(1.f);
       ASC->ExecuteGameplayCue(GASTAG::GameplayCue_Archer_NormalAttackRelease);
    }
    
    if (HasAuthority(&ActivationInfo))
    {
        if (Arrow && ArcherCharacter)
        {
            Arrow->SetLifeSpan(LifeSpan);
           APlayerController* PC = Cast<APlayerController>(ArcherCharacter->GetController());
           
           if (PC)
           {
               FVector Start;
               FRotator CameraRotation;
               PC->GetPlayerViewPoint(Start, CameraRotation); // 서버에서도 PC의 ViewPoint는 동기화됨
        
               FVector End = CameraRotation.Vector() * 20000.f + Start;
               FVector BowStart = Arrow->GetActorLocation(); 
               
               LaunchDirection = (End - BowStart).GetSafeNormal();
           }
           else
           {
               LaunchDirection = ArcherCharacter->GetActorForwardVector();
           }

           Arrow->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
            Arrow->SetDamage(Damage);
           Arrow->FireArrow(LaunchDirection,HoldTimeRatio);
           
           Arrow = nullptr;
        }
    }
}

void UGA_ArcherNormalAttack::OnMontageEnd()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_ArcherNormalAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility, bool bWasCancelled)
{
    if (HasAuthority(&ActivationInfo))
    {
        if (Arrow)
        {
            Arrow->Destroy();
            Arrow = nullptr;
        }
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}