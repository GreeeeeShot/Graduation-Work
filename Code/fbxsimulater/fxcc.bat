set PATH=c:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Utilities\bin\x86\
	cd ./
	fxc /Od /Zi /Ges /T vs_5_0 /E VS /Fo VS.fxo Effect.fx
	fxc /Od /Zi /Ges /T ps_5_0 /E PS /Fo PS.fxo Effect.fx
	cd ..