
#include "Settings.h"


void Settings::readSettings(){

	INFO("Reading settings...");
	
	CSimpleIniA settings;
	readSimpleIni(settings,settingsDir);

	ReadFloatSetting(settings,"General","fParryTimeWindow",fParryTimeWindow);

	ReadBoolSetting(settings,"General","bEnableWeaponParry",bEnableWeaponParry);
	ReadBoolSetting(settings,"General","bEnableShieldParry",bEnableShieldParry);
	ReadBoolSetting(settings,"General","bEnableNPCParry",bEnableNPCParry);
	ReadBoolSetting(settings,"General","bSuccessfulParryNoCost",bSuccessfulParryNoCost);

	ReadBoolSetting(settings,"Effects","bEnableSlowTimeEffect",bEnableSlowTimeEffect);
	ReadBoolSetting(settings,"Effects","bEnableScreenShakeEffect",bEnableScreenShakeEffect);
	ReadBoolSetting(settings,"Effects","bEnableParrySparkEffect",bEnableParrySparkEffect);
	ReadBoolSetting(settings,"Effects","bEnableParrySoundEffect",bEnableParrySoundEffect);

	ReadFloatSetting(settings,"ProjectileParry","fParryTimeWindow_Projectile",fParryTimeWindow_Projectile);
	ReadBoolSetting(settings,"ProjectileParry","bEnableArrowProjectileDeflection",bEnableArrowProjectileDeflection);
	ReadBoolSetting(settings,"ProjectileParry","bEnableMagicProjectileDeflection",bEnableMagicProjectileDeflection);

	ReadFloatSetting(settings,"Experience","fProjectileParryExp",fProjectileParryExp);
	ReadFloatSetting(settings,"Experience","fMeleeParryExp",fMeleeParryExp);

	INFO("done");
}
