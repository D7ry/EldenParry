#pragma once
class Utils
{
private:
#define PI 3.1415926535897932384626
	typedef void(_fastcall* tPushActorAway_sub_14067D4A0)(RE::AIProcess* a_causer, RE::Actor* a_target, RE::NiPoint3& a_origin, float a_magnitude);
	inline static REL::Relocation<tPushActorAway_sub_14067D4A0> _pushActorAway{ REL::ID(38858) };

	template <typename Iter, typename RandomGenerator>
	static inline Iter select_randomly(Iter start, Iter end, RandomGenerator& g)
	{
		std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
		std::advance(start, dis(g));
		return start;
	}

	template <typename Iter>
	static inline Iter select_randomly(Iter start, Iter end)
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		return select_randomly(start, end, gen);
	}

	

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


	static inline const RE::BSFixedString

		poise_largest_fwd = "poise_largest_start_fwd" ,
		poise_largest = "poise_largest_start" ,

		poise_large_fwd = "poise_large_start_fwd" ,
		poise_large = "poise_large_start" ,

		poise_med_fwd = "poise_med_start_fwd" ,
		poise_med = "poise_med_start" ,

		poise_small_fwd = "poise_small_start_fwd" ,
		poise_small = "poise_small_start" ,

		staggerDirection = "staggerDirection" ,
		StaggerMagnitude = "StaggerMagnitude" ,
		staggerStart = "staggerStart" ,
		staggerStop = "staggerStop" ,

		bleedOutGraphBool = "IsBleedingOut" ,
		bleedOutStart = "BleedoutStart" ,
		bleedOutStop = "BleedOutStop" ,

		recoilLargeStart = "recoilLargeStart" ;


	static inline bool ApproximatelyEqual(float A, float B)
	{
		return ((A - B) < FLT_EPSILON) && ((B - A) < FLT_EPSILON);
	}

	static inline void SetRotationMatrix(RE::NiMatrix3& a_matrix, float sacb, float cacb, float sb)
	{
		float 
			cb = std::sqrtf(1 - sb * sb) ,
			ca = cacb / cb ,
			ca = sacb / cb ;

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


	// http://ringofblades.com/Blades/Code/PredictiveAim.cs

	static bool PredictAimProjectile(
		RE::NiPoint3 projectilePos ,
		RE::NiPoint3 targetPosition ,
		RE::NiPoint3 targetVelocity ,
		float gravity ,
		RE::NiPoint3 & projectileVelocity
	){

		if(projectilePos == targetPosition)
			return false;


		float 
			projectileSpeedSquared = projectileVelocity.SqrLength() ,
			projectileSpeed = std::sqrtf(projectileSpeedSquared) ;

		if(projectileSpeed <= 0.f)
			return false;


		float 
			targetSpeedSquared = a_targetVelocity.SqrLength() ,
			targetSpeed = std::sqrtf(targetSpeedSquared) ;
		
		RE::NiPoint3 targetToProjectile = 
			projectilePos - targetPosition;
		
		float 
			distanceSquared = targetToProjectile.SqrLength() ,
			distance = std::sqrtf(distanceSquared) ;
		
		RE::NiPoint3 direction = targetToProjectile;

		direction.Unitize();
		
		RE::NiPoint3 targetVelocityDirection = targetVelocity;
		
		targetVelocityDirection.Unitize();

		float cosTheta = (targetSpeedSquared > 0) 
			? direction.Dot(targetVelocityDirection) 
			: 1.0f;

		bool bValidSolutionFound = true;
		
		float t;

		if(ApproximatelyEqual(projectileSpeedSquared,targetSpeedSquared)){
			
			// We want to avoid div/0 that can result from target and projectile traveling at the same speed
			//We know that cos(theta) of zero or less means there is no solution, since that would mean B goes backwards or leads to div/0 (infinity)
			
			if(cosTheta > 0){
				t = 0.5f * distance / (targetSpeed * cosTheta);
			} else {
				bValidSolutionFound = false;
				t = 1;
			}
		} else {
			
			float 
				a = projectileSpeedSquared - targetSpeedSquared ,
				b = 2.0f * distance * targetSpeed * cosTheta ,
				c = - distanceSquared ;

			float discriminant = b * b - 4.0f * a * c;

			if(discriminant < 0){
				// NaN
				bValidSolutionFound = false;
				t = 1;
			} else {
				
				// a will never be zero
				
				float 
					uglyNumber = sqrtf(discriminant) ,
					t0 = 0.5f * (- b + uglyNumber) / a ,
					t1 = 0.5f * (- b - uglyNumber) / a ;


				// Assign the lowest positive time to t to aim at the earliest hit
				
				t = min(t0,t1);

				if(t < FLT_EPSILON)
					t = max(t0,t1);


				// Time can't flow backwards when it comes to aiming.
				// No real solution was found, take a wild shot at the target's future location

				if(t < FLT_EPSILON){
					bValidSolutionFound = false;
					t = 1;
				}
			}
		}

		projectileVelocity = targetVelocity + (- targetToProjectile / t);

		if(!bValidSolutionFound){
			projectileVelocity.Unitize();
			projectileVelocity *= projectileSpeed;
		}

		if(!ApproximatelyEqual(gravity,0.f)){

			float 
				netFallDistance = (projectileVelocity * t).z ,
				gravityCompensationSpeed = (netFallDistance + 0.5f * gravity * t * t) / t ;
			
			projectileVelocity.z = gravityCompensationSpeed;
		}

		return bValidSolutionFound;
	}



public:

	static void triggerStagger(
		RE::Actor * aggressor ,
		RE::Actor * reactor ,
		float reactionMagnitude
	){
		auto headingAngle = reactor -> GetHeadingAngle(aggressor -> GetPosition(),false);
		
		auto direction = headingAngle;
		
		if(headingAngle < 0.0f) 
			headingAngle += 360.0f;
			
		headingAngle /= 360.0f;

		a_reactor -> SetGraphVariableFloat(staggerDirection,direction);
		a_reactor -> SetGraphVariableFloat(StaggerMagnitude,reactionMagnitude);
		a_reactor -> NotifyAnimationGraph(staggerStart);
	}


	static bool isEquippedShield(RE::Actor * actor){

		auto object = actor -> GetEquippedObject(true);
		
		return object && object -> IsArmor();
	}


	static void resetProjectileOwner(
		RE::Projectile * projectile ,
		RE::Actor * actor ,
		RE::hkpCollidable* collidable
	){
		projectile -> SetActorCause(actor -> GetActorCause());
		projectile -> shooter = actor -> GetHandle();

		uint32_t info;
		actor -> GetCollisionFilterInfo(info);
		
		collidable -> broadPhaseHandle.collisionFilterInfo &= (0x0000FFFF);
		collidable -> broadPhaseHandle.collisionFilterInfo |= (info << 16);
	}


	inline static void PushActorAway(
		RE::Actor * origin ,
		RE::Actor * target ,
		float magnitude
	){
		auto name = origin
			-> race
			-> bodyPartData
			-> parts[0]
			-> targetName.c_str();

		auto point = origin -> GetNodeByName(name);
		auto vector = point -> world.translate;

		_pushActorAway(origin -> currentProcess,target,vector,magnitude);
	}


	/**
	 *	@brief Play sound with formid at a certain actor's position.
	 * 
	 *	@param actor Actor on which to play sonud.
	 *	@param descriptor Formid of the sound descriptor.
	 */

	static void playSound(RE::Actor * actor, RE::BGSSoundDescriptorForm * descriptor){
		
		RE::BSSoundHandle handle;
		handle.soundID = static_cast<uint32_t>(-1);
		handle.assumeSuccess = false;
		
		* (uint32_t *) & handle.state = 0;

		soundHelper_a(RE::BSAudioManager::GetSingleton(),& handle,descriptor -> GetFormID(),16);

		auto location = actor -> data.location;

		if(set_sound_position(& handle,location.x,location.y,location.z)){
			soundHelper_b(& handle,actor -> Get3D());
			soundHelper_c(& handle);
		}
	}


	static void playSound(
		RE::Actor * actor ,
		std::vector<RE::BGSSoundDescriptorForm *> sounds
	){
		playSound(actor,* select_randomly(sounds.begin(),sounds.end()));
	}
		

	static void ReflectProjectile(RE::Projectile * projectile){
		
		projectile -> linearVelocity *= -1.f;

		// rotate model
		
		auto projectileNode = projectile -> Get3D2();
		
		if(projectileNode){

			RE::NiPoint3 direction = projectile -> linearVelocity;

			direction.Unitize();

			projectile -> data.angle.x = asin(direction.z);
			projectile -> data.angle.z = atan2(direction.x,direction.y);

			if(projectile -> data.angle.z < 0.0)
				projectile -> data.angle.z += PI;

			if(direction.x < 0.0)
				projectile -> data.angle.z += PI;

			Utils::SetRotationMatrix(
				projectileNode -> local.rotate ,
				- direction.x , direction.y , direction.z
			);
		}
	}


	/**
	 *	@brief Get the body position of this actor.
	 */

	static void getBodyPos(RE::Actor * actor,RE::NiPoint3 & position){

		if(!actor -> race)
			return;

		auto bodyPart = actor
			-> race
			-> bodyPartData
			-> parts[0];
		
		if(!bodyPart)
			return;
		
		auto 
			name = bodyPart -> targetName.c_str() ,
			point = actor -> GetNodeByName(name) ;
		
		if(!point)
			return;

		position = point -> world.translate;
	}


	/**
	 *	@brief Retarget this projectile to a_target.
	 */

	static void RetargetProjectile(
		RE::Actor * actor ,
		RE::Projectile * projectile ,
		RE::TESObjectREFR * target
	){
		projectile -> desiredTarget = target;

		auto projectileNode = projectile -> Get3D2();
		auto targetHandle = target -> GetHandle();

		RE::NiPoint3 targetPos = target -> GetPosition();

		if(target -> GetFormType() == RE::FormType::ActorCharacter)
			getBodyPos(target -> As<RE::Actor>(),targetPos);

		RE::NiPoint3 targetVelocity;
		targetHandle.get() -> GetLinearVelocity(targetVelocity);

		float projectileGravity = 0.f;

		if(auto ammo = projectile -> ammoSource)
			if(auto bgsProjectile = ammo -> data.projectile){
				
				projectileGravity = bgsProjectile -> data.gravity;
				
				if(auto bhkWorld = projectile -> parentCell -> GetbhkWorld())
					if(auto hkpWorld = bhkWorld -> GetWorld1()){

						auto vec4 = hkpWorld -> gravity;
						
						float quad[4];

						_mm_store_ps(quad, vec4.quad);
						
						float gravity = - quad[2] * RE::bhkWorld::GetWorldScaleInverse();
						
						projectileGravity *= gravity;
					}
			}

		PredictAimProjectile(
			projectile -> data.location , 
			targetPos , targetVelocity , 
			projectileGravity , 
			projectile -> linearVelocity
		);

		// rotate
		
		RE::NiPoint3 direction = projectile -> linearVelocity;

		direction.Unitize();

		projectile -> data.angle.x = asin(direction.z);
		projectile -> data.angle.z = atan2(direction.x,direction.y);

		if(projectile -> data.angle.z < 0.0)
			projectile -> data.angle.z += PI;

		if(direction.x < 0.0)
			projectile -> data.angle.z += PI;

		SetRotationMatrix(projectileNode -> local.rotate,-direction.x,direction.y,direction.z);
	}


	/**
	 *	@brief Slow down game time for a set period.
	 * 
	 *	@param a_duration Duration of the slow time.
	 *	@param a_percentage Relative time speed to normal time(1).
	 */

	static void slowTime(float duration,float percentage){

		int millis = static_cast<int>(duration * 1000);

		RE::BSTimer::setCurrentGlobalTimeMult(percentage);
		
		/* Reset time here */
		
		auto resetSlowTime = [](int duration){
			std::this_thread::sleep_for(std::chrono::milliseconds(duration));
			RE::BSTimer::setCurrentGlobalTimeMult(1);
		};

		std::jthread resetThread(resetSlowTime,millis);
		
		resetThread.detach();
	}
};


class blockSpark
{
private:
	static auto getBipedIndex(RE::TESForm * parryEquipment,bool rightHand){

		if(!parryEquipment)
			return RE::BIPED_OBJECT::kNone;

		auto object = parryEquipment -> As<RE::TESObjectWEAP>();

		if(object){

			auto weaponType = object -> GetWeaponType();

			if(rightHand)
				switch(weaponType){
				case RE::WEAPON_TYPE::kOneHandSword :
					return RE::BIPED_OBJECT::kOneHandSword ;
				case RE::WEAPON_TYPE::kOneHandAxe :
					return RE::BIPED_OBJECT::kOneHandAxe ;
				case RE::WEAPON_TYPE::kOneHandMace :
					return RE::BIPED_OBJECT::kOneHandMace ;
				case RE::WEAPON_TYPE::kOneHandDagger :
					return RE::BIPED_OBJECT::kOneHandDagger ;
				case RE::WEAPON_TYPE::kHandToHandMelee :
				case RE::WEAPON_TYPE::kTwoHandSword :
				case RE::WEAPON_TYPE::kTwoHandAxe :
					return RE::BIPED_OBJECT::kTwoHandMelee ;
				}
			else
				switch(weaponType){
				case RE::WEAPON_TYPE::kOneHandSword :
				case RE::WEAPON_TYPE::kOneHandAxe :
				case RE::WEAPON_TYPE::kOneHandMace :
				case RE::WEAPON_TYPE::kOneHandDagger :
					return RE::BIPED_OBJECT::kShield ;
				case RE::WEAPON_TYPE::kHandToHandMelee :
				case RE::WEAPON_TYPE::kTwoHandSword :
				case RE::WEAPON_TYPE::kTwoHandAxe :
					return RE::BIPED_OBJECT::kTwoHandMelee ;
				}

			return RE::BIPED_OBJECT::kNone;
		}
			
		
		if(parryEquipment -> IsArmor())
			return RE::BIPED_OBJECT::kShield;

		return RE::BIPED_OBJECT::kNone;
	}

public:

	static void playBlockSpark(RE::Actor * actor){

		if(!actor)
			return;
			
		if(!actor -> currentProcess)
			return;
			
		if(!actor -> currentProcess -> high)
			return;
			
		if(!actor -> Get3D())
			return;

		auto defenderLeftEquipped = actor -> GetEquippedObject(true);

		bool isWeaponOrArmor = 
			defenderLeftEquipped -> IsWeapon() || 
			defenderLeftEquipped -> IsArmor() ;

		auto BipeObjIndex = (defenderLeftEquipped && isWeaponOrArmor)
			? getBipedIndex(defenderLeftEquipped,false) :
			: getBipedIndex(actor -> GetEquippedObject(false),true) ;

		if(BipeObjIndex == RE::BIPED_OBJECT::kNone)
			return;

		auto defenderNode = actor
			-> GetCurrentBiped()
			-> objects[BipeObjIndex]
			.  partClone;
		
		if(!defenderNode)
			return;
			
		if(!defenderNode.get())
			return;
		
		bool hasValhalla = Settings::facts::isValhallaCombatAPIObtained;
		
		bool isShield = 
			BipeObjIndex == RE::BIPED_OBJECT::kShield && 
			defenderLeftEquipped && 
			defenderLeftEquipped -> IsArmor();

		const char * modelName;
		
		if(hasValhalla)
			modelName = (isShield)
				? "ValhallaCombat\\impactShieldRoot.nif"
				: "ValhallaCombat\\impactWeaponRoot.nif" ;
		else
			modelName = (isShield)
				? "EldenParry\\impactShieldRoot.nif"
				: "EldenParry\\impactWeaponRoot.nif" ;

		//DEBUG("Get Weapon Spark Position!");
		
		a_actor 
			-> GetParentCell() 
			-> PlaceParticleEffect(
				0.0f , modelName , 
				defenderNode -> world.rotate , 
				defenderNode -> worldBound.center , 
				1.0f , 4U , defenderNode.get()
			);
	}
};

namespace inlineUtils
{
	inline bool isPowerAttacking(RE::Actor * actor){

		if(!actor -> currentProcess)
			return false;
			
		if(!actor -> currentProcess -> high)
			return false;
			
		auto data = actor
			-> currentProcess
			-> high
			-> attackData.get();

		return data && data -> data.flags
			.any(RE::AttackData::AttackFlag::kPowerAttack);
	}

	
	inline void restoreAv(RE::Actor * actor,RE::ActorValue actorValue,float value){

		if(value == 0)
			return;
		
		if(!actor)
			return;

		actor 
			-> As<RE::ActorValueOwner>() 
			-> RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage,actorValue,value);
	}


	inline void damageAv(RE::Actor * actor,RE::ActorValue actorValue,float value){

		if(a_val == 0)
			return;

		if(!a_actor)
			return;

		a_actor
			-> As<RE::ActorValueOwner>()
			-> RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage,actorValue,-value);
	}


	typedef void (_fastcall * _shakeCamera)(float strength,RE::NiPoint3 source,float duration);

	inline static REL::Relocation<_shakeCamera> shakeCamera { 
		RELOCATION_ID(32275, 33012)
	};
};
