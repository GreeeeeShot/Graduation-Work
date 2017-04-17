#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <algorithm>

#ifndef FBXSDK_NEW_API
#define FBXSDK_NEW_API
#endif

#include <fbxsdk.h>
#include <Windows.h>

typedef std::tr1::unordered_map<std::string, int> UVsetID;
typedef std::tr1::unordered_map<std::string, std::vector<std::string>> TextureSet;

void ReadVertexCacheData(FbxMesh* pMesh,
	FbxTime& pTime,
	FbxVector4* pVertexArray);

struct BlendingIndexWeightPair
{
	unsigned int mBlendingIndex;
	double mBlendingWeight;

	BlendingIndexWeightPair() :
		mBlendingIndex(0),
		mBlendingWeight(0)
	{}
};

// Each Control Point in FBX is basically a vertex
// in the physical world. For example, a cube has 8
// vertices(Control Points) in FBX
// Joints are associated with Control Points in FBX
// The mapping is one joint corresponding to 4
// Control Points(Reverse of what is done in a game engine)
// As a result, this struct stores a XMFLOAT3 and a 
// vector of joint indices
struct CtrlPoint
{
	FbxVector4 mPosition;
	std::vector<BlendingIndexWeightPair> mBlendingInfo;

	CtrlPoint()
	{
		mBlendingInfo.reserve(4);
	}
};

// This stores the information of each key frame of each joint
// This is a linked list and each node is a snapshot of the
// global transformation of the joint at a certain frame
struct Keyframe
{
	FbxLongLong mFrameNum;
	FbxAMatrix mGlobalTransform;
	Keyframe* mNext;

	Keyframe() :
		mNext(nullptr)
	{}
};

// This is the actual representation of a joint in a game engine
struct Joint
{
	std::string mName;
	int mParentIndex;
	FbxAMatrix mGlobalBindposeInverse;
	Keyframe* mAnimation;
	FbxNode* mNode;

	Joint() :
		mNode(nullptr),
		mAnimation(nullptr)
	{
		mGlobalBindposeInverse.SetIdentity();
		mParentIndex = -1;
	}

	~Joint()
	{
		while (mAnimation)
		{
			Keyframe* temp = mAnimation->mNext;
			delete mAnimation;
			mAnimation = temp;
		}
	}
};

struct Skeleton
{
	std::vector<Joint> mJoints;
};


namespace FBX_LOADER
{

	struct FBX_MATRIAL_ELEMENT
	{
		enum MATERIAL_ELEMENT_TYPE
		{
			ELEMENT_NONE = 0,
			ELEMENT_COLOR,
			ELEMENT_TEXTURE,
			ELEMENT_BOTH,
			ELEMENT_MAX,
		};

		MATERIAL_ELEMENT_TYPE type;
		float r, g, b, a;
		TextureSet			textureSetArray;

		FBX_MATRIAL_ELEMENT()
		{
			textureSetArray.clear();
		}

		~FBX_MATRIAL_ELEMENT()
		{
			Release();
		}

		void Release()
		{
			for (TextureSet::iterator it = textureSetArray.begin(); it != textureSetArray.end(); ++it)
			{
				it->second.clear();
			}

			textureSetArray.clear();
		}
	};

	struct FBX_MATERIAL_NODE
	{
		enum eMATERIAL_TYPE
		{
			MATERIAL_LAMBERT = 0,
			MATERIAL_PHONG,
		};

		eMATERIAL_TYPE type;
		FBX_MATRIAL_ELEMENT ambient;
		FBX_MATRIAL_ELEMENT diffuse;
		FBX_MATRIAL_ELEMENT emmisive;
		FBX_MATRIAL_ELEMENT specular;

		float shininess;
		float TransparencyFactor;		
	};

	
	struct MESH_ELEMENTS
	{
		unsigned int	numPosition;		
		unsigned int	numNormal;			
		unsigned int	numUVSet;			
	};

	
	struct FBX_MESH_NODE
	{
		std::string		name;			
		std::string		parentName;		

		MESH_ELEMENTS	elements;		
		std::vector<FBX_MATERIAL_NODE> m_materialArray;		
		UVsetID		uvsetID;

		std::vector<unsigned int>		indexArray;
		std::vector<unsigned int>		MeshindexArray;
		std::vector<FbxVector4>			m_positionArray;		
		std::vector<FbxVector4>			m_normalArray;			
		std::vector<FbxVector2>			m_texcoordArray;		

		float	mat4x4[16];	// Matrix

		~FBX_MESH_NODE()
		{
			Release();
		}

		void Release()
		{
			uvsetID.clear();
			m_texcoordArray.clear();
			m_materialArray.clear();
			indexArray.clear();
			m_positionArray.clear();
			m_normalArray.clear();
		}
	};

	struct BoneMove {
		FbxTime time;
		FbxMap<int,FbxAMatrix> moving;
	};

	struct BlendingSkin {
		int num;
		FbxMap<int,float> blending;
	};

	class CFBXLoader
	{
	public:
		enum eAXIS_SYSTEM
		{
			eAXIS_DIRECTX = 0,
			eAXIS_OPENGL,
		};
		enum Status
		{
			UNLOADED,               // Unload file or load failure;
			MUST_BE_LOADED,         // Ready for loading file;
			MUST_BE_REFRESHED,      // Something changed and redraw needed;
			REFRESHED               // No redraw needed.
		};

	protected:
		FbxManager* mSdkManager;
		FbxScene*	mScene;
		FbxImporter * mImporter;
		FbxAnimLayer * mCurrentAnimLayer;

		FbxArray<FbxString*> mAnimStackNameArray;
		FbxArray<FbxPose*> mPoseArray;
		std::vector<BoneMove> mMovingBoneArray;
		std::vector<BlendingSkin> mBlendingSkinArray;
		std::vector<FbxAMatrix> mBindBoneMtx;
		std::vector<D3DXVECTOR3> mControlPoints;
		Skeleton mSkeleton;

		mutable FbxTime mFrameTime, mStart, mStop, mCurrentTime;
		mutable FbxTime mCache_Start, mCache_Stop;
		mutable Status mStatus;

		std::vector<FBX_MESH_NODE>		m_meshNodeArray;

		void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene);
		void TriangulateRecursive(FbxNode* pNode);

		void SetupNode(FbxNode* pNode, std::string parentName);
		void Setup();

		void CopyVertexData(FbxMesh*	pMesh, FBX_MESH_NODE* meshNode);
		void CopyMatrialData(FbxSurfaceMaterial* mat, FBX_MATERIAL_NODE* destMat);

		void ComputeNodeMatrix(FbxNode* pNode, FBX_MESH_NODE* meshNode);

		void SetFbxColor(FBX_MATRIAL_ELEMENT& destColor, const FbxDouble3 srcColor);
		FbxDouble3 GetMaterialProperty(
			const FbxSurfaceMaterial * pMaterial,
			const char * pPropertyName,
			const char * pFactorPropertyName,
			FBX_MATRIAL_ELEMENT*			pElement);

		static void FBXMatrixToFloat16(FbxMatrix* src, float dest[16])
		{
			unsigned int nn = 0;
			for (int i = 0; i<4; i++)
			{
				for (int j = 0; j<4; j++)
				{
					dest[nn] = static_cast<float>(src->Get(i, j));
					nn++;
				}
			}
		}

	public:
		CFBXLoader();
		~CFBXLoader();

		void Release();

		HRESULT LoadFBX(const char* filename, const eAXIS_SYSTEM axis);
		FbxNode&	GetRootNode();

		size_t GetNodesCount() { return m_meshNodeArray.size(); };		// ƒm?ƒh”‚ÌŽæ“¾

		FBX_MESH_NODE&	GetNode(const unsigned int id);
		const FbxMesh& getMesh(FbxNode* pNode) {
			FbxMesh* IMesh = pNode->GetMesh();
			return *IMesh;
		}
		void LoadNode(FbxNode* fbxNode);
		void LoadMesh(FbxMesh *fbxMesh);
		void LoadNodeKeyframeAnimation(FbxNode* fbxNode);
		void LoadMesh_Skeleton(FbxMesh *fbxMesh);
		bool SetCurrentAnimStack(int pIndex);
		void ReadVertexCacheData(FbxMesh* pMesh, FbxTime& pTime, FbxVector4* pVertexArray);
		void AnimationTimePerData();
		void onTime();
		int GetnVex() { return mControlPoints.size(); }
		void PreparePointCacheData(FbxScene* pScene, FbxTime &pCache_Start, FbxTime &pCache_Stop);
		void FillPoseArray(FbxScene* pScene, FbxArray<FbxPose*>& pPoseArray);
		D3DXVECTOR3 GetControlPoint(int id) { return mControlPoints[id]; }
		FbxAMatrix GetGeometry(FbxNode* pNode);
		unsigned int FindJointIndexUsingName(const std::string& inJointName);
	};

}