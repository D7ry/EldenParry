#pragma once
class Utils
{
private:
#define PI 3.1415926535897932384626f
	typedef void(_fastcall* tPushActorAway_sub_14067D4A0)(RE::AIProcess* a_causer, RE::Actor* a_target, RE::NiPoint3& a_origin, float a_magnitude);
	inline static REL::Relocation<tPushActorAway_sub_14067D4A0> _pushActorAway{ REL::ID(38858) };


	static inline int soundHelper_a(void* manager, RE::BSSoundHandle* a2, int a3, int a4)  //sub_140BEEE70
	{
		using func_t = decltype(&soundHelper_a);
		REL::Relocation<func_t> func{ RELOCATION_ID(66401, 67663) };
		return func(manager, a2, a3, a4);
	}

	static inline void soundHelper_b(RE::BSSoundHandle* a1, RE::NiAVObject* source_node)  //sub_140BEDB10
	{
		using func_t = decltype(&soundHelper_b);
		REL::Relocation<func_t> func{ RELOCATION_ID(66375, 67636) };
		return func(a1, source_node);
	}

	static inline char __fastcall soundHelper_c(RE::BSSoundHandle* a1)  //sub_140BED530
	{
		using func_t = decltype(&soundHelper_c);
		REL::Relocation<func_t> func{ RELOCATION_ID(66355, 67616) };
		return func(a1);
	}

	static inline char set_sound_position(RE::BSSoundHandle* a1, float x, float y, float z)
	{
		using func_t = decltype(&set_sound_position);
		REL::Relocation<func_t> func{ RELOCATION_ID(66370, 67631) };
		return func(a1, x, y, z);
	}

	static inline const RE::BSFixedString poise_largest = "poise_largest_start";
	static inline const RE::BSFixedString poise_largest_fwd = "poise_largest_start_fwd";
	static inline const RE::BSFixedString poise_large = "poise_large_start";
	static inline const RE::BSFixedString poise_large_fwd = "poise_large_start_fwd";
	static inline const RE::BSFixedString poise_med = "poise_med_start";
	static inline const RE::BSFixedString poise_med_fwd = "poise_med_start_fwd";
	static inline const RE::BSFixedString poise_small = "poise_small_start";
	static inline const RE::BSFixedString poise_small_fwd = "poise_small_start_fwd";

	static inline const RE::BSFixedString staggerDirection = "staggerDirection";
	static inline const RE::BSFixedString StaggerMagnitude = "StaggerMagnitude";
	static inline const RE::BSFixedString staggerStart = "staggerStart";
	static inline const RE::BSFixedString staggerStop = "staggerStop";
	static inline const RE::BSFixedString bleedOutStart = "BleedoutStart";
	static inline const RE::BSFixedString bleedOutStop = "BleedOutStop";
	static inline const RE::BSFixedString bleedOutGraphBool = "IsBleedingOut";

	static inline const RE::BSFixedString recoilLargeStart = "recoilLargeStart";



	static inline bool ApproximatelyEqual(float A, float B)
	{
		return ((A - B) < FLT_EPSILON) && ((B - A) < FLT_EPSILON);
	}

	static inline void SetRotationMatrix(RE::NiMatrix3& a_matrix, float sacb, float cacb, float sb)
	{
		float cb = std::sqrtf(1 - sb * sb);
		float ca = cacb / cb;
		float sa = sacb / cb;
		a_matrix.entry[0][0] = ca;
		a_matrix.entry[0][1] = -sacb;
		a_matrix.entry[0][2] = sa * sb;
		a_matrix.entry[1][0] = sa;
		a_matrix.entry[1][1] = cacb;
		a_matrix.entry[1][2] = -ca * sb;
		a_matrix.entry[2][0] = 0.0;
		a_matrix.entry[2][1] = sb;
		a_matrix.entry[2][2] = cb;
	}

	static bool PredictAimProjectile(RE::NiPoint3 a_projectilePos, RE::NiPoint3 a_targetPosition, RE::NiPoint3 a_targetVelocity, float a_gravity, RE::NiPoint3& a_projectileVelocity)
	{
		// http://ringofblades.com/Blades/Code/PredictiveAim.cs

		float projectileSpeedSquared = a_projectileVelocity.SqrLength();
		float projectileSpeed = std::sqrtf(projectileSpeedSquared);

		if (projectileSpeed <= 0.f || a_projectilePos == a_targetPosition) {
			return false;
		}

		float targetSpeedSquared = a_targetVelocity.SqrLength();
		float targetSpeed = std::sqrtf(targetSpeedSquared);
		RE::NiPoint3 targetToProjectile = a_projectilePos - a_targetPosition;
		float distanceSquared = targetToProjectile.SqrLength();
		float distance = std::sqrtf(distanceSquared);
		RE::NiPoint3 direction = targetToProjectile;
		direction.Unitize();
		RE::NiPoint3 targetVelocityDirection = a_targetVelocity;
		targetVelocityDirection.Unitize();

		float cosTheta = (targetSpeedSquared > 0) ? direction.Dot(targetVelocityDirection) : 1.0f;

		bool bValidSolutionFound = true;
		float t;

		if (ApproximatelyEqual(projectileSpeedSquared, targetSpeedSquared)) {
			// We want to avoid div/0 that can result from target and projectile traveling at the same speed
			//We know that cos(theta) of zero or less means there is no solution, since that would mean B goes backwards or leads to div/0 (infinity)
			if (cosTheta > 0) {
				t = 0.5f * distance / (targetSpeed * cosTheta);
			} else {
				bValidSolutionFound = false;
				t = 1;
			}
		} else {
			float a = projectileSpeedSquared - targetSpeedSquared;
			float b = 2.0f * distance * targetSpeed * cosTheta;
			float c = -distanceSquared;
			float discriminant = b * b - 4.0f * a * c;

			if (discriminant < 0) {
				// NaN
				bValidSolutionFound = false;
				t = 1;
			} else {
				// a will never be zero
				float uglyNumber = sqrtf(discriminant);
				float t0 = 0.5f * (-b + uglyNumber) / a;
				float t1 = 0.5f * (-b - uglyNumber) / a;

				// Assign the lowest positive time to t to aim at the earliest hit
				t = min(t0, t1);
				if (t < FLT_EPSILON) {
					t = max(t0, t1);
				}

				if (t < FLT_EPSILON) {
					// Time can't flow backwards when it comes to aiming.
					// No real solution was found, take a wild shot at the target's future location
					bValidSolutionFound = false;
					t = 1;
				}
			}
		}

		a_projectileVelocity = a_targetVelocity + (-targetToProjectile / t);

		if (!bValidSolutionFound) {
			a_projectileVelocity.Unitize();
			a_projectileVelocity *= projectileSpeed;
		}

		if (!ApproximatelyEqual(a_gravity, 0.f)) {
			float netFallDistance = (a_projectileVelocity * t).z;
			float gravityCompensationSpeed = (netFallDistance + 0.5f * a_gravity * t * t) / t;
			a_projectileVelocity.z = gravityCompensationSpeed;
		}

		return bValidSolutionFound;
	}



public:
	static void triggerStagger(RE::Actor* a_aggressor, RE::Actor* a_reactor, float a_reactionMagnitude)
	{
		auto headingAngle = a_reactor->GetHeadingAngle(a_aggressor->GetPosition(), false);
		auto direction = (headingAngle >= 0.0f) ? headingAngle / 360.0f : (360.0f + headingAngle) / 360.0f;
		a_reactor->SetGraphVariableFloat(staggerDirection, direction);
		a_reactor->SetGraphVariableFloat(StaggerMagnitude, a_reactionMagnitude);
		a_reactor->NotifyAnimationGraph(staggerStart);
	};

	static bool isEquippedShield(RE::Actor* a_actor)
	{
		auto lhs = a_actor->GetEquippedObject(true);
		return lhs && lhs->IsArmor();
	}

	static void resetProjectileOwner(RE::Projectile* a_projectile, RE::Actor* a_actor, RE::hkpCollidable* a_projectile_collidable)
	{
		a_projectile->SetActorCause(a_actor->GetActorCause());
		a_projectile->shooter = a_actor->GetHandle();
		uint32_t a_collisionFilterInfo;
		a_actor->GetCollisionFilterInfo(a_collisionFilterInfo);
		a_projectile_collidable->broadPhaseHandle.collisionFilterInfo &= (0x0000FFFF);
		a_projectile_collidable->broadPhaseHandle.collisionFilterInfo |= (a_collisionFilterInfo << 16);
	}

	inline static void PushActorAway(RE::Actor* causer, RE::Actor* target, float magnitude)
	{
		auto targetPoint = causer->GetNodeByName(causer->race->bodyPartData->parts[0]->targetName.c_str());
		RE::NiPoint3 vec = targetPoint->world.translate;
		//RE::NiPoint3 vec = causer->GetPosition();
		_pushActorAway(causer->currentProcess, target, vec, magnitude);
	}

		/*Play sound with formid at a certain actor's position.
	@param a: actor on which to play sonud.
	@param formid: formid of the sound descriptor.*/
	static void playSound(RE::Actor* a, RE::BGSSoundDescriptorForm* a_descriptor)
	{
		RE::BSSoundHandle handle;
		handle.soundID = static_cast<uint32_t>(-1);
		handle.assumeSuccess = false;
		*(uint32_t*)&handle.state = 0;


		soundHelper_a(RE::BSAudioManager::GetSingleton(), &handle, a_descriptor->GetFormID(), 16);
		if (set_sound_position(&handle, a->data.location.x, a->data.location.y, a->data.location.z)) {
			soundHelper_b(&handle, a->Get3D());
			soundHelper_c(&handle);
		}
	}

	static void ReflectProjectile(RE::Projectile* a_projectile)
	{
		a_projectile->linearVelocity *= -1.f;

		// rotate model
		auto projectileNode = a_projectile->Get3D2();
		if (projectileNode) {
			RE::NiPoint3 direction = a_projectile->linearVelocity;
			direction.Unitize();

			a_projectile->data.angle.x = asin(direction.z);
			a_projectile->data.angle.z = atan2(direction.x, direction.y);

			if (a_projectile->data.angle.z < 0.0) {
				a_projectile->data.angle.z += PI;
			}

			if (direction.x < 0.0) {
				a_projectile->data.angle.z += PI;
			}

			Utils::SetRotationMatrix(projectileNode->local.rotate, -direction.x, direction.y, direction.z);
		}
	}

	/*Get the body position of this actor.*/
	static void getBodyPos(RE::Actor* a_actor, RE::NiPoint3& pos)
	{
		if (!a_actor->race) {
			return;
		}
		RE::BGSBodyPart* bodyPart = a_actor->race->bodyPartData->parts[0];
		if (!bodyPart) {
			return;
		}
		auto targetPoint = a_actor->GetNodeByName(bodyPart->targetName.c_str());
		if (!targetPoint) {
			return;
		}

		pos = targetPoint->world.translate;
	}

	/*retarget this projectile to a_target.*/
	static void RetargetProjectile(RE::Projectile* a_projectile, RE::TESObjectREFR* a_target)
	{
		a_projectile->desiredTarget = a_target;

		auto projectileNode = a_projectile->Get3D2();
		auto targetHandle = a_target->GetHandle();

		RE::NiPoint3 targetPos = a_target->GetPosition();
		if (a_target->GetFormType() == RE::FormType::ActorCharacter) {
			getBodyPos(a_target->As<RE::Actor>(), targetPos);
		}

		RE::NiPoint3 targetVelocity;
		targetHandle.get()->GetLinearVelocity(targetVelocity);

		float projectileGravity = 0.f;
		if (auto ammo = a_projectile->ammoSource) {
			if (auto bgsProjectile = ammo->data.projectile) {
				projectileGravity = bgsProjectile->data.gravity;
				if (auto bhkWorld = a_projectile->parentCell->GetbhkWorld()) {
					if (auto hkpWorld = bhkWorld->GetWorld1()) {
						auto vec4 = hkpWorld->gravity;
						float quad[4];
						_mm_store_ps(quad, vec4.quad);
						float gravity = -quad[2] * RE::bhkWorld::GetWorldScaleInverse();
						projectileGravity *= gravity;
					}
				}
			}
		}

		PredictAimProjectile(a_projectile->data.location, targetPos, targetVelocity, projectileGravity, a_projectile->linearVelocity);

		// rotate
		RE::NiPoint3 direction = a_projectile->linearVelocity;
		direction.Unitize();

		a_projectile->data.angle.x = asin(direction.z);
		a_projectile->data.angle.z = atan2(direction.x, direction.y);

		if (a_projectile->data.angle.z < 0.0) {
			a_projectile->data.angle.z += PI;
		}

		if (direction.x < 0.0) {
			a_projectile->data.angle.z += PI;
		}

		SetRotationMatrix(projectileNode->local.rotate, -direction.x, direction.y, direction.z);
	}

	/*Slow down game time for a set period.
	@param a_duration: duration of the slow time.
	@param a_percentage: relative time speed to normal time(1).*/
	static void slowTime(float a_duration, float a_percentage)
	{
		int duration_milisec = static_cast<int>(a_duration * 1000);
		RE::BSTimer::setCurrentGlobalTimeMult(a_percentage);
		/*Reset time here*/
		auto resetSlowTime = [](int a_duration) {
			std::this_thread::sleep_for(std::chrono::milliseconds(a_duration));
			RE::BSTimer::setCurrentGlobalTimeMult(1);
		};
		std::jthread resetThread(resetSlowTime, duration_milisec);
		resetThread.detach();
	}
};


class blockSpark
{
private:
	static auto getBipedIndex(RE::TESForm* parryEquipment, bool rightHand) {
		if (!parryEquipment)
			return RE::BIPED_OBJECT::kNone;

		if (parryEquipment->As<RE::TESObjectWEAP>()) {
			switch (parryEquipment->As<RE::TESObjectWEAP>()->GetWeaponType()) {
			case RE::WEAPON_TYPE::kOneHandSword:
				return rightHand ? RE::BIPED_OBJECT::kOneHandSword : RE::BIPED_OBJECT::kShield;
			case RE::WEAPON_TYPE::kOneHandAxe:
				return rightHand ? RE::BIPED_OBJECT::kOneHandAxe : RE::BIPED_OBJECT::kShield;
			case RE::WEAPON_TYPE::kOneHandMace:
				return rightHand ? RE::BIPED_OBJECT::kOneHandMace : RE::BIPED_OBJECT::kShield;
			case RE::WEAPON_TYPE::kOneHandDagger:
				return rightHand ? RE::BIPED_OBJECT::kOneHandDagger : RE::BIPED_OBJECT::kShield;
			case RE::WEAPON_TYPE::kTwoHandAxe:
			case RE::WEAPON_TYPE::kTwoHandSword:
			case RE::WEAPON_TYPE::kHandToHandMelee:
				return RE::BIPED_OBJECT::kTwoHandMelee;
			}
		} else if (parryEquipment->IsArmor())
			return RE::BIPED_OBJECT::kShield;

		return RE::BIPED_OBJECT::kNone;
	}

public:
	static void playBlockSpark(RE::Actor* a_actor)
	{
		if (!a_actor || !a_actor->currentProcess || !a_actor->currentProcess->high || !a_actor->Get3D()) {
			return;
		}
		RE::BIPED_OBJECT BipeObjIndex;
		auto defenderLeftEquipped = a_actor->GetEquippedObject(true);

		if (defenderLeftEquipped && (defenderLeftEquipped->IsWeapon() || defenderLeftEquipped->IsArmor())) {
			BipeObjIndex = getBipedIndex(defenderLeftEquipped, false);
		} else {
			BipeObjIndex = getBipedIndex(a_actor->GetEquippedObject(false), true);
		}

		if (BipeObjIndex == RE::BIPED_OBJECT::kNone) {
			return;
		}

		auto defenderNode = a_actor->GetCurrentBiped()->objects[BipeObjIndex].partClone;
		if (!defenderNode || !defenderNode.get()) {
			return;
		}
		const char* modelName;
		if (BipeObjIndex == RE::BIPED_OBJECT::kShield && defenderLeftEquipped && defenderLeftEquipped->IsArmor()) {
			if (Settings::facts::isValhallaCombatAPIObtained) {
				modelName = "ValhallaCombat\\impactShieldRoot.nif";
			} else {
				modelName = "EldenParry\\impactShieldRoot.nif";
			}
			
		} else {
			if (Settings::facts::isValhallaCombatAPIObtained) {
				modelName = "ValhallaCombat\\impactWeaponRoot.nif";
			} else {
				modelName = "EldenParry\\impactWeaponRoot.nif";
			}
		}
		//DEBUG("Get Weapon Spark Position!");
		a_actor->GetParentCell()->PlaceParticleEffect(0.0f, modelName, defenderNode->world.rotate, defenderNode->worldBound.center, 1.0f, 4U, defenderNode.get());
	}
};

namespace inlineUtils
{
	inline bool isPowerAttacking(RE::Actor* a_actor) {
		if (a_actor->currentProcess && a_actor->currentProcess->high) {
			auto atkData = a_actor->currentProcess->high->attackData.get();
			if (atkData) {
				return atkData->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack);
			}
		}
		return false;
	}

	
	inline void restoreAv(RE::Actor* a_actor, RE::ActorValue a_actorValue, float a_val)
	{
		if (a_val == 0) {
			return;
		}
		if (a_actor) {
			a_actor->As<RE::ActorValueOwner>()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, a_actorValue, a_val);
		}
	}

	inline void damageAv(RE::Actor* a_actor, RE::ActorValue a_actorValue, float a_val)
	{
		if (a_val == 0) {
			return;
		}
		if (a_actor) {
			a_actor->As<RE::ActorValueOwner>()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, a_actorValue, -a_val);
		}
	}


	typedef void(_fastcall* _shakeCamera)(float strength, RE::NiPoint3 source, float duration);
	inline static REL::Relocation<_shakeCamera> shakeCamera{ RELOCATION_ID(32275, 33012) };
};
