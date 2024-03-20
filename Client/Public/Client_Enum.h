#ifndef Client_Enum_h__
#define Client_Enum_h__

namespace Engine
{
	enum class MODEL_SHADER { 
		MODEL_ORIGIN,				// 0 - 원래 모델 그래도 Render
		MODEL_NODEOPTH,				// 1 - 깊이기록x -> SkyBox 같은것 용도 
		MODEL_SHADOW,				// 2 - 그림자 그리기 
		MODEL_WHITEBLINK,			// 3 - 흰색으로 깜빡거림 
		MODEL_OUTLINE,				// 4
		MODEL_NONECULL_NONEDSS,		// 5
		MODEL_SHADER_END
	};

	enum class ANIM_SHADER {
		ANIM_ORIGIN,				// 0 - 기본형태
		ANIM_WIREFRAME,				// 1 - 메시로 보기 
		ANIM_SHADOW,				// 2 - 그림자
		ANIM_INTRO_BOSS,			// 3 - INTRO STAGE 보스 
		ANIM_EXAMPLE,				// 4 - Bloom , Rim 사용하는 예시
		ANIM_SHADER_END };

	enum class MONSTER_SHADER {
		COMMON_ORIGIN,				// 0 - 기본 그대로 그리기 
		COMMON_WIREFRAME,			// 1 - 
		COMMON_SHADOW,				// 2 - 그림자 그리기 
		INFECTED_PUNCH,				// 3 - Infected - Attack에서 주먹변하기 
		SNIPER_WEAPON,				// 4 - 스나이퍼 무기 
		MONSTER_SHADER_END };

	enum class UI_SHADER {UI_SHADER_END};

	enum class INSTANCING_SHADER {INSTANCING_SHADER_END};


}

namespace Client
{
#pragma region UI
	enum class UISTATE
	{
		READY,
		ENTER,
		LOOP,
		EXIT,
		STATE_END
	};
	enum class UITYPE
	{
		NONE,
		LEFT_HUD,
		RIGHT_HUD,
		LEVEL_UP,
		REWARD,
		QUESTBOX,
		TUTORIALBOX,
		CROSSHAIR,
		WEAKNESS,
		TYPE_END
	};
	enum class UI_BUTTON
	{
		NONE,

		// OPTION WINDOW 
			TITLE_PLAYER,
			TITLE_GAME,
			TITLE_INDEX_UP,
			TITLE_INDEX_DOWN,
			CLOSE_OPTION_WINDOW,
		// OPTION_WINDOW_END
		
		// GAME_OPTION
			/* SHADER */
			HBAO_PLUS,	// 세기 조절 할수도
			FOG,
			RADIAL_BLUR,
			DOF,
			HDR,
			FXAA,
			HSV,
			SHADOW,
			RIGHT_SHAFT, // 예정
			PBR,		 // 예정
			SSD,		 // 예정
			SSR,		 // 예정
			/* SHADER_END */
			/* SOUND */
			MASTER_SOUND,
			MUSIC_SOUND,
			EFFECT_SOUND,
			/* SOUND_END */
		// GAME_OPTION_END

		// PLAYER_EQUIPMENT
			WEAPON_GAUNTLET,
			WEAPON_REVOLVER,
			WEAPON_RIFLE,
			WEAPON_FLAMETHROWER,
			WEAPON_DYNAMITE,
		// PLAYER_EQUIPMENT_END

		BUTTON_END
	};
#pragma endregion End
}
#endif // Client_Enum_h__
