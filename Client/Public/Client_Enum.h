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
		ANIM_ORIGIN,			// 0 - 기본형태
		ANIM_WIREFRAME,			// 1 - 메시로 보기 
		ANIM_SHADOW,			// 2 - 그림자
		ANIM_DISSOLVE,			// 3 - 디졸브
		ANIM_DISTORTION,		// 4 - 왜곡
		ANIM_GRAYSCALE,			// 5 - 회색조 : 죽거나 했을때
		ANIM_MASKING,			// 6 - 마스킹텍스쳐로 자르기 
		ANIM_BLOOM,				// 7 - 색 + 뿌옇게 흐리기까지 
		ANIM_SHADER_END };

	enum class UI_SHADER {UI_SHADER_END};

	enum class INSTANCING_SHADER {INSTANCING_SHADER_END};


}

namespace Client
{
#pragma region UI
	enum class UISTATE
	{
		READY,
		APPEAR,
		TICK,
		DISAPPEAR,
		STATE_END
	};
#pragma endregion End
}
#endif // Client_Enum_h__
