#pragma once
class Utils
{
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

public:
	inline static void PushActorAway(RE::Actor* causer, RE::Actor* target, float magnitude)
	{
		auto targetPoint = causer->GetNodeByName(causer->race->bodyPartData->parts[0]->targetName.c_str());
		RE::NiPoint3 vec = targetPoint->world.translate;
		//RE::NiPoint3 vec = causer->GetPosition();
		_pushActorAway(causer->currentProcess, target, vec, magnitude);
	}

	

	static inline int soundHelper_a(void* manager, RE::BSSoundHandle* a2, int a3, int a4)  //sub_140BEEE70
	{
		using func_t = decltype(&soundHelper_a);
		REL::Relocation<func_t> func{ REL::ID(66401) };
		return func(manager, a2, a3, a4);
	}

	static inline void soundHelper_b(RE::BSSoundHandle* a1, RE::NiAVObject* source_node)  //sub_140BEDB10
	{
		using func_t = decltype(&soundHelper_b);
		REL::Relocation<func_t> func{ REL::ID(66375) };
		return func(a1, source_node);
	}

	static inline char __fastcall soundHelper_c(RE::BSSoundHandle* a1)  //sub_140BED530
	{
		using func_t = decltype(&soundHelper_c);
		REL::Relocation<func_t> func{ REL::ID(66355) };
		return func(a1);
	}

	static inline char set_sound_position(RE::BSSoundHandle* a1, float x, float y, float z)
	{
		using func_t = decltype(&set_sound_position);
		REL::Relocation<func_t> func{ REL::ID(66370) };
		return func(a1, x, y, z);
	}

	static inline void* BSAudioManager__GetSingleton()
	{
		using func_t = decltype(&BSAudioManager__GetSingleton);
		REL::Relocation<func_t> func{ RE::Offset::BSAudioManager::GetSingleton };
		return func();
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

		auto manager = BSAudioManager__GetSingleton();
		soundHelper_a(manager, &handle, a_descriptor->GetFormID(), 16);
		if (set_sound_position(&handle, a->data.location.x, a->data.location.y, a->data.location.z)) {
			soundHelper_b(&handle, a->Get3D());
			soundHelper_c(&handle);
		}
	}

	static void playSound(RE::Actor* a, std::vector<RE::BGSSoundDescriptorForm*> sounds)
	{
		playSound(a, *select_randomly(sounds.begin(), sounds.end()));
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

	static void triggerStagger(RE::Actor* a_aggressor, RE::Actor* a_reactor, float a_reactionMagnitude)
	{
		auto headingAngle = a_reactor->GetHeadingAngle(a_aggressor->GetPosition(), false);
		auto direction = (headingAngle >= 0.0f) ? headingAngle / 360.0f : (360.0f + headingAngle) / 360.0f;
		a_reactor->SetGraphVariableFloat(staggerDirection, direction);
		a_reactor->SetGraphVariableFloat(StaggerMagnitude, a_reactionMagnitude);
		a_reactor->NotifyAnimationGraph(staggerStart);
	};

	static bool isEquippedShield(RE::Actor* a_actor) {
		auto lhs = a_actor->GetEquippedObject(true);
		return lhs && lhs->IsArmor();
	}
};
