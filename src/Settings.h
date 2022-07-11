#pragma once


static const char * settingsDir = "Data\\SKSE\\Plugins\\EldenParry.ini";


class Settings {
	
	public:

		class facts {
		public:
		
			static inline bool
				isValhallaCombatAPIObtained = false ,
				isPrecisionAPIObtained = false ;
		};

		static inline float 
			fParryTimeWindow_Projectile = 1.0f ,
			fProjectileParryExp = 20.0f ,
			fParryTimeWindow = 0.2f ,
			fMeleeParryExp = 10.0f ;
		
		static inline bool
			bEnableArrowProjectileDeflection = true ,
			bEnableMagicProjectileDeflection = true ,
			bEnableScreenShakeEffect = true ,
			bEnableParrySparkEffect = true ,
			bEnableParrySoundEffect = true , 
			bSuccessfulParryNoCost = true ,
			bEnableSlowTimeEffect = false ,
			bEnableWeaponParry = true ,
			bEnableShieldParry = true ,
			bEnableNPCParry = true ;


		static void readSettings();


	private:

		static bool readSimpleIni(
			CSimpleIniA & settings ,
			const char * path
		){
			if(std::filesystem::exists(path)){
				settings.LoadFile(path);
				return true;
			}
			
			logger::critical("{} is not a valid ini address.",path);
			return false;
		}


		static void ReadIntSetting(
			CSimpleIniA & settings ,
			const char * section ,
			const char * setting,
			uint32_t & value
		){
			const char * present = 
				settings.GetValue(section,setting);
			
			if(present)
				value = static_cast<int>(settings.GetDoubleValue(section,setting));
		}


		static void ReadFloatSetting(
			CSimpleIniA & settings ,
			const char * section ,
			const char * setting ,
			float & value
		){
			const char * present = 
				settings.GetValue(section,setting);
			
			if(present)
				value = static_cast<float>(settings.GetDoubleValue(section,setting));
		}


		static void ReadBoolSetting(
			CSimpleIniA & settings ,
			const char * section , 
			const char * setting ,
			bool & value
		){
			const char * present = 
				settings.GetValue(section,setting);
			
			if(present)
				value = settings.GetBoolValue(section,setting);
		}
};
