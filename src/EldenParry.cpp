#include "EldenParry.h"
#include "Settings.h"
#include "Utils.hpp"


void EldenParry::init() {
	INFO("Obtaining precision API...");
	_precision_API = reinterpret_cast<PRECISION_API::IVPrecision1*>(PRECISION_API::RequestPluginAPI());
	if (_precision_API) {
		INFO("Precision API successfully obtained.");
		Settings::facts::isPrecisionAPIObtained = true;
		if (_precision_API->AddPreHitCallback(SKSE::GetPluginHandle(), precisionPrehitCallbackFunc) ==
			PRECISION_API::APIResult::OK) {
			INFO("Successfully registered precision API prehit callback.");
		}
	} else {
		INFO("Precision API not found.");
	}

	//read parry sound
	parrySound = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0xD62, "EldenParry.esp");
	if (!parrySound) {
		ERROR("Parry sound not found");
	}

	//read fcombatHitConeAngle
	GMST_fCombatHitConeAngle = RE::GameSettingCollection::GetSingleton()->GetSetting("fCombatHitConeAngle")->GetFloat();
	parryAngle = GMST_fCombatHitConeAngle;
}

bool EldenParry::inBlockAngle(RE::Actor* a_blocker, RE::TESObjectREFR* a_obj)
{
	auto angle = a_blocker->GetHeadingAngle(a_obj->GetPosition(), false);
	return (angle <= parryAngle && angle >= -parryAngle);
}

bool EldenParry::canParry(RE::Actor* a_actor) {
	if (a_actor->IsPlayerRef()) {
		if (bashButtonHeldTime > Settings::fParryTimeWindow) {//check button held time for player
			return false;
		}
	} else if (!Settings::bEnableNPCParry) {
		return false;
	}
	if (a_actor->GetAttackState() != RE::ATTACK_STATE_ENUM::kBash) {  //parrier has to be bashing
		return false;
	}
	if (Utils::isEquippedShield(a_actor)) {  //check settings conditions
		if (!Settings::bEnableShieldParry) {
			return false;
		}
	} else {
		if (!Settings::bEnableWeaponParry) {
			return false;
		}
	}

	return true;
}

bool EldenParry::canParry(RE::Actor* a_parrier, RE::Actor* a_attacker)
{
	return canParry(a_parrier) && inBlockAngle(a_parrier, a_attacker);
}

bool EldenParry::canParry(RE::Actor* a_parrier, RE::Projectile* a_proj) {
	if (a_proj->spell) {
		if (!Settings::bEnableMagicProjectileDeflection) {
			return false;
		}
	} else {
		if (!Settings::bEnableArrowProjectileDeflection) {
			return false;
		}
	}
	return canParry(a_parrier) && inBlockAngle(a_parrier, a_proj);
}


bool EldenParry::processPhysicalParry(RE::Actor* a_attacker, RE::Actor* a_parrier)
{
	if (!canParry(a_parrier, a_attacker)) {
		return false;
	}

	playParryEffects(a_parrier);
	Utils::triggerStagger(a_parrier, a_attacker, 10);
	return true;
}

bool EldenParry::processProjectileParry(RE::Actor* a_parrier, RE::Projectile* a_projectile, RE::hkpCollidable* a_projectile_collidable)
{
	if (!canParry(a_parrier, a_projectile)) {
		return false;
	}
	


	return true;
}

void EldenParry::playParryEffects(RE::Actor* a_parrier) {
	Utils::playSound(a_parrier, parrySound);

}

void EldenParry::updateBashButtonHeldTime(float a_time) {
	INFO(std::to_string(a_time));
	bashButtonHeldTime = a_time;
}

PRECISION_API::PreHitCallbackReturn EldenParry::precisionPrehitCallbackFunc(const PRECISION_API::PrecisionHitData& a_precisionHitData) {
	PRECISION_API::PreHitCallbackReturn returnData;
	if (!a_precisionHitData.target->Is(RE::FormType::ActorCharacter)) {
		return returnData;
	}
	if (EldenParry::GetSingleton()->processPhysicalParry(a_precisionHitData.attacker, a_precisionHitData.target->As<RE::Actor>())) {
		returnData.bIgnoreHit = true;
	}
	return returnData;
}
