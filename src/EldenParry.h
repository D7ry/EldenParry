#pragma once
#include <memory>
#include "lib/PrecisionAPI.h"


class EldenParry
{
public:
	static EldenParry* GetSingleton() {
		static EldenParry singleton;
		return std::addressof(singleton);
	}

	void init();

	/// <summary>
	/// Try to process a parry by the parrier.
	/// </summary>
	/// <param name="a_attacker"></param>
	/// <param name="a_parrier"></param>
	/// <returns>True if the parry is successful.</returns>
	bool processPhysicalParry(RE::Actor* a_attacker, RE::Actor* a_parrier);

	bool processProjectileParry(RE::Actor* a_blocker, RE::Projectile* a_projectile, RE::hkpCollidable* a_projectile_collidable);
	void playParryEffects(RE::Actor* a_parrier);

	void updateBashButtonHeldTime(float a_time);
	PRECISION_API::IVPrecision1* _precision_API;
	RE::BGSSoundDescriptorForm* parrySound;
	float GMST_fCombatHitConeAngle;
	float parryAngle;
	float bashButtonHeldTime;
	static PRECISION_API::PreHitCallbackReturn precisionPrehitCallbackFunc(const PRECISION_API::PrecisionHitData& a_precisionHitData);

private:
	inline bool canParry(RE::Actor* a_parrier);
	inline bool canParry(RE::Actor* a_parrier, RE::Projectile* a_proj);
	inline bool canParry(RE::Actor* a_parrier, RE::Actor* a_obj);
	inline bool inBlockAngle(RE::Actor* a_blocker, RE::TESObjectREFR* a_obj);
};


