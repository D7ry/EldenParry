#pragma once


#include <memory>
#include "lib/PrecisionAPI.h"
#include "lib/ValhallaCombatAPI.h"
#include <mutex>
#include <shared_mutex>

#include <unordered_set>


class EldenParry {

	private:

		static PRECISION_API::PreHitCallbackReturn precisionPrehitCallbackFunc(
			const PRECISION_API::PrecisionHitData &);

		void playParryEffects(RE::Actor * parrier);

		inline bool inBlockAngle(RE::Actor * blocker,RE::TESObjectREFR *);
		inline bool inParryState(RE::Actor * parrier,bool projectileParry);
		inline bool canParry(RE::Actor * parrier,RE::Actor * object);
		inline bool canParry(RE::Actor * parrier,RE::Projectile *);

		std::unordered_map<RE::Actor *,float> _parryCostQueue;
		std::unordered_set<RE::Actor *> _parrySuccessActors;
		
		RE::BGSSoundDescriptorForm
			* _parrySound_shd ,
			* _parrySound_wpn ;

		float 
			_GMST_fCombatHitConeAngle ;
			_bashButtonHeldTime ,
			_parryAngle ;

		mutable std::shared_mutex mtx_parryCostQueue;
		mutable std::shared_mutex mtx_parrySuccessActors;


	public:

		static EldenParry * GetSingleton(){
			static EldenParry singleton;
			return std::addressof(singleton);
		}

		void init();

		
		/**
		 *	@brief Try to process a parry by the parrier.
		 * 
		 *	@returns True if the parry is successful. 
		 */

		bool processMeleeParry(
			RE::Actor * attacker ,
			RE::Actor * parrier);

		bool processProjectileParry(
			RE::Actor * blocker ,
			RE::Projectile * projectile,
			RE::hkpCollidable * collidable);

		void updateBashButtonHeldTime(float time);

		PRECISION_API::IVPrecision1 * _precision_API;
		VAL_API::IVVAL1 * _ValhallaCombat_API;

		void negateParryCost(RE::Actor *);
		void applyParryCost(RE::Actor *);
		void cacheParryCost(RE::Actor *,float cost);
};


