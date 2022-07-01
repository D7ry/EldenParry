#include "Settings.h"

void Settings::readSettings() {
	INFO("Reading settings...");
	CSimpleIniA settings;
	readSimpleIni(settings, settingsDir);

	ReadBoolSetting(settings, "General", "bEnableWeaponParry", bEnableWeaponParry);
	ReadBoolSetting(settings, "General", "bEnableShieldParry", bEnableShieldParry);
	ReadBoolSetting(settings, "General", "bEnableNPCParry", bEnableNPCParry);
	ReadBoolSetting(settings, "General", "bSuccessfulParryNoCost", bSuccessfulParryNoCost);

	ReadFloatSetting(settings, "General", "fParryWindow_Start", fParryWindow_Start);
	ReadFloatSetting(settings, "General", "fParryWindow_End", fParryWindow_End);

	ReadBoolSetting(settings, "Effects", "bEnableSlowTimeEffect", bEnableSlowTimeEffect);
	ReadBoolSetting(settings, "Effects", "bEnableScreenShakeEffect", bEnableScreenShakeEffect);
	ReadBoolSetting(settings, "Effects", "bEnableParrySparkEffect", bEnableParrySparkEffect);
	ReadBoolSetting(settings, "Effects", "bEnableParrySoundEffect", bEnableParrySoundEffect);

	ReadBoolSetting(settings, "GuardBash", "bEnableWeaponGuardBash", bEnableWeaponGuardBash);
	ReadBoolSetting(settings, "GuardBash", "bEnableShieldGuardBash", bEnableShieldGuardBash);

	ReadBoolSetting(settings, "ProjectileParry", "bEnableArrowProjectileDeflection", bEnableArrowProjectileDeflection);
	ReadBoolSetting(settings, "ProjectileParry", "bEnableMagicProjectileDeflection", bEnableMagicProjectileDeflection);

	ReadFloatSetting(settings, "Experience", "fProjectileParryExp", fProjectileParryExp);
	ReadFloatSetting(settings, "Experience", "fMeleeParryExp", fMeleeParryExp);

	INFO("done");
}
