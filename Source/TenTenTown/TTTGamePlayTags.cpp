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
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Event_Fighter_Attack,"Event.Fighter.Attack")
//스테이트
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Movement_IsJumping,"State.Movement.IsJumping")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Movement_IsDoubleJumping,"State.Movement.IsDoubleJumping")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Movement_IsDashing,"State.Movement.IsDashing")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Movement_IsSprinting,"State.Movement.IsSprinting")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_IsChanelling,"State.IsChannelling")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_UltimateOnGoing,"State.Fighter.UltimateOnGoing")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Block_Movement,"State.Block.Movement")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Block_Everything,"State.Block.Everything")
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
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Fighter_PunchRight,"GameplayCue.Fighter.PunchRight")
UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Fighter_PunchLeft,"GameplayCue.Fighter.PunchLeft")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Fighter_PunchHit,"GameplayCue.Fighter.PunchHit")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Fighter_PunchWhirlWind,"GameplayCue.Fighter.PunchWhirlWind")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Fighter_PunchWhirlWindHit,"GameplayCue.Fighter.PunchWhirlWindHit")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::GameplayCue_Fighter_KickWind,"GameplayCue.Fighter.KickWind")
//쿨다운
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Cooldown_Dash,"Cooldown.Dash")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Cooldown_Fireball,"Cooldown.Fireball")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Cooldown_Fighter_Ultimate,"Cooldown.Fighter.Ultimate")

//구조물
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_IsInstall, "State.IsInstall")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Structure_Confirm, "State.Structure.Confirm")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::State_Structure_Cancel,  "State.Structure.Cancel")


// --Enemy
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_State_Combat,"Enemy.State.Combat")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_State_Move,"Enemy.State.Move")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_State_Dead,"Enemy.State.Dead")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_State_KnockBack,"Enemy.State.KnockBack")
UE_DEFINE_GAMEPLAY_TAG(GASTAG::Enemy_Ability_Attack,"Enemy.Ability.Attack")


