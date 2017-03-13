#include "FBXLoader.h"

namespace FBX_LOADER
{

	CFBXLoader::CFBXLoader()
	{
		mSdkManager = nullptr;
		mScene = nullptr;
	}

	CFBXLoader::~CFBXLoader()
	{
		Release();
	}

	void CFBXLoader::Release()
	{
		m_meshNodeArray.clear();

		if (mImporter)
		{
			mImporter->Destroy();
			mImporter = nullptr;
		}

		if (mScene)
		{
			mScene->Destroy();
			mScene = nullptr;
		}

		if (mSdkManager)
		{
			mSdkManager->Destroy();
			mSdkManager = nullptr;
		}
	}

	HRESULT CFBXLoader::LoadFBX(const char* filename, const eAXIS_SYSTEM axis)
	{
		if (!filename)
			return E_FAIL;

		HRESULT hr = S_OK;
		// mSdkManager mScene 초기화
		InitializeSdkObjects(mSdkManager, mScene);
		if (!mSdkManager)
			return E_FAIL;

		int lFileFormat = -1;

		//Importer를 만든다
		mImporter = FbxImporter::Create(mSdkManager, "");

		//파일 포맷 설정
		if (!mSdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(filename, lFileFormat))
		{
			// Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
			lFileFormat = mSdkManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");;
		}

		// Initialize the importer by providing a filename. 파일이름과 포맷의 fbx파일을 불러와서 초기화
		if (!mImporter || mImporter->Initialize(filename, lFileFormat) == false)
			return E_FAIL;

		if (!mImporter || mImporter->Import(mScene) == false)
			return E_FAIL;

		//DirectX로 설정
		FbxAxisSystem OurAxisSystem = FbxAxisSystem::DirectX;

		if (axis == eAXIS_OPENGL)
			OurAxisSystem = FbxAxisSystem::OpenGL;

		
		FbxAxisSystem SceneAxisSystem = mScene->GetGlobalSettings().GetAxisSystem();
		if (SceneAxisSystem != OurAxisSystem)
		{
			FbxAxisSystem::DirectX.ConvertScene(mScene);
		}
		FbxSystemUnit SceneSystemUnit = mScene->GetGlobalSettings().GetSystemUnit();
		if (SceneSystemUnit.GetScaleFactor() != 1.0)
		{
			FbxSystemUnit::cm.ConvertScene(mScene);
		}

		TriangulateRecursive(mScene->GetRootNode());

		Setup();

		return hr;
	}

	// FBXLoader초기화
	void CFBXLoader::InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene)
	{
		//FbxManager를 만든다
		pManager = FbxManager::Create();
		if (!pManager)
		{
			FBXSDK_printf("Error: Unable to create FBX Manager!\n");
			exit(1);
		}
		else FBXSDK_printf("Autodesk FBX SDK version %s\n", pManager->GetVersion());

		//FbxIOSetting 즉 Import/Export를 담당하는 녀석을 만들어낸다.
		FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
		pManager->SetIOSettings(ios);

		//Load plugins from the executable directory (optional) 실행가능한 디렉터리에서 플러그인을 가져온다(선택사항)
		FbxString lPath = FbxGetApplicationDirectory();
		pManager->LoadPluginsDirectory(lPath.Buffer());

		//FBX Scene을 만든다.
		pScene = FbxScene::Create(pManager, "My Scene");
		if (!pScene)
		{
			FBXSDK_printf("Error: Unable to create FBX scene!\n");
			exit(1);
		}
	}
	//삼각형화 시킬수 있는 모든 node를 삼각형으로 만드는것. 자식노드로 내려가는 재귀함수
	void CFBXLoader::TriangulateRecursive(FbxNode* pNode)
	{
		FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

		if (lNodeAttribute)
		{
			if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh ||
				lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbs ||
				lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbsSurface ||
				lNodeAttribute->GetAttributeType() == FbxNodeAttribute::ePatch)
			{
				FbxGeometryConverter lConverter(pNode->GetFbxManager());
				
#if 0
				lConverter.TriangulateInPlace(pNode);
#endif 
				lConverter.Triangulate(mScene, true);
			}
		}

		const int lChildCount = pNode->GetChildCount();
		for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
		{
			TriangulateRecursive(pNode->GetChild(lChildIndex));
		}
	}

	//루트노드를 가져옴
	FbxNode&	CFBXLoader::GetRootNode()
	{
		return *mScene->GetRootNode();
	}
	//루트노드를 세팅
	void CFBXLoader::Setup()
	{
		if (mScene->GetRootNode())
		{
			SetupNode(mScene->GetRootNode(), "null");
		}
	}
	//노드에 있는 버텍스 정보를 행렬로 만든다. 그리고 메쉬노드행렬에 저장
	void CFBXLoader::SetupNode(FbxNode* pNode, std::string parentName)
	{
		if (!pNode)
			return;

		FBX_MESH_NODE meshNode;

		meshNode.name = pNode->GetName();
		meshNode.parentName = parentName;

		ZeroMemory(&meshNode.elements, sizeof(MESH_ELEMENTS));
		//노드에서 메쉬를 갖고옴
		FbxMesh* lMesh = pNode->GetMesh();

		if (lMesh)
		{
			const int lVertexCount = lMesh->GetControlPointsCount();//메쉬의 컨트롤포인트 갯수를 가져옴

			if (lVertexCount>0)
			{
				CopyVertexData(lMesh, &meshNode);
			}
		}

		const int lMaterialCount = pNode->GetMaterialCount();
		for (int i = 0; i<lMaterialCount; i++)
		{
			FbxSurfaceMaterial* mat = pNode->GetMaterial(i);
			if (!mat)
				continue;

			FBX_MATERIAL_NODE destMat;
			CopyMatrialData(mat, &destMat);

			meshNode.m_materialArray.push_back(destMat);
		}

		ComputeNodeMatrix(pNode, &meshNode);

		m_meshNodeArray.push_back(meshNode);

		const int lCount = pNode->GetChildCount();
		for (int i = 0; i < lCount; i++)
		{
			SetupNode(pNode->GetChild(i), meshNode.name);
		}
	}

	void CFBXLoader::SetFbxColor(FBX_MATRIAL_ELEMENT& destColor, const FbxDouble3 srcColor)
	{
		destColor.a = 1.0f;
		destColor.r = static_cast<float>(srcColor[0]);
		destColor.g = static_cast<float>(srcColor[1]);
		destColor.b = static_cast<float>(srcColor[2]);
	}

	FbxDouble3 CFBXLoader::GetMaterialProperty(
		const FbxSurfaceMaterial * pMaterial,
		const char * pPropertyName,
		const char * pFactorPropertyName,
		FBX_MATRIAL_ELEMENT*			pElement)
	{
		pElement->type = FBX_MATRIAL_ELEMENT::ELEMENT_NONE;

		FbxDouble3 lResult(0, 0, 0);
		const FbxProperty lProperty = pMaterial->FindProperty(pPropertyName);
		const FbxProperty lFactorProperty = pMaterial->FindProperty(pFactorPropertyName);
		if (lProperty.IsValid() && lFactorProperty.IsValid())
		{
			lResult = lProperty.Get<FbxDouble3>();
			double lFactor = lFactorProperty.Get<FbxDouble>();
			if (lFactor != 1)
			{
				lResult[0] *= lFactor;
				lResult[1] *= lFactor;
				lResult[2] *= lFactor;
			}

			pElement->type = FBX_MATRIAL_ELEMENT::ELEMENT_COLOR;
		}

		if (lProperty.IsValid())
		{
			int existTextureCount = 0;
			const int lTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();

			for (int i = 0; i<lTextureCount; i++)
			{
				FbxFileTexture* lFileTexture = lProperty.GetSrcObject<FbxFileTexture>(i);
				if (!lFileTexture)
					continue;

				FbxString uvsetName = lFileTexture->UVSet.Get();
				std::string uvSetString = uvsetName.Buffer();
				std::string filepath = lFileTexture->GetFileName();

				pElement->textureSetArray[uvSetString].push_back(filepath);
				existTextureCount++;
			}

			const int lLayeredTextureCount = lProperty.GetSrcObjectCount<FbxLayeredTexture>();

			for (int i = 0; i<lLayeredTextureCount; i++)
			{
				FbxLayeredTexture* lLayeredTexture = lProperty.GetSrcObject<FbxLayeredTexture>(i);

				const int lTextureFileCount = lLayeredTexture->GetSrcObjectCount<FbxFileTexture>();

				for (int j = 0; j<lTextureFileCount; j++)
				{
					FbxFileTexture* lFileTexture = lLayeredTexture->GetSrcObject<FbxFileTexture>(j);
					if (!lFileTexture)
						continue;

					FbxString uvsetName = lFileTexture->UVSet.Get();
					std::string uvSetString = uvsetName.Buffer();
					std::string filepath = lFileTexture->GetFileName();

					pElement->textureSetArray[uvSetString].push_back(filepath);
					existTextureCount++;
				}
			}

			if (existTextureCount > 0)
			{
				if (pElement->type == FBX_MATRIAL_ELEMENT::ELEMENT_COLOR)
					pElement->type = FBX_MATRIAL_ELEMENT::ELEMENT_BOTH;
				else
					pElement->type = FBX_MATRIAL_ELEMENT::ELEMENT_TEXTURE;
			}
		}

		return lResult;
	}

	void CFBXLoader::CopyMatrialData(FbxSurfaceMaterial* mat, FBX_MATERIAL_NODE* destMat)
	{
		if (!mat)
			return;

		if (mat->GetClassId().Is(FbxSurfaceLambert::ClassId))
		{
			destMat->type = FBX_MATERIAL_NODE::MATERIAL_LAMBERT;
		}
		else if (mat->GetClassId().Is(FbxSurfacePhong::ClassId))
		{
			destMat->type = FBX_MATERIAL_NODE::MATERIAL_PHONG;
		}

		const FbxDouble3 lEmissive = GetMaterialProperty(mat,
			FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor, &destMat->emmisive);
		SetFbxColor(destMat->emmisive, lEmissive);

		const FbxDouble3 lAmbient = GetMaterialProperty(mat,
			FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor, &destMat->ambient);
		SetFbxColor(destMat->ambient, lAmbient);

		const FbxDouble3 lDiffuse = GetMaterialProperty(mat,
			FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor, &destMat->diffuse);
		SetFbxColor(destMat->diffuse, lDiffuse);

		const FbxDouble3 lSpecular = GetMaterialProperty(mat,
			FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor, &destMat->specular);
		SetFbxColor(destMat->specular, lSpecular);

		
		FbxProperty lTransparencyFactorProperty = mat->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
		if (lTransparencyFactorProperty.IsValid())
		{
			double lTransparencyFactor = lTransparencyFactorProperty.Get<FbxDouble>();
			destMat->TransparencyFactor = static_cast<float>(lTransparencyFactor);
		}

		FbxProperty lShininessProperty = mat->FindProperty(FbxSurfaceMaterial::sShininess);
		if (lShininessProperty.IsValid())
		{
			double lShininess = lShininessProperty.Get<FbxDouble>();
			destMat->shininess = static_cast<float>(lShininess);
		}
	}

	
	void CFBXLoader::ComputeNodeMatrix(FbxNode* pNode, FBX_MESH_NODE* meshNode)
	{
		if (!pNode || !meshNode)
		{
			return;
		}

		//FbxAnimEvaluator* lEvaluator = mScene->GetEvaluator();
		FbxMatrix lGlobal;
		lGlobal.SetIdentity();

		if (pNode != mScene->GetRootNode())
		{
			//lGlobal= lEvaluator->GetNodeGlobalTransform(pNode);

			FBXMatrixToFloat16(&lGlobal, meshNode->mat4x4);
		}
		else
		{
			FBXMatrixToFloat16(&lGlobal, meshNode->mat4x4);
		}
	}

	
	void CFBXLoader::CopyVertexData(FbxMesh*	pMesh, FBX_MESH_NODE* meshNode)
	{
		if (!pMesh)
			return;

		int lPolygonCount = pMesh->GetPolygonCount();

		FbxVector4 pos, nor;

		meshNode->elements.numPosition = 1;
		meshNode->elements.numNormal = 1;

		unsigned int indx = 0;

		for (int i = 0; i<lPolygonCount; i++)
		{
			//폴리곤의 크기
			int lPolygonsize = pMesh->GetPolygonSize(i);

			for (int pol = 0; pol<lPolygonsize; pol++)
			{
				int index = pMesh->GetPolygonVertex(i, pol);//폴리곤의 인덱스를 갖고옴
				meshNode->indexArray.push_back(indx);//인덱스행렬에 집어넣음

				pos = pMesh->GetControlPointAt(index); //인덱스의 컨트롤 포인트를 갖고옴
				pMesh->GetPolygonVertexNormal(i, pol, nor);//폴리곤 버텍스의 노말값

				meshNode->m_positionArray.push_back(pos); //컨트롤포인트 집어넣
				meshNode->m_normalArray.push_back(nor); //노말값 집어넣

				++indx;
			}
		}

		FbxStringList	uvsetName;
		pMesh->GetUVSetNames(uvsetName);
		int numUVSet = uvsetName.GetCount();
		meshNode->elements.numUVSet = numUVSet;

		bool unmapped = false;

		for (int uv = 0; uv<numUVSet; uv++)
		{
			meshNode->uvsetID[uvsetName.GetStringAt(uv)] = uv;
			for (int i = 0; i<lPolygonCount; i++)
			{
				int lPolygonsize = pMesh->GetPolygonSize(i);

				for (int pol = 0; pol<lPolygonsize; pol++)
				{
					FbxString name = uvsetName.GetStringAt(uv);
					FbxVector2 texCoord;
					pMesh->GetPolygonVertexUV(i, pol, name, texCoord, unmapped);
					meshNode->m_texcoordArray.push_back(texCoord);
				}
			}
		}
	}

	FBX_MESH_NODE& CFBXLoader::GetNode(const unsigned int id)
	{
		return m_meshNodeArray[id];
	}

}