#pragma once


#include "PCH.h"
#include "EldenParry.h"
#include "Settings.h"
#include "Utils.hpp"


namespace Hooks {


	//Actor__sub_140627930+16E	call ActorValueOwner__sub_1403BEC90

	class Hook_getAttackStaminaCost {

		/* to cancel out vanilla power attack stamina consumption. */

		private:

			static inline REL::Relocation<decltype(getAttackStaminaCost)> _getAttackStaminaCost;


			static float getAttackStaminaCost(uintptr_t owner,RE::BGSAttackData * data){

				if(data -> data.flags.any(RE::AttackData::AttackFlag::kBashAttack)){
					
					EldenParry::GetSingleton() -> cacheParryCost(
						(RE::Actor *)(owner - 0xB0) ,
						_getAttackStaminaCost(owner,data));
					
					return 0;
				}

				return _getAttackStaminaCost(owner,data);
			}

	
		public:

			static void install(){
				
				auto & trampoline = SKSE::GetTrampoline();

				// SE:627930 + 16E => 3BEC90 AE:64D350 + 171 => 3D6720

				REL::Relocation<uintptr_t> hook { RELOCATION_ID(37650, 38603) };  

				#ifdef SKYRIM_SUPPORT_AE
					int offset = 0x171;
				#else
					int offset = 0x16E;
				#endif


				_getAttackStaminaCost = trampoline.write_call<5>
					(hook.address() + offset,getAttackStaminaCost);

				logger::info("Attack stamina hook installed.");
			}

	
		
	};


	class MeleeCollision {

		private:

			static inline REL::Relocation<decltype(processHit1)> _ProcessHit1;
			static inline REL::Relocation<decltype(processHit)> _ProcessHit;

			static inline bool didAggressorParry(RE::Actor * aggressor){

				if(aggressor -> GetAttackState() != RE::ATTACK_STATE_ENUM::kBash)
					return false;

				if(inlineUtils::isPowerAttacking(aggressor))
					return false;

				bool canParry = 
					aggressor -> IsPlayerRef() || 
					Settings::bEnableNPCParry;
				
				bool isParryWeapon = Utils::isEquippedShield(aggressor) 
					? Settings::bEnableShieldParry 
					: Settings::bEnableWeaponParry ;

				return canParry && isParryWeapon;
			}

			static inline bool shouldIgnoreHit(
				RE::Actor * aggressor ,
				RE::Actor * victim ,
				std::int64_t integer,
				bool boolean,
				void * pointer
			){
				
				// Aggressor: Cancel parry hitframe
				
				if(didAggressorParry(aggressor))
					return true;

				// Victim: Process parry
				
				return EldenParry::GetSingleton() 
					-> processMeleeParry(aggressor,victim))
			}

			
			static void processHit(
				RE::Actor * aggressor ,
				RE::Actor * victim ,
				std::int64_t integer,
				bool boolean,
				void * pointer
			){
				if(!shouldIgnoreHit(aggressor,victim,integer,boolean,pointer))
					_ProcessHit(aggressor,victim,integer,boolean,pointer);
			}


			static void processHit1(
				RE::Actor * aggressor ,
				RE::Actor * victim ,
				std::int64_t integer,
				bool boolean,
				void * pointer
			){
				if(!shouldIgnoreHit(aggressor,victim,integer,boolean,pointer))
					_ProcessHit1(aggressor,victim,integer,boolean,pointer);
			}


		public:

			static void install(){

				//SE:627930 + 38B AE:64D350 + 40A / 45A

				REL::Relocation<uintptr_t> hook { RELOCATION_ID(37650,38603) };
				
				auto & trampoline = SKSE::GetTrampoline();

				#ifdef SKYRIM_SUPPORT_AE
					int offset = 0x45A;
				#else
					int offset = 0x38B;
				#endif

				_ProcessHit = trampoline.write_call<5>
					(hook.address() + offset,processHit);

				logger::info("Melee Hit hook installed.");
			}
	};


	class AttackBlockHandler {
	
		private:

			static inline REL::Relocation<decltype(ProcessButton)> _ProcessButton;


			static void ProcessButton(
				RE::AttackBlockHandler * handler,
				RE::ButtonEvent * event,
				RE::PlayerControlsData * data
			){
				auto character = RE::PlayerCharacter::GetSingleton();

				if(
					character && 
					character -> GetAttackState() == RE::ATTACK_STATE_ENUM::kBash && 
					event -> QUserEvent() == "Right Attack/Block"
				){
					if(event -> IsHeld())
						EldenParry::GetSingleton() -> updateBashButtonHeldTime(event -> HeldDuration());
					
					if(event -> IsUp())
						EldenParry::GetSingleton() -> updateBashButtonHeldTime(0);
				}

				_ProcessButton(handler,event,data);
			}



		public:
		
			static void install(){

				REL::Relocation<std::uintptr_t> AttackBlockHandlerVtbl { RE::VTABLE_AttackBlockHandler[0] };
				_ProcessButton = AttackBlockHandlerVtbl.write_vfunc(0x4,ProcessButton);
			}
	};


	class ProjectileCollision {
		private:

			static inline REL::Relocation<decltype(OnMissileCollision)> _missileCollission;
			static inline REL::Relocation<decltype(OnArrowCollision)> _arrowCollission;


			static inline bool initProjectileBlock(
				RE::Projectile * projectile ,
				RE::hkpAllCdPointCollector * collector
			){
				if(!collector)
					return false;

				for(auto & hit : collector -> hits){

					auto collidableA = RE::TESHavokUtilities::FindCollidableRef(* hit.rootCollidableA);
					
					if(collidableA && collidableA -> formType == RE::FormType::ActorCharacter)
						return EldenParry::GetSingleton() -> processProjectileParry(
							collidableA -> As<RE::Actor>() , projectile , 
							const_cast<RE::hkpCollidable*>(hit.rootCollidableB)
						);

					auto collidableB = RE::TESHavokUtilities::FindCollidableRef(* hit.rootCollidableB);
		
					if(collidableB && collidableB -> formType == RE::FormType::ActorCharacter)
						return EldenParry::GetSingleton() -> processProjectileParry(
							collidableB -> As<RE::Actor>() , projectile , 
							const_cast<RE::hkpCollidable*>(hit.rootCollidableA)
						);
				}
				
				return false;
			}


			static void OnArrowCollision(
				RE::Projectile * projectile ,
				RE::hkpAllCdPointCollector * collector
			){
				if(!initProjectileBlock(projectile,collector))
					_arrowCollission(projectile,collector);
			}


			static void OnMissileCollision(
				RE::Projectile * projectile ,
				RE::hkpAllCdPointCollector * collector
			){
				if(!initProjectileBlock(projectile,collector))
					_missileCollission(projectile,collector);
			}
		

			public:

				static void install(){

					REL::Relocation<std::uintptr_t> 
						missileProjectileVtbl { RE::VTABLE_MissileProjectile[0] } ,
						arrowProjectileVtbl { RE::VTABLE_ArrowProjectile[0] } ;

					_arrowCollission = arrowProjectileVtbl.write_vfunc(190,OnArrowCollision);
					_missileCollission = missileProjectileVtbl.write_vfunc(190,OnMissileCollision);
				};
	};


	static void install(){

		SKSE::AllocTrampoline(1 << 5);
		
		if(Settings::bSuccessfulParryNoCost)
			Hook_getAttackStaminaCost::install();
		
		AttackBlockHandler::install();
		MeleeCollision::install();
		ProjectileCollision::install();
	}
}
