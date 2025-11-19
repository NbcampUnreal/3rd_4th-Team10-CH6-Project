// Fill out your copyright notice in the Description page of Project Settings.

#include "TTTGamePlayTags.h"

//어빌리티
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Ability_Movement,"Ability.Movement")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Ability_Movement_Jump,"Ability.Movement.Jump")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Ability_Movement_Dash,"Ability.Movement.Dash")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Ability_Movement_Sprint,"Ability.Movement.Sprint")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Ability_Fireball,"Ability.Fireball")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Ability_Fighter_Ultimate,"Ability.Fighter.Ultimate")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Ability_Fighter_UltimateAttack,"Ability.Fighter.UltimateAttack")
//이벤트
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Jump_Cost,"Event.Jump.Cost")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_LevelUP,"Event.LevelUP")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Fireball_Charging,"Event.Fireball.Charging")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Fireball_Release,"Event.Fireball.Release")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Fighter_UltimateRelease,"Event.Fighter.UltimateRelease")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Fighter_UltimateReleaseEnd,"Event.Fighter.UltimateReleaseEnd")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Fighter_ComboStart,"Event.Fighter.ComboStart")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Fighter_ComboEnd,"Event.Fighter.ComboEnd")
//스테이트
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Movement_IsJumping,"State.Movement.IsJumping")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Movement_IsDoubleJumping,"State.Movement.IsDoubleJumping")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Movement_IsDashing,"State.Movement.IsDashing")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Movement_IsSprinting,"State.Movement.IsSprinting")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_IsChanelling,"State.IsChannelling")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_UltimateOnGoing,"State.Fighter.UltimateOnGoing")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Block_Movement,"State.Block.Movement")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Block_Everything,"State.Block.Everything")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Invulnerable,"State.Invulnerable")
//데이터
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Data_Jump_Stamina,"Data.Jump.Stamina")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Data_Enemy_Damage,"Data.Enemy.Damage")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Data_Damage,"Data.Damage")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Data_Duration,"Data.Duration")
//게임 플레이 큐
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Jump,"GameplayCue.Jump")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Dash,"GameplayCue.Dash")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Fireball_Charging,"GameplayCue.Fireball.Charging")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Fireball_Cast,"GameplayCue.Fireball.Cast")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Fireball_Explode,"GameplayCue.Fireball.Explode")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Fighter_Ultimate,"GameplayCue.Fighter.Ultimate")

UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Enemy_Sound_Attack,"GameplayCue.Enemy.Sound.Attack")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Enemy_Sound_Projectile,"GameplayCue.Enemy.Sound.Burrow")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Enemy_Sound_Burrow,"GameplayCue.Enemy.Sound.Projectile")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Enemy_Effect_Attack,"GameplayCue.Enemy.Effect.Attack")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Enemy_Effect_Projectile,"GameplayCue.Enemy.Effect.Projectile")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Enemy_Effect_Burrow,"GameplayCue.Enemy.Effect.Burrow")

//쿨다운
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Cooldown_Dash,"Cooldown.Dash")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Cooldown_Fireball,"Cooldown.Fireball")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Cooldown_Fighter_Ultimate,"Cooldown.Fighter.Ultimate")

//구조물
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_IsInstall, "State.IsInstall")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Structure_Confirm, "State.Structure.Confirm")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Structure_Cancel,  "State.Structure.Cancel")


// --Enemy
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Type,"Enemy.Type")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Type_Orc,"Enemy.Type.Orc")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Type_Dragon,"Enemy.Type.Dragon")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Type_Worm,"Enemy.Type.Worm")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Type_Beholder,"Enemy.Type.Beholder")

UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_State_Combat,"Enemy.State.Combat")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_State_Move,"Enemy.State.Move")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_State_Dead,"Enemy.State.Dead")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_State_Burrowed,"Enemy.State.Burrowed")

UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Ability_Attack, "Enemy.Ability.Attack")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Ability_Attack_Melee,"Enemy.Ability.Attack.Melee")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Ability_Attack_Range,"Enemy.Ability.Attack.Range")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Ability_Burrow,"Enemy.Ability.Burrow")


