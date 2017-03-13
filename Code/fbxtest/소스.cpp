#include <iostream>
#include <string>
#include <fbxsdk.h>
#include "Common/Common.h"

using namespace std;

int main(int argc, char* argv)
{
	FbxManager* mSdkManager = NULL;
	FbxScene* mScene = NULL;
	FbxImporter* mImporter = NULL;
	FbxArray<FbxString*> mAnimStackNameArray = NULL;
	FbxTime mFrameTime;

	InitializeSdkObjects(mSdkManager, mScene);

	bool result;

	string fn;
	cin >> fn;

	FbxString mFileName(fn.data());

	InitializeSdkObjects(mSdkManager, mScene);
	if (mSdkManager)
	{
		FbxIOSettings* ios = FbxIOSettings::Create(mSdkManager, IOSROOT);
		mSdkManager->SetIOSettings(ios);
	
		FbxString lPath = FbxGetApplicationDirectory();
		mSdkManager->LoadPluginsDirectory(lPath.Buffer());

		int mFileFormat = -1;
		mImporter = FbxImporter::Create(mSdkManager, "");
		
		mScene = FbxScene::Create(mSdkManager, "My Scene");
	
		if (!mScene)
		{
			FBXSDK_printf("Error: Unable to create FBX scene!\n");
			exit(1);
		}

		if (!mSdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(mFileName, mFileFormat))
		{
			// Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
			mFileFormat = mSdkManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");;
		}
		
		if (mImporter->Initialize(mFileName, mFileFormat) == true)
		{
				// The file is going to be imported at
				// the end of the first display callback.
				FBXSDK_printf( "Importing file ");
				FBXSDK_printf(mFileName);
				FBXSDK_printf("\nPlease wait!");

		}
		else
		{
			FBXSDK_printf("Fail");
		}
		
		FbxAxisSystem OurAxisSystem = FbxAxisSystem::DirectX;
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

		mScene->FillAnimStackNameArray(mAnimStackNameArray);

		FbxGeometryConverter lGeomConverter(mSdkManager);
		lGeomConverter.Triangulate(mScene, /*replace*/true);

		mFrameTime.SetTime(0, 0, 0, 1, 0, mScene->GetGlobalSettings().GetTimeMode());
		result = LoadScene(mSdkManager, mScene, mFileName);
	}
}