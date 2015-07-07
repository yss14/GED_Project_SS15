#include <stdio.h>
#include <tchar.h>

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <vector>
#include <iostream>
#include <map>
#include <list>

#include "SpriteRenderer.h"
#include "dxut.h"
#include "DXUTmisc.h"
#include "DXUTcamera.h"
#include "DXUTgui.h"
#include "DXUTsettingsDlg.h"
#include "SDKmisc.h"
#include "ConfigParser.h"
#include "Mesh.h"
#include "d3dx11effect.h"
#include "Terrain.h"
#include "GameEffect.h"


// Help macros
#define DEG2RAD( a ) ( (a) * XM_PI / 180.f )

using namespace std;
using namespace DirectX;

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
struct EnemyInstance{
	XMFLOAT3 position; // Have to be XMFloat3, because c++ compiler optimization gets in trouble with XMVector
	XMFLOAT3 velocity;

	int remainingHP;
	std::string typeName;

	XMFLOAT3 s1, s2;
};
// Camera
struct CAMERAPARAMS {
	float   fovy;
	float   aspect;
	float   nearPlane;
	float   farPlane;
}                                       g_cameraParams;
float                                   g_cameraMoveScaler = 1000.f;
float                                   g_cameraRotateScaler = 0.01f;
CFirstPersonCamera                      g_camera;               // A first person camera

// User Interface
CDXUTDialogResourceManager              g_dialogResourceManager; // manager for shared resources of dialogs
CD3DSettingsDlg                         g_settingsDlg;          // Device settings dialog
CDXUTTextHelper*                        g_txtHelper = nullptr;
CDXUTDialog                             g_hud;                  // dialog for standard controls
CDXUTDialog                             g_sampleUI;             // dialog for sample specific controls

//ID3D11InputLayout*                      g_terrainVertexLayout; // Describes the structure of the vertex buffer to the input assembler stage


bool                                    g_terrainSpinning = true;
XMMATRIX                                g_terrainWorld; // object- to world-space transformation


// Scene information
XMVECTOR                                g_lightDir;
Terrain									g_terrain;

GameEffect								g_gameEffect; // CPU part of Shader
ConfigParser*							cfgParser; // ConfigParser Reference

std::map<std::string, Mesh*>			g_Meshes;

std::list<EnemyInstance*>				g_enemyInstances;

bool									canMove = false;
float									g_SpawnTimer = 0.0f;

SpriteRenderer*							g_SpriteRenderer;
std::vector<SpriteVertex>				spritesVector;

float									gatlingTime = 0.0f;
float									plasmaTime = 0.0f;
//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           2
#define IDC_CHANGEDEVICE        3
#define IDC_TOGGLESPIN          4
#define IDC_RELOAD_SHADERS		101

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
	void* pUserContext);
void CALLBACK OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext);
void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext);
void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext);
bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext);

bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo *, UINT, const CD3D11EnumDeviceInfo *,
	DXGI_FORMAT, bool, void*);
HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
	void* pUserContext);
HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
	const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext);
void CALLBACK OnD3D11DestroyDevice(void* pUserContext);
void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime,
	float fElapsedTime, void* pUserContext);

void InitApp();
void RenderText();
void DeinitApp();
boolean SpriteSort(const SpriteVertex &s1, const SpriteVertex &s2);
void ReleaseShader();
HRESULT ReloadShader(ID3D11Device* pd3dDevice);

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);

	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// Old Direct3D Documentation:
	// Start > All Programs > Microsoft DirectX SDK (June 2010) > Windows DirectX Graphics Documentation

	// DXUT Documentaion:
	// Start > All Programs > Microsoft DirectX SDK (June 2010) > DirectX Documentation for C++ : The DirectX Software Development Kit > Programming Guide > DXUT

	// New Direct3D Documentaion (just for reference, use old documentation to find explanations):
	// http://msdn.microsoft.com/en-us/library/windows/desktop/hh309466%28v=vs.85%29.aspx


	// Initialize COM library for windows imaging components
	/*HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (hr != S_OK)
	{
	MessageBox(0, L"Error calling CoInitializeEx", L"Error", MB_OK | MB_ICONERROR);
	exit(-1);
	}*/


	// Set DXUT callbacks
	DXUTSetCallbackMsgProc(MsgProc);
	DXUTSetCallbackKeyboard(OnKeyboard);
	DXUTSetCallbackFrameMove(OnFrameMove);
	DXUTSetCallbackDeviceChanging(ModifyDeviceSettings);

	DXUTSetCallbackD3D11DeviceAcceptable(IsD3D11DeviceAcceptable);
	DXUTSetCallbackD3D11DeviceCreated(OnD3D11CreateDevice);
	DXUTSetCallbackD3D11SwapChainResized(OnD3D11ResizedSwapChain);
	DXUTSetCallbackD3D11SwapChainReleasing(OnD3D11ReleasingSwapChain);
	DXUTSetCallbackD3D11DeviceDestroyed(OnD3D11DestroyDevice);
	DXUTSetCallbackD3D11FrameRender(OnD3D11FrameRender);
	//DXUTSetIsInGammaCorrectMode(false);

	InitApp();
	DXUTInit(true, true, nullptr); // Parse the command line, show msgboxes on error, no extra command line params
	DXUTSetCursorSettings(true, true);
	DXUTCreateWindow(L"Ultra Game lel"); // You may change the title

	DXUTCreateDevice(D3D_FEATURE_LEVEL_10_0, true, 1280, 720);

	DXUTMainLoop(); // Enter into the DXUT render loop

	DXUTShutdown();
	DeinitApp();
	return DXUTGetExitCode();
}

//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
	HRESULT hr;
	WCHAR path[MAX_PATH];

	// Parse the config file

	V(DXUTFindDXSDKMediaFileCch(path, MAX_PATH, L"game.cfg"));
	char pathA[MAX_PATH];
	size_t size;
	wcstombs_s(&size, pathA, path, MAX_PATH);

	cfgParser = new ConfigParser();
	cfgParser->load(pathA);
	std::cout << "Loaded game.cfg from " << pathA << "\n";

	g_Meshes;
	std::vector<wstring> sprites;
	sprites.push_back(Utils::buildRessourcePath(cfgParser->projectileData["Gatling"]->textureName));
	sprites.push_back(Utils::buildRessourcePath(cfgParser->projectileData["Plasma"]->textureName));

	g_SpriteRenderer = new SpriteRenderer(sprites);
	srand(time(0));
	//for (int i = 0; i < 500; i++)
	//{
		//spritesVector.push_back(SpriteVertex(
		//	DirectX::XMFLOAT3(
		//		rand() / (float)(RAND_MAX) * 1000 - 500.0f, 
		//		rand() / (float)(RAND_MAX) * 500 + 100,
		//		rand() / (float)(RAND_MAX) * 1000 - 500.0f), 
		//		rand() / (float)(RAND_MAX) * 50, 
		//		int(rand() / (float)(RAND_MAX) * 2)));	
	//}

	// Intialize the user interface

	g_settingsDlg.Init(&g_dialogResourceManager);
	g_hud.Init(&g_dialogResourceManager);
	g_sampleUI.Init(&g_dialogResourceManager);

	g_hud.SetCallback(OnGUIEvent);
	int iY = 30;
	int iYo = 26;
	g_hud.AddButton(IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 0, iY, 170, 22);
	g_hud.AddButton(IDC_TOGGLEREF, L"Toggle REF (F3)", 0, iY += iYo, 170, 22, VK_F3);
	g_hud.AddButton(IDC_CHANGEDEVICE, L"Change device (F2)", 0, iY += iYo, 170, 22, VK_F2);

	g_hud.AddButton(IDC_RELOAD_SHADERS, L"Reload shaders (F5)", 0, iY += 24, 170, 22, VK_F5);

	g_sampleUI.SetCallback(OnGUIEvent); iY = 10;
	iY += 24;
	g_sampleUI.AddCheckBox(IDC_TOGGLESPIN, L"Toggle Spinning", 0, iY += 24, 125, 22, g_terrainSpinning);

	// init cockpit mesh
	//std::wcout << Utils::buildRessourcePath(cfgParser->getCockpitMeshFiles()->modelPath) << std::endl;
	//std::wcout << Utils::buildRessourcePath(cfgParser->getCockpitMeshFiles()->diffuseTexturePath) << std::endl;
	//std::wcout << Utils::buildRessourcePath(cfgParser->getCockpitMeshFiles()->specularTexturePath) << std::endl;
	//std::wcout << Utils::buildRessourcePath(cfgParser->getCockpitMeshFiles()->glowTexturePath) << std::endl;
	//std::wcout << Utils::buildRessourcePath(cfgParser->meshPathes["Cockpit"]->modelPath) << std::endl;
	for (auto iterator = cfgParser->meshPathes.begin(); iterator != cfgParser->meshPathes.end(); iterator++) {
		g_Meshes[iterator->first] = new Mesh(Utils::buildRessourcePath(cfgParser->meshPathes[iterator->first]->modelPath),
			Utils::buildRessourcePath(cfgParser->meshPathes[iterator->first]->diffuseTexturePath),
			Utils::buildRessourcePath(cfgParser->meshPathes[iterator->first]->specularTexturePath),
			Utils::buildRessourcePath(cfgParser->meshPathes[iterator->first]->glowTexturePath));
	}
}

//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for 
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText()
{
	g_txtHelper->Begin();
	g_txtHelper->SetInsertionPos(5, 5);
	g_txtHelper->SetForegroundColor(XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f));
	g_txtHelper->DrawTextLine(DXUTGetFrameStats(true)); //DXUTIsVsyncEnabled() ) );
	g_txtHelper->DrawTextLine(DXUTGetDeviceStats());
	g_txtHelper->End();
}

//--------------------------------------------------------------------------------------
// Reject any D3D11 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo *, UINT, const CD3D11EnumDeviceInfo *,
	DXGI_FORMAT, bool, void*)
{
	return true;
}

//--------------------------------------------------------------------------------------
// Specify the initial device settings
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext)
{
	UNREFERENCED_PARAMETER(pDeviceSettings);
	UNREFERENCED_PARAMETER(pUserContext);

	// For the first device created if its a REF device, optionally display a warning dialog box
	static bool s_bFirstTime = true;
	if (s_bFirstTime)
	{
		s_bFirstTime = false;
		if (pDeviceSettings->d3d11.DriverType == D3D_DRIVER_TYPE_REFERENCE)
		{
			DXUTDisplaySwitchingToREFWarning();
		}
	}
	//// Enable anti aliasing
	pDeviceSettings->d3d11.sd.SampleDesc.Count = 4;
	pDeviceSettings->d3d11.sd.SampleDesc.Quality = 1;

	return true;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice,
	const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	UNREFERENCED_PARAMETER(pBackBufferSurfaceDesc);
	UNREFERENCED_PARAMETER(pUserContext);

	HRESULT hr;

	ID3D11DeviceContext* pd3dImmediateContext = DXUTGetD3D11DeviceContext(); // http://msdn.microsoft.com/en-us/library/ff476891%28v=vs.85%29
	V_RETURN(g_dialogResourceManager.OnD3D11CreateDevice(pd3dDevice, pd3dImmediateContext));
	V_RETURN(g_settingsDlg.OnD3D11CreateDevice(pd3dDevice));
	g_txtHelper = new CDXUTTextHelper(pd3dDevice, pd3dImmediateContext, &g_dialogResourceManager, 15);

	V_RETURN(ReloadShader(pd3dDevice));

	// Define the input layout
	const D3D11_INPUT_ELEMENT_DESC layout[] = // http://msdn.microsoft.com/en-us/library/bb205117%28v=vs.85%29.aspx
	{
		{ "SV_POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	// Create the input layout
	D3DX11_PASS_DESC pd;
	V_RETURN(g_gameEffect.pass0->GetDesc(&pd));
	//V_RETURN( pd3dDevice->CreateInputLayout( layout, numElements, pd.pIAInputSignature, pd.IAInputSignatureSize, &g_terrainVertexLayout ) );

	// Create the terrain

	// TODO: You might pass a ConfigParser object to the create function.
	//       Therefore you can adjust the TerrainClass accordingly
	V_RETURN(g_terrain.create(pd3dDevice, cfgParser));
	// Initialize the camera
	XMVECTOR vEye = XMVectorSet(0.0f, (g_terrain.getCenterHeight() + 0.5) * cfgParser->getTerrainHeight(), 0.0f, 0.0f);   // Camera eye is here
	std::cout << g_terrain.getCenterHeight() << std::endl;
	XMVECTOR vAt = XMVectorSet(1.0f, (g_terrain.getCenterHeight() + 0.5) * cfgParser->getTerrainHeight(), 0.0f, 1.0f);               // ... facing at this position
	g_camera.SetViewParams(vEye, vAt); // http://msdn.microsoft.com/en-us/library/windows/desktop/bb206342%28v=vs.85%29.aspx
	g_camera.SetScalers(g_cameraRotateScaler, g_cameraMoveScaler);
	//std::cout << "In Create Device" << std::endl;
	for (auto iterator = g_Meshes.begin(); iterator != g_Meshes.end(); iterator++) {
		V_RETURN(g_Meshes[iterator->first]->create(pd3dDevice));
	}

	V_RETURN(Mesh::createInputLayout(pd3dDevice, g_gameEffect.meshPass1));
	V_RETURN(g_SpriteRenderer->create(pd3dDevice));
	return S_OK;
}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11DestroyDevice(void* pUserContext)
{
	UNREFERENCED_PARAMETER(pUserContext);

	g_dialogResourceManager.OnD3D11DestroyDevice();
	g_settingsDlg.OnD3D11DestroyDevice();
	DXUTGetGlobalResourceCache().OnDestroyDevice();
	//SAFE_RELEASE( g_terrainVertexLayout );

	// Destroy the terrain

	for (auto iterator = g_Meshes.begin(); iterator != g_Meshes.end(); iterator++) {
		g_Meshes[iterator->first]->destroy();
	}

	g_terrain.destroy();
	g_SpriteRenderer->destroy();
	Mesh::destroyInputLayout();
	SAFE_DELETE(g_txtHelper);
	ReleaseShader();
}

//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
	const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	UNREFERENCED_PARAMETER(pSwapChain);
	UNREFERENCED_PARAMETER(pUserContext);

	HRESULT hr;

	// Intialize the user interface

	V_RETURN(g_dialogResourceManager.OnD3D11ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc));
	V_RETURN(g_settingsDlg.OnD3D11ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc));

	g_hud.SetLocation(pBackBufferSurfaceDesc->Width - 170, 0);
	g_hud.SetSize(170, 170);
	g_sampleUI.SetLocation(pBackBufferSurfaceDesc->Width - 170, pBackBufferSurfaceDesc->Height - 300);
	g_sampleUI.SetSize(170, 300);

	// Initialize the camera

	g_cameraParams.aspect = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
	g_cameraParams.fovy = 0.785398f;
	g_cameraParams.nearPlane = 1.f;
	g_cameraParams.farPlane = 50000.f;

	g_camera.SetProjParams(g_cameraParams.fovy, g_cameraParams.aspect, g_cameraParams.nearPlane, g_cameraParams.farPlane);
	g_camera.SetEnablePositionMovement(false);
	g_camera.SetRotateButtons(true, false, false);
	g_camera.SetScalers(g_cameraRotateScaler, g_cameraMoveScaler);
	g_camera.SetDrag(true);

	return S_OK;
}

//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext)
{
	UNREFERENCED_PARAMETER(pUserContext);
	g_dialogResourceManager.OnD3D11ReleasingSwapChain();
}

//--------------------------------------------------------------------------------------
// Loads the effect from file
// and retrieves all dependent variables
//--------------------------------------------------------------------------------------
HRESULT ReloadShader(ID3D11Device* pd3dDevice)
{
	assert(pd3dDevice != NULL);

	HRESULT hr;
	ReleaseShader();
	V_RETURN(g_gameEffect.create(pd3dDevice));
	V_RETURN(g_SpriteRenderer->reloadShader(pd3dDevice));
	return S_OK;
}

//--------------------------------------------------------------------------------------
// Release resources created in ReloadShader
//--------------------------------------------------------------------------------------
void ReleaseShader()
{
	g_gameEffect.destroy();
	g_SpriteRenderer->releaseShader();
}

//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
	void* pUserContext)
{
	UNREFERENCED_PARAMETER(pUserContext);

	// Pass messages to dialog resource manager calls so GUI state is updated correctly
	*pbNoFurtherProcessing = g_dialogResourceManager.MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;

	// Pass messages to settings dialog if its active
	if (g_settingsDlg.IsActive())
	{
		g_settingsDlg.MsgProc(hWnd, uMsg, wParam, lParam);
		return 0;
	}

	// Give the dialogs a chance to handle the message first
	*pbNoFurtherProcessing = g_hud.MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;
	*pbNoFurtherProcessing = g_sampleUI.MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;

	// Use the mouse weel to control the movement speed
	if (uMsg == WM_MOUSEWHEEL) {
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		g_cameraMoveScaler *= (1 + zDelta / 500.0f);
		if (g_cameraMoveScaler < 0.1f)
			g_cameraMoveScaler = 0.1f;
		g_camera.SetScalers(g_cameraRotateScaler, g_cameraMoveScaler);
	}

	// Pass all remaining windows messages to camera so it can respond to user input
	g_camera.HandleMessages(hWnd, uMsg, wParam, lParam);

	return 0;
}

//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext)
{
	if (nChar == 'C' && bKeyDown){
		canMove = !canMove;
	}

	if (nChar == 'G')			// Fire Gatling Gun
	{
		if (gatlingTime > cfgParser->projectileData["Gatling"]->firerate)
		{
			const XMFLOAT3 tmpPos(cfgParser->projectileData["Gatling"]->posX, cfgParser->projectileData["Gatling"]->posY, cfgParser->projectileData["Gatling"]->posZ);

			XMFLOAT3 newtmpPos;
			XMFLOAT3 tmpVel;
			XMStoreFloat3(&tmpVel, g_camera.GetWorldAhead());
			XMStoreFloat3(&tmpVel, XMLoadFloat3(&tmpVel)  * cfgParser->projectileData["Gatling"]->speed);
			XMStoreFloat3(&newtmpPos, XMVector3Transform(XMLoadFloat3(&tmpPos), g_camera.GetWorldMatrix()));
			spritesVector.push_back(SpriteVertex(newtmpPos, tmpVel, cfgParser->projectileData["Gatling"]->radius, 0, cfgParser->projectileData["Gatling"]->gravity, cfgParser->projectileData["Gatling"]->damage));
			std::cout << "Fire Gatling \n";
			gatlingTime = 0.0f;
		}
	}

	if (nChar == 'P')			// Fire Plasma Gun
	{
		if (plasmaTime > cfgParser->projectileData["Plasma"]->firerate)
		{
			const XMFLOAT3 tmpPos(cfgParser->projectileData["Plasma"]->posX, cfgParser->projectileData["Plasma"]->posY, cfgParser->projectileData["Plasma"]->posZ);
			
			XMFLOAT3 newtmpPos;
			XMFLOAT3 tmpVel;
			XMStoreFloat3(&tmpVel, g_camera.GetWorldAhead());
			XMStoreFloat3(&tmpVel, XMLoadFloat3(&tmpVel)  * cfgParser->projectileData["Gatling"]->speed);
			XMStoreFloat3(&newtmpPos, XMVector3Transform(XMLoadFloat3(&tmpPos), g_camera.GetWorldMatrix()));
			spritesVector.push_back(SpriteVertex(newtmpPos, tmpVel, cfgParser->projectileData["Plasma"]->radius, 1, cfgParser->projectileData["Plasma"]->gravity, cfgParser->projectileData["Plasma"]->damage));
			std::cout << "Fire Plasma \n";
			plasmaTime = 0.0f;
		}
	}

	g_camera.SetEnablePositionMovement(canMove);
	UNREFERENCED_PARAMETER(nChar);
	UNREFERENCED_PARAMETER(bKeyDown);
	UNREFERENCED_PARAMETER(bAltDown);
	UNREFERENCED_PARAMETER(pUserContext);
}

//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext)
{
	UNREFERENCED_PARAMETER(nEvent);
	UNREFERENCED_PARAMETER(pControl);
	UNREFERENCED_PARAMETER(pUserContext);

	switch (nControlID)
	{
	case IDC_TOGGLEFULLSCREEN:
		DXUTToggleFullScreen(); break;
	case IDC_TOGGLEREF:
		DXUTToggleREF(); break;
	case IDC_CHANGEDEVICE:
		g_settingsDlg.SetActive(!g_settingsDlg.IsActive()); break;
	case IDC_TOGGLESPIN:
		g_terrainSpinning = g_sampleUI.GetCheckBox(IDC_TOGGLESPIN)->GetChecked();
		break;
	case IDC_RELOAD_SHADERS:
		ReloadShader(DXUTGetD3D11Device());
		break;
	}
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
	for (auto iter = g_enemyInstances.begin(); iter != g_enemyInstances.end();){
		// update enemy position
		XMVECTOR posAsVector = XMLoadFloat3(&(*iter)->position);
		XMVECTOR newPos = posAsVector + fElapsedTime * XMLoadFloat3(&(*iter)->velocity);
		(*iter)->position = XMFLOAT3(XMVectorGetX(newPos), XMVectorGetY(newPos), XMVectorGetZ(newPos));

		//EnemyInstance *instance = *iter;
		XMVECTOR tmp = XMVector3Length(XMVectorSubtract(posAsVector, XMLoadFloat3(&(*iter)->s2)));
		float distance = 0.0f;
		XMStoreFloat(&distance, tmp);

		if (distance < 2.0f)// remove enemy
		{		
			auto itrm = iter;
			iter++;
			g_enemyInstances.remove(*itrm);
		}
		else
		{
			iter++;
		}


	}

	UNREFERENCED_PARAMETER(pUserContext);
	// Update the camera's position based on user input 
	g_camera.FrameMove(fElapsedTime);

	// Initialize the terrain world matrix
	// http://msdn.microsoft.com/en-us/library/windows/desktop/bb206365%28v=vs.85%29.aspx

	// Start with identity matrix
	g_terrainWorld = XMMatrixIdentity();

	//Scaling
	g_terrainWorld *= XMMatrixScaling(cfgParser->getTerrainWidth(), cfgParser->getTerrainHeight(), cfgParser->getTerrainDepth());
	//g_terrainWorld *= XMMatrixScaling(10.0f, 800.0f, 10.0f);

	if (g_terrainSpinning)
	{
		// If spinng enabled, rotate the world matrix around the y-axis
		g_terrainWorld *= XMMatrixRotationY(0.0f * DEG2RAD((float)fTime)); // Rotate around world-space "up" axis
	}

	// Set the light vector
	g_lightDir = XMVectorSet(1, 1, 1, 0); // Direction to the directional light in world space    
	g_lightDir = XMVector3Normalize(g_lightDir);

	g_SpawnTimer -= fElapsedTime;
	if (g_SpawnTimer < 0)
	{
		g_SpawnTimer += cfgParser->getIntervall();
		// spawn enemy

		EnemyInstance* instance = new EnemyInstance();
		float a = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (XM_2PI)));

		float height = cfgParser->getMinSpawn() + float(rand()) / float(RAND_MAX) * (cfgParser->getMaxSpawn() - cfgParser->getMinSpawn());

		instance->s1 = XMFLOAT3(3 * cfgParser->getTerrainWidth() * std::cos(a), -1 * height, 3 * cfgParser->getTerrainDepth() * std::sin(a));
		instance->s2 = XMFLOAT3(cfgParser->getTerrainWidth() * std::cos(a), -1 * (g_terrain.getCenterHeight() + 0.2f) * cfgParser->getTerrainHeight(),cfgParser->getTerrainDepth() * std::sin(a));

		instance->position = instance->s1;
		int randomizer = rand() % 5;

		std::string enemyType;

		switch (randomizer)
		{
		case 0:
			enemyType = "AmyShip1";
			break;
		case 1:
			enemyType = "AmyShip2";
			break;
		case 2:
			enemyType = "JufShip";
			break;
		case 3: 
			enemyType = "ManShip1";
			break;
		case 4:
			enemyType = "ManShip2";
			break;
		default:
			enemyType = "JufShip";
		}
		XMVECTOR directionNormal = cfgParser->objectsEnemyData[enemyType]->speed * XMVector3Normalize(XMLoadFloat3(&instance->s2) - XMLoadFloat3(&instance->s1));
		instance->velocity = XMFLOAT3(XMVectorGetX(directionNormal), XMVectorGetY(directionNormal), XMVectorGetZ(directionNormal));
		instance->typeName = enemyType;
		g_enemyInstances.push_back(instance);
		//delete instance;
		//std::cout << "Number of enemies: " << g_enemyInstances.size() << std::endl;
	}

	// Projectile Handling
	plasmaTime += fElapsedTime;
	gatlingTime += fElapsedTime;

	for (int i = 0; i < spritesVector.size(); i++)
	{
		const XMFLOAT3 newGrav(0.0f, spritesVector[i].gravity * fElapsedTime, 0.0f);
		XMVECTOR gravity = XMLoadFloat3(&newGrav);
		const XMFLOAT3 vel(spritesVector[i].velocity);
		XMVECTOR velo = XMLoadFloat3(&vel);
		XMFLOAT3 NewVel(spritesVector[i].velocity);
		XMStoreFloat3(&NewVel, velo + gravity);
		spritesVector[i].velocity = NewVel;
		XMVECTOR newVelVec = XMLoadFloat3(&NewVel);

		const XMFLOAT3 oldPos3 = spritesVector[i].position;
		XMVECTOR oldPos = XMLoadFloat3(&oldPos3);
		const XMFLOAT3 deltaTime(fElapsedTime, fElapsedTime, fElapsedTime);
		XMVECTOR deltaVec = XMLoadFloat3(&deltaTime);
		XMVECTOR newPos = oldPos + newVelVec * deltaVec;
		XMStoreFloat3(&spritesVector[i].position, newPos);
		
		XMVECTOR distance = newPos - g_camera.GetEyePt();
		float distancef = 0.0f;
		XMStoreFloat(&distancef, XMVector3Length(distance));
		spritesVector[i].distanceToCam = distancef;

		if (distancef > 10000.0f)
		{
			spritesVector.erase(spritesVector.begin() + i);
			i--;
			std::cout << "Number of projectiles " << spritesVector.size() << "\n";
		} else
		{
			// check for colision with enemy
			for (auto iter = g_enemyInstances.begin(); iter != g_enemyInstances.end();)
			{
				const XMFLOAT3 enemyPos3 = (*iter)->position;
				XMVECTOR enemyPos = XMLoadFloat3(&enemyPos3);
				XMVECTOR distance = enemyPos - newPos;
				float distancec1c2;
				XMStoreFloat(&distancec1c2, XMVector3Length(distance));

				if (distancec1c2 < pow(cfgParser->objectsEnemyData[(*iter)->typeName]->size + spritesVector[i].radius, 2))
				{
					(*iter)->remainingHP -= spritesVector[i].damage;
					// Remove projectile
					spritesVector.erase(spritesVector.begin() + i);
					i--;
					std::cout << "Hit" << std::endl; 

					if ((*iter)->remainingHP <= 0)
					{
						auto itrm = iter;
						iter++;
						g_enemyInstances.remove(*itrm);
						std::cout << "Enemy died! \n";
					} else
					{
						iter++;
					}
					break;
				}else
				{
					iter++;
				}
			}
		}
		
	}

	// sort Projectiles

	std::sort(spritesVector.begin(), spritesVector.end(), SpriteSort);


	// Check if enemies reached target location
	//for (auto iter = g_enemyInstances.begin(); iter != g_enemyInstances.end();) {
	//	iter++;
	//	//EnemyInstance *instance = *iter;
	//	XMVECTOR tmp = XMVector3Length(XMVectorSubtract((*iter)->position, (*iter)->s2));
	//	float distance = 0.0f;
	//	XMStoreFloat(&distance, tmp);

	//	if (distance < 0){
	//		std::cout << "enemy reached target location" << std::endl;
	//	}
	//	else{
	//		std::cout << "Distance to target location: " << distance << std::endl;
	//	}
	//}

}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D11 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime,
	float fElapsedTime, void* pUserContext)
{
	UNREFERENCED_PARAMETER(pd3dDevice);
	UNREFERENCED_PARAMETER(fTime);
	UNREFERENCED_PARAMETER(pUserContext);

	HRESULT hr;

	// If the settings dialog is being shown, then render it instead of rendering the app's scene
	if (g_settingsDlg.IsActive())
	{
		g_settingsDlg.OnRender(fElapsedTime);
		return;
	}

	ID3D11RenderTargetView* pRTV = DXUTGetD3D11RenderTargetView();
	float clearColor[4] = { 0.0f, 0.3f, 0.9f, 1.0f };		// background color 
	pd3dImmediateContext->ClearRenderTargetView(pRTV, clearColor);

	if (g_gameEffect.effect == nullptr) {
		g_txtHelper->Begin();
		g_txtHelper->SetInsertionPos(5, 5);
		g_txtHelper->SetForegroundColor(XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f));
		g_txtHelper->DrawTextLine(L"SHADER ERROR");
		g_txtHelper->End();
		return;
	}

	// Clear the depth stencil
	ID3D11DepthStencilView* pDSV = DXUTGetD3D11DepthStencilView();
	pd3dImmediateContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0, 0);

	// Update variables that change once per frame
	XMMATRIX const view = g_camera.GetViewMatrix(); // http://msdn.microsoft.com/en-us/library/windows/desktop/bb206342%28v=vs.85%29.aspx
	XMMATRIX const proj = g_camera.GetProjMatrix(); // http://msdn.microsoft.com/en-us/library/windows/desktop/bb147302%28v=vs.85%29.aspx
	XMMATRIX worldViewProj = g_terrainWorld * view * proj;

	//inverse transposed worldNormalsMatrix
	XMMATRIX worldNormalsMatrix = XMMatrixInverse(NULL, g_terrainWorld);
	worldNormalsMatrix = XMMatrixTranspose(worldNormalsMatrix);

	V(g_gameEffect.worldEV->SetMatrix((float*)&g_terrainWorld));
	V(g_gameEffect.worldViewProjectionEV->SetMatrix((float*)&worldViewProj));
	V(g_gameEffect.worldNormalsMatrix->SetMatrix((float*)&worldNormalsMatrix));
	V(g_gameEffect.lightDirEV->SetFloatVector((float*)&g_lightDir));

	// Set input layout
	//pd3dImmediateContext->IASetInputLayout( g_terrainVertexLayout );

	g_terrain.render(pd3dImmediateContext, g_gameEffect.pass0);

	XMMATRIX mTrans, mScale, mRotX, mRotY, mRotZ, mAnim;
	for (int i = 0; i < cfgParser->objectsData.size(); ++i) {

		// Mesh Rotation Matrices	
		mRotX = XMMatrixRotationX(DEG2RAD(cfgParser->objectsData[i]->rotX));
		mRotY = XMMatrixRotationY(DEG2RAD(cfgParser->objectsData[i]->rotY));
		mRotZ = XMMatrixRotationZ(DEG2RAD(cfgParser->objectsData[i]->rotZ));

		// tansform + scale matrice
		mTrans = XMMatrixTranslation(cfgParser->objectsData[i]->posX, cfgParser->objectsData[i]->posY, cfgParser->objectsData[i]->posZ);
		mScale = XMMatrixScaling(cfgParser->objectsData[i]->scale, cfgParser->objectsData[i]->scale, cfgParser->objectsData[i]->scale);


		if (cfgParser->objectsData[i]->type == "CockpitObject"){
			g_terrainWorld = mScale * mRotX * mRotY * mRotZ * mTrans * g_camera.GetWorldMatrix();
		}
		else if (cfgParser->objectsData[i]->type == "GroundObject"){
			g_terrainWorld = mScale * mRotX * mRotY * mRotZ * mTrans;
		}

		worldViewProj = g_terrainWorld * g_camera.GetViewMatrix() * g_camera.GetProjMatrix();
		worldNormalsMatrix = XMMatrixInverse(NULL, g_terrainWorld);
		worldNormalsMatrix = XMMatrixTranspose(worldNormalsMatrix);


		// Bind shader variables for Cockpit
		V(g_gameEffect.worldEV->SetMatrix((float*)&g_terrainWorld));
		V(g_gameEffect.worldViewProjectionEV->SetMatrix((float*)&worldViewProj));
		V(g_gameEffect.worldNormalsMatrix->SetMatrix((float*)&worldNormalsMatrix));
		V(g_gameEffect.cameraPosWorldEV->SetFloatVector((float*)&g_camera.GetEyePt()));

		g_Meshes[cfgParser->objectsData[i]->name]->render(pd3dImmediateContext, g_gameEffect.meshPass1, g_gameEffect.diffuseEV, g_gameEffect.specularEV, g_gameEffect.glowEV);
	}

	for (auto it = g_enemyInstances.begin(); it 
		
		
		
		!= g_enemyInstances.end(); it++){
		// Mesh Rotation Matrices	
		mRotX = XMMatrixRotationX(DEG2RAD(cfgParser->objectsEnemyData[(*it)->typeName]->transform.rotX));
		mRotY = XMMatrixRotationY(DEG2RAD(cfgParser->objectsEnemyData[(*it)->typeName]->transform.rotY));
		mRotZ = XMMatrixRotationZ(DEG2RAD(cfgParser->objectsEnemyData[(*it)->typeName]->transform.rotZ));

		// tansform + scale matrice
		mTrans = XMMatrixTranslation(cfgParser->objectsEnemyData[(*it)->typeName]->transform.posX, cfgParser->objectsEnemyData[(*it)->typeName]->transform.posY, cfgParser->objectsEnemyData[(*it)->typeName]->transform.posZ);
		mScale = XMMatrixScaling(cfgParser->objectsEnemyData[(*it)->typeName]->transform.scale, cfgParser->objectsEnemyData[(*it)->typeName]->transform.scale, cfgParser->objectsEnemyData[(*it)->typeName]->transform.scale);
	
		XMVECTOR tmp = XMLoadFloat3(&XMFLOAT3(cfgParser->objectsEnemyData[(*it)->typeName]->transform.posX, cfgParser->objectsEnemyData[(*it)->typeName]->transform.posY, cfgParser->objectsEnemyData[(*it)->typeName]->transform.posZ));
		tmp = tmp - XMLoadFloat3(&(*it)->position);
		XMVECTOR d = XMVector3Normalize(XMLoadFloat3(&(*it)->velocity));
		float a = std::atan2(XMVectorGetX(d), XMVectorGetZ(d));

		mAnim = XMMatrixRotationY(a) * XMMatrixTranslationFromVector(tmp);

		g_terrainWorld = mScale * mRotX * mRotY * mRotZ * mTrans * mAnim;  //g_camera.GetWorldMatrix();
		worldViewProj = g_terrainWorld * g_camera.GetViewMatrix() * g_camera.GetProjMatrix();

		worldNormalsMatrix = XMMatrixInverse(NULL, g_terrainWorld);
		worldNormalsMatrix = XMMatrixTranspose(worldNormalsMatrix);

		// Bind shader variables for Enemys
		V(g_gameEffect.worldEV->SetMatrix((float*)&g_terrainWorld));
		V(g_gameEffect.worldViewProjectionEV->SetMatrix((float*)&worldViewProj));
		V(g_gameEffect.worldNormalsMatrix->SetMatrix((float*)&worldNormalsMatrix));
		V(g_gameEffect.cameraPosWorldEV->SetFloatVector((float*)&g_camera.GetEyePt())); 
		g_Meshes[cfgParser->objectsEnemyData[(*it)->typeName]->transform.name]->render(pd3dImmediateContext, g_gameEffect.meshPass1, g_gameEffect.diffuseEV, g_gameEffect.specularEV, g_gameEffect.glowEV);
	}

	if (spritesVector.size() > 0)
	{
		g_SpriteRenderer->renderSprites(pd3dImmediateContext, spritesVector, g_camera);
	}
	

	DXUT_BeginPerfEvent(DXUT_PERFEVENTCOLOR, L"HUD / Stats");
	V(g_hud.OnRender(fElapsedTime));
	V(g_sampleUI.OnRender(fElapsedTime));
	RenderText();
	DXUT_EndPerfEvent();

	static DWORD dwTimefirst = GetTickCount();
	if (GetTickCount() - dwTimefirst > 5000)
	{
		OutputDebugString(DXUTGetFrameStats(DXUTIsVsyncEnabled()));
		OutputDebugString(L"\n");
		dwTimefirst = GetTickCount();
	}
}

void DeinitApp()
{
	SAFE_DELETE(g_SpriteRenderer);
	SAFE_DELETE(cfgParser);
	for (auto iterator = g_Meshes.begin(); iterator != g_Meshes.end(); iterator++) 
	{
		SAFE_DELETE(g_Meshes[iterator->first]);
	}
	for (auto it = g_enemyInstances.begin(); it != g_enemyInstances.end(); it++)
	{
		SAFE_DELETE((*it));
	}
}

boolean SpriteSort(const SpriteVertex &s1, const SpriteVertex &s2)
{
	return (s1.distanceToCam < s2.distanceToCam);
}
