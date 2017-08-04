#define MAX_LIGHTS		8
#define POINT_LIGHT		1.0f
#define SPOT_LIGHT		2.0f
#define DIRECTIONAL_LIGHT	3.0f

//#define _WITH_LOCAL_VIEWER_HIGHLIGHTING
//#define _WITH_THETA_PHI_CONES
#define _WITH_REFLECT

//������ ���� ����ü�� �����Ѵ�.
struct MATERIAL
{
	float4 m_cAmbient;	
	float4 m_cDiffuse;		// w = ���İ�
	float4 m_cSpecular; 	// w = ����ŧ�� ��¦�� ���
	float4 m_cEmissive;		// w = �ſ� ȿ�� ����
};

//������ ���� ����ü�� �����Ѵ�.
struct LIGHT
{
	float4 m_cAmbient;
	float4 m_cDiffuse;
	float4 m_cSpecular;	// w = �������� ���� ���� ��
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
// ������ ���� ����ü
struct LIGHT
{
	float4 m_cAmbient;		// dir, point, spot	
	float4 m_cDiffuse;		// dir, point, spot
	float4 m_cSpecular;		// dir, point, spot	; w = �������� ���� ���� ��

	float3 m_vDirection;		// dir
	float  m_fType;			// dir, point, spot

	float3 m_vPosition;		// 	point, spot
	float  m_fRange;		//	point, spot

	float3 m_vAttenuation;		// 	point, spot
	float m_bEnable;		// dir, point, spot
}*/


//������ ���� ������۸� �����Ѵ�. 
cbuffer cbLight : register(b0)
{
	LIGHT gLights[MAX_LIGHTS];
	float4 gcLightGlobalAmbient;
	float4 gvCameraPosition;
};

//������ ���� ������۸� �����Ѵ�. 
cbuffer cbMaterial : register(b1)
{
	MATERIAL gMaterial;
};

// �Ȱ��� ���� ������۸� �����Ѵ�.
cbuffer cbFog : register(b2)
{
	float gFogEnable;
	float gFogStart;
	float gFogRange;
	//float pad0;			// ����
	float4 gFogColor;
};

struct LIGHTEDCOLOR
{
	float4 m_cAmbient;
	float4 m_cDiffuse;
	float4 m_cSpecular;
};

/*���⼺ ������ ȿ���� ����ϴ� �Լ��̴�. ���⼺ ������ ��������� �Ÿ��� ���� ������ ���� ������ �ʴ´�.*/
LIGHTEDCOLOR DirectionalLight(int i, float3 vNormal, float3 vToCamera)
{
	LIGHTEDCOLOR output = (LIGHTEDCOLOR)0;

	float3 vToLight = normalize(-gLights[i].m_vDirection);

	float fDiffuseFactor = dot(vToLight, vNormal);		// Kd
	//������ ������ ���� ���Ϳ� �̷�� ������ ������ �� ���� ������ ������ ����Ѵ�.
	[flatten]
	if (fDiffuseFactor > 0.0f)
	{
		//������ ����ŧ�� �Ŀ��� 0�� �ƴ� ���� ����ŧ�� ������ ������ ����Ѵ�.
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

//�� ������ ȿ���� ����ϴ� �Լ��̴�.
LIGHTEDCOLOR PointLight(int i, float3 vPosition, float3 vNormal, float3 vToCamera)
{
	LIGHTEDCOLOR output = (LIGHTEDCOLOR)0;

	float3 vToLight = gLights[i].m_vPosition - vPosition;
	float fDistance = length(vToLight);
	//��������� �Ÿ��� ������ ��ȿ�Ÿ����� ���� ���� ������ ������ ����Ѵ�.
	if (fDistance <= gLights[i].m_fRange)
	{
		vToLight /= fDistance;		// ����ȭ
		float fDiffuseFactor = dot(vToLight, vNormal);
		//������ ������ ���� ���Ϳ� �̷�� ������ ������ �� ���� ������ ������ ����Ѵ�.
		[flatten]
		if (fDiffuseFactor > 0.0f)
		{
			//������ ����ŧ�� �Ŀ��� 0�� �ƴ� ���� ����ŧ�� ������ ������ ����Ѵ�.
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
		//��������� �Ÿ��� ���� ������ ������ ����Ѵ�.
		float fAttenuationFactor = 1.0f / dot(gLights[i].m_vAttenuation, float3(1.0f, fDistance, fDistance*fDistance));
		output.m_cAmbient = gMaterial.m_cAmbient * gLights[i].m_cAmbient;/*( * fAttenuationFactor);*/
		output.m_cDiffuse *= fAttenuationFactor;
		output.m_cSpecular *= fAttenuationFactor;
	}

	return(output);
}

//���� ������ ȿ���� ����ϴ� �Լ��̴�.
LIGHTEDCOLOR SpotLight(int i, float3 vPosition, float3 vNormal, float3 vToCamera)
{
	LIGHTEDCOLOR output = (LIGHTEDCOLOR)0;
	float3 vToLight = gLights[i].m_vPosition - vPosition;
	float fDistance = length(vToLight);
	//��������� �Ÿ��� ������ ��ȿ�Ÿ����� ���� ���� ������ ������ ����Ѵ�.

	if (fDistance <= gLights[i].m_fRange)
	{
		vToLight /= fDistance;
		float fDiffuseFactor = dot(vToLight, vNormal);
		//������ ������ ���� ���Ϳ� �̷�� ������ ������ �� ���� ������ ������ ����Ѵ�.
		[flatten]
		if (fDiffuseFactor > 0.0f)
		{
			//������ ����ŧ�� �Ŀ��� 0�� �ƴ� ���� ����ŧ�� ������ ������ ����Ѵ�.
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
	float3 vToCamera = normalize(vCameraPosition - vPosition);										// �ü� ���� ����
	LIGHTEDCOLOR LightedColor = (LIGHTEDCOLOR)0;
	float4 cColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// �ϳ��� �ȼ��� ���Ͽ� �ִ� ���� ������ ������ ������ŭ ���� ����� �̷������ �ȴ�. 
	for (i = 0; i < MAX_LIGHTS; i++)
	{
		//Ȱ��ȭ�� ���� ���Ͽ� ������ ������ ����Ѵ�.
		if (gLights[i].m_bEnable == 1.0f)
		{
			//������ ������ ���� ������ ������ ����Ѵ�.
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
	//�۷ι� �ֺ� ������ ������ ���� ���� ���Ѵ�.
	cColor += (gcLightGlobalAmbient * gMaterial.m_cAmbient);

	//���� ������ ���İ��� ������ ��ǻ�� ������ ���İ����� �����Ѵ�.
	cColor.a = gMaterial.m_cDiffuse.a;
	return(cColor);
}

void BreakLighting(float3 vPosition, float3 vNormal, out LIGHTEDCOLOR LightedColor)
{
	int i;
	float3 vCameraPosition = float3(gvCameraPosition.x, gvCameraPosition.y, gvCameraPosition.z);
	float3 vToCamera = normalize(vCameraPosition - vPosition);										// �ü� ���� ����
	//LIGHTEDCOLOR LightedColor = (LIGHTEDCOLOR)0;
	LIGHTEDCOLOR TempColor = (LIGHTEDCOLOR)0;
	float4 cColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// �ϳ��� �ȼ��� ���Ͽ� �ִ� ���� ������ ������ ������ŭ ���� ����� �̷������ �ȴ�. 
	for (i = 0; i < MAX_LIGHTS; i++)
	{
		//Ȱ��ȭ�� ���� ���Ͽ� ������ ������ ����Ѵ�.
		if (gLights[i].m_bEnable == 1.0f)
		{
			//������ ������ ���� ������ ������ ����Ѵ�.
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
	// ���������κ��� ����� ��� ������ ������ [0, 1]�� [-1, 1]�� �����Ѵ�.
	float3 vSNormBasedTex = 2.0f * vUNormBasedTex - 1.0f;

	// �׶� ����Ʈ ������ ���� TBN������ ��������ȭ�Ѵ�.
	float3 N = vAvrVertexNorm;				// ���� ���Ͷ� ����.
	float3 T = normalize(vAvrTBasis - dot(vAvrTBasis, N) * N);
	float3 B = cross(N, T);

	float3x3 TBN = float3x3(T, B, N);

	// ź��Ʈ �������� ���� �������� ����� ��ȯ�Ѵ�.
	float3 vSNormToWorld = mul(vSNormBasedTex, TBN);

	return vSNormToWorld;
}

static const float SMAP_SIZE = 7000.0f ;
static const float SMAP_DX = 1.0f / SMAP_SIZE;

float CalcShadowFactor(SamplerState samShadow, Texture2D shadowMap, float4 shadowPosH)	//shadowPosH : VxPxT���� �Ϸ�� ����
{
	// w�� ����� ������ �Ϸ��Ѵ�. // ���� ������ ���� �ʾƵ� �� ��
	shadowPosH.xyz /= shadowPosH.w;
	
	float4 s;
	float4 result;
	// NDC ���� ������ ���� ��. => ������� ���̰�
	float depth = shadowPosH.z;

	// �ؼ� ũ��.
	const float dx = SMAP_DX;

	float percentLit = 0.0f;
	const float2 offsets[4] =
	{
		float2(-dx, -dx),	float2(0.0f, -dx),	
		float2(-dx, 0.0f),	float2(0.0f, 0.0f)
	};

	// 3x3 ���� ���� ����. �� ǥ������ 4ǥ�� PCF�� �����Ѵ�.
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

	//������� ����� ��ȯ�Ѵ�.
	//return percentLit /= 9.0f;
}





// �ν��Ͻ̿����� ����.
/*���⼺ ������ ȿ���� ����ϴ� �Լ��̴�. ���⼺ ������ ��������� �Ÿ��� ���� ������ ���� ������ �ʴ´�.*/
LIGHTEDCOLOR InstDirectionalLight(int i, float3 vNormal, float3 vToCamera, float4 D, float4 A, float4 S, float4 E)
{
	LIGHTEDCOLOR output = (LIGHTEDCOLOR)0;

	float3 vToLight = normalize(-gLights[i].m_vDirection);

	float fDiffuseFactor = dot(vToLight, vNormal);		// Kd
														//������ ������ ���� ���Ϳ� �̷�� ������ ������ �� ���� ������ ������ ����Ѵ�.
	[flatten]
	if (fDiffuseFactor > 0.0f)
	{
		//������ ����ŧ�� �Ŀ��� 0�� �ƴ� ���� ����ŧ�� ������ ������ ����Ѵ�.
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

//�� ������ ȿ���� ����ϴ� �Լ��̴�.
LIGHTEDCOLOR InstPointLight(int i, float3 vPosition, float3 vNormal, float3 vToCamera, float4 D, float4 A, float4 S, float4 E)
{
	LIGHTEDCOLOR output = (LIGHTEDCOLOR)0;

	float3 vToLight = gLights[i].m_vPosition - vPosition;
	float fDistance = length(vToLight);
	//��������� �Ÿ��� ������ ��ȿ�Ÿ����� ���� ���� ������ ������ ����Ѵ�.
	if (fDistance <= gLights[i].m_fRange)
	{
		vToLight /= fDistance;		// ����ȭ
		float fDiffuseFactor = dot(vToLight, vNormal);
		//������ ������ ���� ���Ϳ� �̷�� ������ ������ �� ���� ������ ������ ����Ѵ�.
		[flatten]
		if (fDiffuseFactor > 0.0f)
		{
			//������ ����ŧ�� �Ŀ��� 0�� �ƴ� ���� ����ŧ�� ������ ������ ����Ѵ�.
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
		//��������� �Ÿ��� ���� ������ ������ ����Ѵ�.
		float fAttenuationFactor = 1.0f / dot(gLights[i].m_vAttenuation, float3(1.0f, fDistance, fDistance*fDistance));
		output.m_cAmbient = A * gLights[i].m_cAmbient;/*( * fAttenuationFactor);*/
		output.m_cDiffuse *= fAttenuationFactor;
		output.m_cSpecular *= fAttenuationFactor;
	}

	return(output);
}

//���� ������ ȿ���� ����ϴ� �Լ��̴�.
LIGHTEDCOLOR InstSpotLight(int i, float3 vPosition, float3 vNormal, float3 vToCamera, float4 D, float4 A, float4 S, float4 E)
{
	LIGHTEDCOLOR output = (LIGHTEDCOLOR)0;
	float3 vToLight = gLights[i].m_vPosition - vPosition;
	float fDistance = length(vToLight);
	//��������� �Ÿ��� ������ ��ȿ�Ÿ����� ���� ���� ������ ������ ����Ѵ�.

	if (fDistance <= gLights[i].m_fRange)
	{
		vToLight /= fDistance;
		float fDiffuseFactor = dot(vToLight, vNormal);
		//������ ������ ���� ���Ϳ� �̷�� ������ ������ �� ���� ������ ������ ����Ѵ�.
		[flatten]
		if (fDiffuseFactor > 0.0f)
		{
			//������ ����ŧ�� �Ŀ��� 0�� �ƴ� ���� ����ŧ�� ������ ������ ����Ѵ�.
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
	float3 vToCamera = normalize(vCameraPosition - vPosition);										// �ü� ���� ����
	LIGHTEDCOLOR LightedColor = (LIGHTEDCOLOR)0;
	float4 cColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// �ϳ��� �ȼ��� ���Ͽ� �ִ� ���� ������ ������ ������ŭ ���� ����� �̷������ �ȴ�. 
	for (i = 0; i < MAX_LIGHTS; i++)
	{
		//Ȱ��ȭ�� ���� ���Ͽ� ������ ������ ����Ѵ�.
		if (gLights[i].m_bEnable == 1.0f)
		{
			//������ ������ ���� ������ ������ ����Ѵ�.
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
	//�۷ι� �ֺ� ������ ������ ���� ���� ���Ѵ�.
	cColor += (gcLightGlobalAmbient * A);

	//���� ������ ���İ��� ������ ��ǻ�� ������ ���İ����� �����Ѵ�.
	cColor.a = D.a;
	return(cColor);
}

void InstBreakLighting(float3 vPosition, float3 vNormal, float4 D, float4 A, float4 S, float4 E, out LIGHTEDCOLOR LightedColor)
{
	int i;
	float3 vCameraPosition = float3(gvCameraPosition.x, gvCameraPosition.y, gvCameraPosition.z);
	float3 vToCamera = normalize(vCameraPosition - vPosition);										// �ü� ���� ����
																									//LIGHTEDCOLOR LightedColor = (LIGHTEDCOLOR)0;
	LIGHTEDCOLOR TempColor = (LIGHTEDCOLOR)0;
	float4 cColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// �ϳ��� �ȼ��� ���Ͽ� �ִ� ���� ������ ������ ������ŭ ���� ����� �̷������ �ȴ�. 
	for (i = 0; i < MAX_LIGHTS; i++)
	{
		//Ȱ��ȭ�� ���� ���Ͽ� ������ ������ ����Ѵ�.
		if (gLights[i].m_bEnable == 1.0f)
		{
			//������ ������ ���� ������ ������ ����Ѵ�.
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