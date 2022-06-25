#pragma once
const char* settingsDir = "Data\\SKSE\\Plugins\\EldenParry.ini";
class Settings
{

private:
	static bool readSimpleIni(CSimpleIniA& a_ini, const char* a_iniAddress)
	{
		if (std::filesystem::exists(a_iniAddress)) {
			a_ini.LoadFile(a_iniAddress);
			return true;
		} else {
			logger::critical("{} is not a valid ini address.", a_iniAddress);
			return false;
		}
	}

	static void ReadIntSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, uint32_t& a_setting)
	{
		const char* bFound = nullptr;
		bFound = a_ini.GetValue(a_sectionName, a_settingName);
		if (bFound) {
			a_setting = static_cast<int>(a_ini.GetDoubleValue(a_sectionName, a_settingName));
		}
	}
	static void ReadFloatSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, float& a_setting)
	{
		const char* bFound = nullptr;
		bFound = a_ini.GetValue(a_sectionName, a_settingName);
		if (bFound) {
			a_setting = static_cast<float>(a_ini.GetDoubleValue(a_sectionName, a_settingName));
		}
	}

	static void ReadBoolSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, bool& a_setting)
	{
		const char* bFound = nullptr;
		bFound = a_ini.GetValue(a_sectionName, a_settingName);
		if (bFound) {
			a_setting = a_ini.GetBoolValue(a_sectionName, a_settingName);
		}
	}


	
public:
	class facts
	{
	public:
		static inline bool isPrecisionAPIObtained = false;
	};
	static inline float fParryTimeWindow = 0.2;
	static inline bool bEnableWeaponParry = true;
	static inline bool bEnableShieldParry = true;
	static inline bool bEnableNPCParry = true;
	static inline bool bEnableArrowProjectileDeflection = true;
	static inline bool bEnableMagicProjectileDeflection = true;

	static void readSettings() {
		INFO("Reading settings...");
		CSimpleIniA settings;
		readSimpleIni(settings, settingsDir);
		ReadFloatSetting(settings, "General", "fParryTimeWindow", fParryTimeWindow);
		ReadBoolSetting(settings, "General", "bEnableWeaponParry", bEnableWeaponParry);
		ReadBoolSetting(settings, "General", "bEnableShieldParry", bEnableShieldParry);
		ReadBoolSetting(settings, "General", "bEnableNPCParry", bEnableNPCParry);

		ReadBoolSetting(settings, "ProjectileParry", "bEnableArrowProjectileDeflection", bEnableArrowProjectileDeflection);
		ReadBoolSetting(settings, "ProjectileParry", "bEnableMagicProjectileDeflection", bEnableMagicProjectileDeflection);
		INFO("done");
	}
};
