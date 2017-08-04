#define MAX_LIGHTS		8
#define POINT_LIGHT		1.0f
#define SPOT_LIGHT		2.0f
#define DIRECTIONAL_LIGHT	3.0f

//#define _WITH_LOCAL_VIEWER_HIGHLIGHTING
//#define _WITH_THETA_PHI_CONES
#define _WITH_REFLECT

//재질을 위한 구조체를 선언한다.
struct MATERIAL
{
	float4 m_cAmbient;	
	float4 m_cDiffuse;		// w = 알파값
	float4 m_cSpecular; 	// w = 스페큘러 반짝임 계수
	float4 m_cEmissive;		// w = 거울 효과 정도
};

//조명을 위한 구조체를 선언한다.
struct LIGHT
{
	float4 m_cAmbient;
	float4 m_cDiffuse;
	float4 m_cSpecular;	// w = 점적광의 원뿔 내부 각
	float3 m_vPosition;
	float m_fRange;
	float3 m_vDirection;
	float m_nType;
	float3 m_vAttenuation;
	float m_fFalloff;
	float m_fTheta; //cos(m_fTheta)
	float m_fPhi; //cos(m_fPhi)
	float m_bEnable;
	float padding;
};

/*
// 개선된 조명 구조체
struct LIGHT
{
	float4 m_cAmbient;		// dir, point, spot	
	float4 m_cDiffuse;		// dir, point, spot
	float4 m_cSpecular;		// dir, point, spot	; w = 점적광의 원뿔 내부 각

	float3 m_vDirection;		// dir
	float  m_fType;			// dir, point, spot

	float3 m_vPosition;		// 	point, spot
	float  m_fRange;		//	point, spot

	float3 m_vAttenuation;		// 	point, spot
	float m_bEnable;		// dir, point, spot
}*/


//조명을 위한 상수버퍼를 선언한다. 
cbuffer cbLight : register(b0)
{
	LIGHT gLights[MAX_LIGHTS];
	float4 gcLightGlobalAmbient;
	float4 gvCameraPosition;
};

//재질을 위한 상수버퍼를 선언한다. 
cbuffer cbMaterial : register(b1)
{
	MATERIAL gMaterial;
};

// 안개를 위한 상수버퍼를 선언한다.
cbuffer cbFog : register(b2)
{
	float gFogEnable;
	float gFogStart;
	float gFogRange;
	//float pad0;			// 더미
	float4 gFogColor;
};

struct LIGHTEDCOLOR
{
	float4 m_cAmbient;
	float4 m_cDiffuse;
	float4 m_cSpecular;
};

/*방향성 조명의 효과를 계산하는 함수이다. 방향성 조명은 조명까지의 거리에 따라 조명의 양이 변하지 않는다.*/
LIGHTEDCOLOR DirectionalLight(int i, float3 vNormal, float3 vToCamera)
{
	LIGHTEDCOLOR output = (LIGHTEDCOLOR)0;

	float3 vToLight = normalize(-gLights[i].m_vDirection);

	float fDiffuseFactor = dot(vToLight, vNormal);		// Kd
	//조명의 방향이 법선 벡터와 이루는 각도가 예각일 때 직접 조명의 영향을 계산한다.
	[flatten]
	if (fDiffuseFactor > 0.0f)
	{
		//재질의 스펙큘러 파워가 0이 아닐 때만 스펙큘러 조명의 영향을 계산한다.
		if (gMaterial.m_cSpecular.a != 0.0f)
		{
#ifdef _WITH_REFLECT
			float3 vReflect = reflect(-vToLight, vNormal);
			float fSpecularFactor = pow(max(dot(vReflect, vToCamera), 0.0f), gMaterial.m_cSpecular.a);	// Ks
#else
#ifdef _WITH_LOCAL_VIEWER_HIGHLIGHTING
			float3 vHalf = normalize(vToCamera + vToLight);
#else
			float3 vHalf = float3(0.0f, 1.0f, 0.0f);
#endif
			float fSpecularFactor = pow(max(dot(vHalf, vNormal), 0.0f), gMaterial.m_cSpecular.a);
#endif
			output.m_cSpecular = fSpecularFactor * (gMaterial.m_cSpecular * gLights[i].m_cSpecular);
		}
		output.m_cDiffuse = fDiffuseFactor * (gMaterial.m_cDiffuse * gLights[i].m_cDiffuse);
	}
	output.m_cAmbient = gMaterial.m_cAmbient * gLights[i].m_cAmbient;

	return(output);
}

//점 조명의 효과를 계산하는 함수이다.
LIGHTEDCOLOR PointLight(int i, float3 vPosition, float3 vNormal, float3 vToCamera)
{
	LIGHTEDCOLOR output = (LIGHTEDCOLOR)0;

	float3 vToLight = gLights[i].m_vPosition - vPosition;
	float fDistance = length(vToLight);
	//조명까지의 거리가 조명의 유효거리보다 작을 때만 조명의 영향을 계산한다.
	if (fDistance <= gLights[i].m_fRange)
	{
		vToLight /= fDistance;		// 정규화
		float fDiffuseFactor = dot(vToLight, vNormal);
		//조명의 방향이 법선 벡터와 이루는 각도가 예각일 때 직접 조명의 영향을 계산한다.
		[flatten]
		if (fDiffuseFactor > 0.0f)
		{
			//재질의 스펙큘러 파워가 0이 아닐 때만 스펙큘러 조명의 영향을 계산한다.
			if (gMaterial.m_cSpecular.a != 0.0f)
			{
#ifdef _WITH_REFLECT
				float3 vReflect = reflect(-vToLight, vNormal);
				float fSpecularFactor = pow(max(dot(vReflect, vToCamera), 0.0f), gMaterial.m_cSpecular.w);
#else
#ifdef _WITH_LOCAL_VIEWER_HIGHLIGHTING
				float3 vHalf = normalize(vToCamera + vToLight);
#else
				float3 vHalf = float3(0.0f, 1.0f, 0.0f);
#endif
				float fSpecularFactor = pow(max(dot(vHalf, vNormal), 0.0f), gMaterial.m_cSpecular.a);
#endif
				output.m_cSpecular = fSpecularFactor * (gMaterial.m_cSpecular * gLights[i].m_cSpecular);
			}
			output.m_cDiffuse = fDiffuseFactor * (gMaterial.m_cDiffuse * gLights[i].m_cDiffuse);
		}
		//조명까지의 거리에 따라 조명의 영향을 계산한다.
		float fAttenuationFactor = 1.0f / dot(gLights[i].m_vAttenuation, float3(1.0f, fDistance, fDistance*fDistance));
		output.m_cAmbient = gMaterial.m_cAmbient * gLights[i].m_cAmbient;/*( * fAttenuationFactor);*/
		output.m_cDiffuse *= fAttenuationFactor;
		output.m_cSpecular *= fAttenuationFactor;
	}

	return(output);
}

//스팟 조명의 효과를 계산하는 함수이다.
LIGHTEDCOLOR SpotLight(int i, float3 vPosition, float3 vNormal, float3 vToCamera)
{
	LIGHTEDCOLOR output = (LIGHTEDCOLOR)0;
	float3 vToLight = gLights[i].m_vPosition - vPosition;
	float fDistance = length(vToLight);
	//조명까지의 거리가 조명의 유효거리보다 작을 때만 조명의 영향을 계산한다.

	if (fDistance <= gLights[i].m_fRange)
	{
		vToLight /= fDistance;
		float fDiffuseFactor = dot(vToLight, vNormal);
		//조명의 방향이 법선 벡터와 이루는 각도가 예각일 때 직접 조명의 영향을 계산한다.
		[flatten]
		if (fDiffuseFactor > 0.0f)
		{
			//재질의 스펙큘러 파워가 0이 아닐 때만 스펙큘러 조명의 영향을 계산한다.
			if (gMaterial.m_cSpecular.a != 0.0f)
			{
#ifdef _WITH_REFLECT
				float3 vReflect = reflect(-vToLight, vNormal);
				float fSpecularFactor = pow(max(dot(vReflect, vToCamera), 0.0f), gMaterial.m_cSpecular.w);
#else
#ifdef _WITH_LOCAL_VIEWER_HIGHLIGHTING
				float3 vHalf = normalize(vToCamera + vToLight);
#else
				float3 vHalf = float3(0.0f, 1.0f, 0.0f);
#endif
				float fSpecularFactor = pow(max(dot(vHalf, vNormal), 0.0f), gMaterial.m_cSpecular.a);
#endif
				output.m_cSpecular = fSpecularFactor * (gMaterial.m_cSpecular * gLights[i].m_cSpecular);
			}
			output.m_cDiffuse =  fDiffuseFactor * (gMaterial.m_cDiffuse * gLights[i].m_cDiffuse);
		}
#ifdef _WITH_THETA_PHI_CONES
		float fAlpha = max(dot(-vToLight, gLights[i].m_vDirection), 0.0f);
		float fSpotFactor = pow(max(((fAlpha - gLights[i].m_fPhi) / (gLights[i].m_fTheta - gLights[i].m_fPhi)), 0.0f), gLights[i].m_fFalloff);
#else
		float fSpotFactor = pow(max(dot(-vToLight, normalize(gLights[i].m_vDirection)), 0.0f), gLights[i].m_cSpecular.w);
#endif
		float fAttenuationFactor = 1.0f / dot(gLights[i].m_vAttenuation, float3(1.0f, fDistance, fDistance*fDistance));
		output.m_cAmbient = fSpotFactor * (gMaterial.m_cAmbient * gLights[i].m_cAmbient);//* fAttenuationFactor
		output.m_cDiffuse *= (fAttenuationFactor * fSpotFactor);
		output.m_cSpecular *= (fAttenuationFactor * fSpotFactor);
	}

	return(output);
}

float4 Lighting(float3 vPosition, float3 vNormal)
{
	int i;
	float3 vCameraPosition = float3(gvCameraPosition.x, gvCameraPosition.y, gvCameraPosition.z);
	float3 vToCamera = normalize(vCameraPosition - vPosition);										// 시선 벡터 세팅
	LIGHTEDCOLOR LightedColor = (LIGHTEDCOLOR)0;
	float4 cColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// 하나의 픽셀에 대하여 최대 현재 설정된 조명의 갯수만큼 조명 계산이 이루어지게 된다. 
	for (i = 0; i < MAX_LIGHTS; i++)
	{
		//활성화된 조명에 대하여 조명의 영향을 계산한다.
		if (gLights[i].m_bEnable == 1.0f)
		{
			//조명의 유형에 따라 조명의 영향을 계산한다.
			if (gLights[i].m_nType == DIRECTIONAL_LIGHT)
			{
				LightedColor = DirectionalLight(i, vNormal, vToCamera);
				cColor += (LightedColor.m_cAmbient + LightedColor.m_cDiffuse + LightedColor.m_cSpecular);
			}
			if (gLights[i].m_nType == POINT_LIGHT)
			{
				LightedColor = PointLight(i, vPosition, vNormal, vToCamera);
				cColor += (LightedColor.m_cAmbient + LightedColor.m_cDiffuse + LightedColor.m_cSpecular);
			}
			if (gLights[i].m_nType == SPOT_LIGHT)
			{
				LightedColor = SpotLight(i, vPosition, vNormal, vToCamera);
				cColor += (LightedColor.m_cAmbient + LightedColor.m_cDiffuse + LightedColor.m_cSpecular);
			}
		}
	}
	//글로벌 주변 조명의 영향을 최종 색상에 더한다.
	cColor += (gcLightGlobalAmbient * gMaterial.m_cAmbient);

	//최종 색상의 알파값은 재질의 디퓨즈 색상의 알파값으로 설정한다.
	cColor.a = gMaterial.m_cDiffuse.a;
	return(cColor);
}

void BreakLighting(float3 vPosition, float3 vNormal, out LIGHTEDCOLOR LightedColor)
{
	int i;
	float3 vCameraPosition = float3(gvCameraPosition.x, gvCameraPosition.y, gvCameraPosition.z);
	float3 vToCamera = normalize(vCameraPosition - vPosition);										// 시선 벡터 세팅
	//LIGHTEDCOLOR LightedColor = (LIGHTEDCOLOR)0;
	LIGHTEDCOLOR TempColor = (LIGHTEDCOLOR)0;
	float4 cColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// 하나의 픽셀에 대하여 최대 현재 설정된 조명의 갯수만큼 조명 계산이 이루어지게 된다. 
	for (i = 0; i < MAX_LIGHTS; i++)
	{
		//활성화된 조명에 대하여 조명의 영향을 계산한다.
		if (gLights[i].m_bEnable == 1.0f)
		{
			//조명의 유형에 따라 조명의 영향을 계산한다.
			if (gLights[i].m_nType == DIRECTIONAL_LIGHT)
			{
				TempColor = DirectionalLight(i, vNormal, vToCamera);
				LightedColor.m_cAmbient += TempColor.m_cAmbient;
				LightedColor.m_cDiffuse += TempColor.m_cDiffuse;
				LightedColor.m_cSpecular += TempColor.m_cSpecular;
			}
			if (gLights[i].m_nType == POINT_LIGHT)
			{
				TempColor = PointLight(i, vPosition, vNormal, vToCamera);
				LightedColor.m_cAmbient += TempColor.m_cAmbient;
				LightedColor.m_cDiffuse += TempColor.m_cDiffuse;
				LightedColor.m_cSpecular += TempColor.m_cSpecular;
			}
			if (gLights[i].m_nType == SPOT_LIGHT)
			{
				TempColor = SpotLight(i, vPosition, vNormal, vToCamera);
				LightedColor.m_cAmbient += TempColor.m_cAmbient;
				LightedColor.m_cDiffuse += TempColor.m_cDiffuse;
				LightedColor.m_cSpecular += TempColor.m_cSpecular;
			}
		}
	}
}

float3 ConvertNormalBasedOnTBN(float3 vUNormBasedTex, float3 vAvrVertexNorm, float3 vAvrTBasis)
{
	// 법선맵으로부터 추출된 노멀 벡터의 범위값 [0, 1]을 [-1, 1]로 변경한다.
	float3 vSNormBasedTex = 2.0f * vUNormBasedTex - 1.0f;

	// 그람 슈미트 과정을 통해 TBN기저를 정규직교화한다.
	float3 N = vAvrVertexNorm;				// 단위 벡터라 가정.
	float3 T = normalize(vAvrTBasis - dot(vAvrTBasis, N) * N);
	float3 B = cross(N, T);

	float3x3 TBN = float3x3(T, B, N);

	// 탄젠트 공간에서 세계 공간으로 노멀을 변환한다.
	float3 vSNormToWorld = mul(vSNormBasedTex, TBN);

	return vSNormToWorld;
}

static const float SMAP_SIZE = 7000.0f ;
static const float SMAP_DX = 1.0f / SMAP_SIZE;

float CalcShadowFactor(SamplerState samShadow, Texture2D shadowMap, float4 shadowPosH)	//shadowPosH : VxPxT까지 완료된 상태
{
	// w로 나누어서 투영을 완료한다. // 직교 투영은 하지 않아도 될 것
	shadowPosH.xyz /= shadowPosH.w;
	
	float4 s;
	float4 result;
	// NDC 공간 기준의 깊이 값. => 상대적인 깊이값
	float depth = shadowPosH.z;

	// 텍셀 크기.
	const float dx = SMAP_DX;

	float percentLit = 0.0f;
	const float2 offsets[4] =
	{
		float2(-dx, -dx),	float2(0.0f, -dx),	
		float2(-dx, 0.0f),	float2(0.0f, 0.0f)
	};

	// 3x3 상자 필터 패턴. 각 표본마다 4표본 PCF를 수행한다.
	[unroll]
	for (int i = 0; i < 4; ++i)
	{
		s[i] = shadowMap.Sample(samShadow, shadowPosH.xy + offsets[i]).r;
		result[i] = depth < s[i];
		//percentLit += shadowMap.SampleCmp(samShadow, shadowPosH.xy + offsets[i], depth).r;
	}
	float2 t = frac(SMAP_SIZE*shadowPosH.xy);
	
	return lerp(lerp(result[0], result[1], t.x),
		lerp(result[2], result[3], t.x), t.y);

	//결과들의 평균을 반환한다.
	//return percentLit /= 9.0f;
}





// 인스턴싱용으로 개조.
/*방향성 조명의 효과를 계산하는 함수이다. 방향성 조명은 조명까지의 거리에 따라 조명의 양이 변하지 않는다.*/
LIGHTEDCOLOR InstDirectionalLight(int i, float3 vNormal, float3 vToCamera, float4 D, float4 A, float4 S, float4 E)
{
	LIGHTEDCOLOR output = (LIGHTEDCOLOR)0;

	float3 vToLight = normalize(-gLights[i].m_vDirection);

	float fDiffuseFactor = dot(vToLight, vNormal);		// Kd
														//조명의 방향이 법선 벡터와 이루는 각도가 예각일 때 직접 조명의 영향을 계산한다.
	[flatten]
	if (fDiffuseFactor > 0.0f)
	{
		//재질의 스펙큘러 파워가 0이 아닐 때만 스펙큘러 조명의 영향을 계산한다.
		if (S.a != 0.0f)
		{
#ifdef _WITH_REFLECT
			float3 vReflect = reflect(-vToLight, vNormal);
			float fSpecularFactor = pow(max(dot(vReflect, vToCamera), 0.0f), S.a);	// Ks
#else
#ifdef _WITH_LOCAL_VIEWER_HIGHLIGHTING
			float3 vHalf = normalize(vToCamera + vToLight);
#else
			float3 vHalf = float3(0.0f, 1.0f, 0.0f);
#endif
			float fSpecularFactor = pow(max(dot(vHalf, vNormal), 0.0f), S.a);
#endif
			output.m_cSpecular = fSpecularFactor * (S * gLights[i].m_cSpecular);
		}
		output.m_cDiffuse = fDiffuseFactor * (D * gLights[i].m_cDiffuse);
	}
	output.m_cAmbient = A * gLights[i].m_cAmbient;

	return(output);
}

//점 조명의 효과를 계산하는 함수이다.
LIGHTEDCOLOR InstPointLight(int i, float3 vPosition, float3 vNormal, float3 vToCamera, float4 D, float4 A, float4 S, float4 E)
{
	LIGHTEDCOLOR output = (LIGHTEDCOLOR)0;

	float3 vToLight = gLights[i].m_vPosition - vPosition;
	float fDistance = length(vToLight);
	//조명까지의 거리가 조명의 유효거리보다 작을 때만 조명의 영향을 계산한다.
	if (fDistance <= gLights[i].m_fRange)
	{
		vToLight /= fDistance;		// 정규화
		float fDiffuseFactor = dot(vToLight, vNormal);
		//조명의 방향이 법선 벡터와 이루는 각도가 예각일 때 직접 조명의 영향을 계산한다.
		[flatten]
		if (fDiffuseFactor > 0.0f)
		{
			//재질의 스펙큘러 파워가 0이 아닐 때만 스펙큘러 조명의 영향을 계산한다.
			if (S.a != 0.0f)
			{
#ifdef _WITH_REFLECT
				float3 vReflect = reflect(-vToLight, vNormal);
				float fSpecularFactor = pow(max(dot(vReflect, vToCamera), 0.0f), S.w);
#else
#ifdef _WITH_LOCAL_VIEWER_HIGHLIGHTING
				float3 vHalf = normalize(vToCamera + vToLight);
#else
				float3 vHalf = float3(0.0f, 1.0f, 0.0f);
#endif
				float fSpecularFactor = pow(max(dot(vHalf, vNormal), 0.0f), S.a);
#endif
				output.m_cSpecular = fSpecularFactor * (S * gLights[i].m_cSpecular);
			}
			output.m_cDiffuse = fDiffuseFactor * (D * gLights[i].m_cDiffuse);
		}
		//조명까지의 거리에 따라 조명의 영향을 계산한다.
		float fAttenuationFactor = 1.0f / dot(gLights[i].m_vAttenuation, float3(1.0f, fDistance, fDistance*fDistance));
		output.m_cAmbient = A * gLights[i].m_cAmbient;/*( * fAttenuationFactor);*/
		output.m_cDiffuse *= fAttenuationFactor;
		output.m_cSpecular *= fAttenuationFactor;
	}

	return(output);
}

//스팟 조명의 효과를 계산하는 함수이다.
LIGHTEDCOLOR InstSpotLight(int i, float3 vPosition, float3 vNormal, float3 vToCamera, float4 D, float4 A, float4 S, float4 E)
{
	LIGHTEDCOLOR output = (LIGHTEDCOLOR)0;
	float3 vToLight = gLights[i].m_vPosition - vPosition;
	float fDistance = length(vToLight);
	//조명까지의 거리가 조명의 유효거리보다 작을 때만 조명의 영향을 계산한다.

	if (fDistance <= gLights[i].m_fRange)
	{
		vToLight /= fDistance;
		float fDiffuseFactor = dot(vToLight, vNormal);
		//조명의 방향이 법선 벡터와 이루는 각도가 예각일 때 직접 조명의 영향을 계산한다.
		[flatten]
		if (fDiffuseFactor > 0.0f)
		{
			//재질의 스펙큘러 파워가 0이 아닐 때만 스펙큘러 조명의 영향을 계산한다.
			if (S.a != 0.0f)
			{
#ifdef _WITH_REFLECT
				float3 vReflect = reflect(-vToLight, vNormal);
				float fSpecularFactor = pow(max(dot(vReflect, vToCamera), 0.0f), S.w);
#else
#ifdef _WITH_LOCAL_VIEWER_HIGHLIGHTING
				float3 vHalf = normalize(vToCamera + vToLight);
#else
				float3 vHalf = float3(0.0f, 1.0f, 0.0f);
#endif
				float fSpecularFactor = pow(max(dot(vHalf, vNormal), 0.0f), S.a);
#endif
				output.m_cSpecular = fSpecularFactor * (S * gLights[i].m_cSpecular);
			}
			output.m_cDiffuse = fDiffuseFactor * (D * gLights[i].m_cDiffuse);
		}
#ifdef _WITH_THETA_PHI_CONES
		float fAlpha = max(dot(-vToLight, gLights[i].m_vDirection), 0.0f);
		float fSpotFactor = pow(max(((fAlpha - gLights[i].m_fPhi) / (gLights[i].m_fTheta - gLights[i].m_fPhi)), 0.0f), gLights[i].m_fFalloff);
#else
		float fSpotFactor = pow(max(dot(-vToLight, normalize(gLights[i].m_vDirection)), 0.0f), gLights[i].m_cSpecular.w);
#endif
		float fAttenuationFactor = 1.0f / dot(gLights[i].m_vAttenuation, float3(1.0f, fDistance, fDistance*fDistance));
		output.m_cAmbient = fSpotFactor * (A * gLights[i].m_cAmbient);//* fAttenuationFactor
		output.m_cDiffuse *= (fAttenuationFactor * fSpotFactor);
		output.m_cSpecular *= (fAttenuationFactor * fSpotFactor);
	}

	return(output);
}

float4 InstLighting(float3 vPosition, float3 vNormal, float4 D, float4 A, float4 S, float4 E)
{
	int i;
	float3 vCameraPosition = float3(gvCameraPosition.x, gvCameraPosition.y, gvCameraPosition.z);
	float3 vToCamera = normalize(vCameraPosition - vPosition);										// 시선 벡터 세팅
	LIGHTEDCOLOR LightedColor = (LIGHTEDCOLOR)0;
	float4 cColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// 하나의 픽셀에 대하여 최대 현재 설정된 조명의 갯수만큼 조명 계산이 이루어지게 된다. 
	for (i = 0; i < MAX_LIGHTS; i++)
	{
		//활성화된 조명에 대하여 조명의 영향을 계산한다.
		if (gLights[i].m_bEnable == 1.0f)
		{
			//조명의 유형에 따라 조명의 영향을 계산한다.
			if (gLights[i].m_nType == DIRECTIONAL_LIGHT)
			{
				LightedColor = InstDirectionalLight(i, vNormal, vToCamera, D, A, S, E);
				cColor += (LightedColor.m_cAmbient + LightedColor.m_cDiffuse + LightedColor.m_cSpecular);
			}
			if (gLights[i].m_nType == POINT_LIGHT)
			{
				LightedColor = InstPointLight(i, vPosition, vNormal, vToCamera, D, A, S, E);
				cColor += (LightedColor.m_cAmbient + LightedColor.m_cDiffuse + LightedColor.m_cSpecular);
			}
			if (gLights[i].m_nType == SPOT_LIGHT)
			{
				LightedColor = InstSpotLight(i, vPosition, vNormal, vToCamera, D, A, S, E);
				cColor += (LightedColor.m_cAmbient + LightedColor.m_cDiffuse + LightedColor.m_cSpecular);
			}
		}
	}
	//글로벌 주변 조명의 영향을 최종 색상에 더한다.
	cColor += (gcLightGlobalAmbient * A);

	//최종 색상의 알파값은 재질의 디퓨즈 색상의 알파값으로 설정한다.
	cColor.a = D.a;
	return(cColor);
}

void InstBreakLighting(float3 vPosition, float3 vNormal, float4 D, float4 A, float4 S, float4 E, out LIGHTEDCOLOR LightedColor)
{
	int i;
	float3 vCameraPosition = float3(gvCameraPosition.x, gvCameraPosition.y, gvCameraPosition.z);
	float3 vToCamera = normalize(vCameraPosition - vPosition);										// 시선 벡터 세팅
																									//LIGHTEDCOLOR LightedColor = (LIGHTEDCOLOR)0;
	LIGHTEDCOLOR TempColor = (LIGHTEDCOLOR)0;
	float4 cColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// 하나의 픽셀에 대하여 최대 현재 설정된 조명의 갯수만큼 조명 계산이 이루어지게 된다. 
	for (i = 0; i < MAX_LIGHTS; i++)
	{
		//활성화된 조명에 대하여 조명의 영향을 계산한다.
		if (gLights[i].m_bEnable == 1.0f)
		{
			//조명의 유형에 따라 조명의 영향을 계산한다.
			if (gLights[i].m_nType == DIRECTIONAL_LIGHT)
			{
				TempColor = InstDirectionalLight(i, vNormal, vToCamera, D, A, S, E);
				LightedColor.m_cAmbient += TempColor.m_cAmbient;
				LightedColor.m_cDiffuse += TempColor.m_cDiffuse;
				LightedColor.m_cSpecular += TempColor.m_cSpecular;
			}
			if (gLights[i].m_nType == POINT_LIGHT)
			{
				TempColor = InstPointLight(i, vPosition, vNormal, vToCamera, D, A, S, E);
				LightedColor.m_cAmbient += TempColor.m_cAmbient;
				LightedColor.m_cDiffuse += TempColor.m_cDiffuse;
				LightedColor.m_cSpecular += TempColor.m_cSpecular;
			}
			if (gLights[i].m_nType == SPOT_LIGHT)
			{
				TempColor = InstSpotLight(i, vPosition, vNormal, vToCamera, D, A, S, E);
				LightedColor.m_cAmbient += TempColor.m_cAmbient;
				LightedColor.m_cDiffuse += TempColor.m_cDiffuse;
				LightedColor.m_cSpecular += TempColor.m_cSpecular;
			}
		}
	}
}