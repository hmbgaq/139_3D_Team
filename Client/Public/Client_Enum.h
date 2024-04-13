#ifndef Client_Enum_h__
#define Client_Enum_h__

namespace Engine
{
	enum class MODEL_SHADER {
		MODEL_ORIGIN,				// 0 - 원래 모델 그래도 Render
		MODEL_NODEOPTH,				// 1 - 깊이기록x -> SkyBox 같은것 용도 
		MODEL_SHADOW,				// 2 - 그림자 그리기 
		MODEL_CASCADE,				// 3 - CSM 
		MODEL_WHITEBLINK,			// 4 - 흰색으로 깜빡거림 
		MODEL_NONECULL_NONEDSS,		// 5
		MODEL_INTROBOSS_BLOODPOOL,  // 6 - Intro보스맵 바닥렌더용
		MODEL_NORMAL_MAPPING,		// 7 - 노말맵핑 한거 
		MODEL_DIFFUSE,				// 8 - Diffuse 컬러만 외부에서 던지는용도 
		MODEL_ALPHA,				// 9 - AlphaTexture 바인딩 
		MODEL_ICICLE,				// 10 - 고드름 얼음 모델들 렌더용
		MODEL_FOLIAGECLIP,			// 11 - 
		MODEL_OUTLINE_KEEP,			// 12 - OutLine 그대로 유지 
		MODEL_OUTLINE_BLINK,		// 13 - OutLine 깜빡거림
		MODEL_EX_EMISSIVE,			// 14 - Emissive 있음에도 안넣음
		MODEL_MESH_BLOOM ,			// 15 - GLOW효과 그러니까 해당메시전체에 림블룸 주는거 
		MODEL_SHADER_END
	};

	enum class ANIM_SHADER {
		ANIM_ORIGIN,				// 0 - 기본형태
		ANIM_WIREFRAME,				// 1 - 메시로 보기 
		ANIM_SHADOW,				// 2 - 그림자
		ANIM_CASCADE,				// 3 - CSM
		ANIM_INTRO_BOSS,			// 4 - INTRO STAGE 보스 
		ANIM_EXAMPLE,				// 5 - Bloom , Rim 사용하는 예시
		ANIM_SHADER_END };

	enum class MONSTER_SHADER {
		COMMON_ORIGIN,				// 0 - 기본 그대로 그리기 
		COMMON_WIREFRAME,			// 1 - 빈깡통 
		COMMON_SHADOW,				// 2 - 그림자 그리기 
		COMMON_CASCADE,				// 3 - CSM
		COMMON_DISSOLVE,			// 4 - 공통 : 디졸브로 사라지는효과 
		COMMON_MESHCHECK,			// 5 - 공통 : 던지는 컬러로 해당하는 메시의 색을 바꿔서 메시가 몇번인덱스인지 체크하는용도 
		COMMON_OUTLINE_BLINK,		// 6 - 외곽선 깜빡거림 
		COMMON_OUTLINE_KEEP,		// 7 - 외곽선 계속 유지 
		INFECTED_PUNCH,				// 8 - Infected - Attack에서 주먹변하기 
		COMMON_EXCEPT_EMISSIVE,		// 9 - EMISSIVE텍스쳐 없앤버전 
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
		PLAYER_HUD,
		TUTORIAL_BOX,
		DEIDSCREEN,
		STATE_END
	};

	enum class UITYPE
	{
		NONE,
		LEFT_HUD,
		RIGHT_HUD,
		PLAYER_HUD,
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
			HBAO_PLUS,	 // 세기 조절 할수도
			FOG,		
			RADIAL_BLUR, 
			DOF,		
			HDR,
			FXAA,
			HSV,		 // 세기 조절 할수도
			SHADOW,
			LIGHT_SHAFT, // 예정 - 빼야함
			PBR,		 // 예정 - 없는채로 만들기 
			SSR,		 // 예정 - 없는채로 만들기 
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

	enum class TUTORIAL_TEXT // 튜토리얼 텍스트 번호
	{
		START,					//				시작
		CRANE,					//				크레인
		ZOMBIE,					//				좀비
		JENU,					//				제누거인 설명
		ZOMBIE_OVER,			//				좀비무리
		SNIPER,					//				스나이퍼
		SNOW_START,				//				설산시작
		TANK,					//				탱크 설명
		REWARD,					//				습득
		TUTORIALTEXT_END		//				End
	};

	enum class SKILLSTATE // 스킬 상태
	{
		LOCK,					//				
		COOLDOWN,				//
		ACTIVE,					//				
		SKILL_END				//				
	};
#pragma endregion End


	enum class MODE_COLOR // 컬러 합성모드
	{ 
		MUL,		// 곱하기		0
		SCREEN,		// 스크린		1
		OVERLAY,	// 오버레이		2
		ADD,		// 더하기		3 
		BURN,		// 번(Burn)		4

		VIVID_RIGHT,	// 비비드 라이트 5
		SOFT_RIGHT,		// 소프트 라이트 6
		HARD_RIGHT,		// 하드 라이트 7

		COLOR_DODGE,	// 컬러 닷지 8
		MIX_BURN,		// 혼합 번 9

		
		MODE_COLOR_END	//			10
	};


	enum class Additional_Skill
	{
		/* Active */
		KICK,			// 발차기
		UPPER_CUT,		// 어퍼컷
		HERO_PUNCH,		// 영웅펀치 (앞으로 강공격)
		HEAL,			// 회복 스킬
		ELECTRIC_DASH,	// 전기대시
		ELECTROCUTE,	// 대시 후 감전 (합선)
		ELECTRIC_WHIP,	// 전기 끌어당기기
		QUAKE_PUNCH,	// 내려찍기 1
		QUAKE_PUNCH2,	// 내려찍기 2
		QUAKE_PUNCH3,	// 내려찍기 3
		HIT_EIGHT,		// E-콤보 (연속 펀치) [감전상태여야 사용가능]
		SUPER_CHARGE,	// 슈퍼차지 모드
		TELEPORT_PUNCH,	// 텔레포트 펀치

		/* Passive */
		MAX_HP,						// 최대 체력 증가 (10%)
		REGEN_HP,					// 체력 재생 (체력이 서서히 회복되는 효과)
		ELECTRIC_DAMAGE_INCREASE,	// 전기스킬 대미지 증가
		ELECTRIC_COOLTIME_DECREASE,	// 전기스킬 쿨타임 감소
		MELEE_DAMAGE_INCREASE,		// 물리스킬 대미지 증가
		INCREASE_EXP,				// 경험치 획득량 증가
		//MELEE_COOLTIME_DECREASE,	// 물리스킬 쿨타임 감소 => INCREASE_EXP !경험치 획득량 증가로 변경
		HASTE,						// 이동속도 증가

		/* Weapon */
		RIFLE,						// 라이플
		REVOLVER,					// 리볼버
		SHOTGUN,					// 샷건

		/* Weapon Passive */
		RIFLE_SKILL1,				// 라이플 스킬1
		REVOLVER_SKILL1,			// 리볼버 스킬1
		SHOTGUN_SKILL1,				// 샷건 스킬1


		Additional_Skill_End
	};

	enum class Additional_Weapon
	{
		REVOLVER,
		RIFLE,
		SHOTGUN,

		REVOLVER_UPGRADE,
		RIFLE_UPGRADE,
		SHOTGUN_UPGRADE,

		Additional_Weapon_End
	};

}
#endif // Client_Enum_h__
