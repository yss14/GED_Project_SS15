#include "Terrain.h"
#include "GameEffect.h"
#include <algorithm>
//#include "SimpleImage/SimpleImage.h"
#include <DDSTextureLoader.h>
#include "DirectXTex.h"

// You can use this macro to access your height field
#define IDX(X,Y,WIDTH) ((X) + (Y) * (WIDTH))
std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	std::wstring r(len, L'\0');
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, &r[0], len);
	return r;
}

Terrain::Terrain(void) :
indexBuffer(nullptr),
vertexBuffer(nullptr),
diffuseTexture(nullptr),
diffuseTextureSRV(nullptr),
debugSRV(nullptr)
{
}


Terrain::~Terrain(void)
{
}



HRESULT Terrain::create(ID3D11Device* device, ConfigParser* parser)
{
	HRESULT hr;

	// In our example, we load a debug texture

	// Loading the heightMap
	std::wstring outDir(TARGET_DIRECTORY);
	outDir += L"resources\\";
	std::wstring tmp = s2ws(parser->getTerrainPath());
	//std::wstring tmp(parser->getTerrainPath().begin(), parser->getTerrainPath().end());
	outDir += tmp;
	GEDUtils::SimpleImage heightMap(outDir.c_str());

	outDir = TARGET_DIRECTORY;
	outDir += L"resources\\";
	tmp = s2ws(parser->getTerrainTexturePath());
	outDir += tmp;
	// Loading Terrain Texture
	V(DirectX::CreateDDSTextureFromFile(device, outDir.c_str() , nullptr, &diffuseTextureSRV));

	if (hr != S_OK) {
		MessageBoxA(NULL, "Could not load Terrain Texture", "Invalid texture", MB_ICONERROR | MB_OK);
		return hr;
	}
	
	//std::wcout << "Path: " << outDir << std::endl;
	
	// Load Debug Texture
	/*outDir = TARGET_DIRECTORY;
	outDir += L"resources\\debug_green.dds";
	V(DirectX::CreateDDSTextureFromFile(device,outDir.c_str(), nullptr, &diffuseTexture));*/

	//if (hr != S_OK) {
	//	MessageBoxA(NULL, "Could not load texture \"resources\\debug_green.dds\"", "Invalid texture", MB_ICONERROR | MB_OK);
	//	return hr;
	//}

	// TODO: Replace this vertex array (triangle) with an array (or vector)
	// which contains the vertices of your terrain. Calculate position,
	// normal and texture coordinates according to your height field and
	// the dimensions of the terrain specified by the ConfigParser

	// Note 1: The normal map that you created last week will not be used
	// in this assignment (Assignment 4). It will be of use in later assignments

	// Note 2: For each vertex 10 floats are stored. Do not use another
	// layout.

	// Note 3: In the coordinate system of the vertex buffer (output):
	// x = east,    y = up,    z = south,          x,y,z in [0,1] (float)

	indexLength = (heightMap.getWidth() - 1) * (heightMap.getHeight() - 1) * 6;
	std::vector<SimpleVertex> vertexVector(heightMap.getWidth() * heightMap.getHeight(), {});
	std::vector<float> vertexFloatVector(heightMap.getWidth() * heightMap.getHeight() * 10, 0.0f);
	std::vector<unsigned int> indexVector(indexLength,0);

	D3D11_SUBRESOURCE_DATA id;
	id.pSysMem = &vertexFloatVector[0];
	id.SysMemPitch = 10 * sizeof(float); // Stride
	id.SysMemSlicePitch = 0;

	// Vertex buffer init
	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//bd.ByteWidth = vertexFloatVector.size() * sizeof(char*);//The size in bytes of the pointer array of the heightmap
	bd.ByteWidth = vertexFloatVector.size() * sizeof(float);
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;

	// index buffer init stuff

	D3D11_SUBRESOURCE_DATA idi;
	ZeroMemory(&idi, sizeof(idi));
	idi.pSysMem = &indexVector[0];
	
 	D3D11_BUFFER_DESC bdi;
	ZeroMemory(&bdi, sizeof(bdi));
	bdi.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bdi.ByteWidth = indexLength * sizeof(unsigned int);//The size in bytes of the pointer array of the heightmap
	bdi.CPUAccessFlags = 0;
	bdi.MiscFlags = 0;
	bdi.Usage = D3D11_USAGE_DEFAULT;




	int indexCnt = 0;
	int vertexCnt = 0;
	int width = heightMap.getWidth();
	int height = heightMap.getHeight();

	heightMapVector = std::vector<float>(height*width);
	for (int z = 0; z < height; z++){
		for (int x = 0; x < width; x++){
			heightMapVector[IDX(x, z, width)] = heightMap.getPixel(x, z);
		}
	}

//	#echo "Creating new resources..."
//	#mkdir "$(OutDir)resources"
//#"$(OutDir)TerrainGenerator.exe" -r 4096 -o_height "$(OutDir)resources\terrain_height.tiff" -o_color "$(IntDir)terrain_color.tiff" -o_normal "$(IntDir)terrain_normal.tiff"
//#"$(OutDir)texconv.exe" -srgbi -f R8G8B8A8_UNORM_SRGB -o "$(OutDir)resources" "$(IntDir)terrain_color.tiff"
//#"$(OutDir)texconv.exe" -f BC5_UNORM -o "$(OutDir)resources" "$(IntDir)terrain_normal.tiff"
//#"$(OutDir)texconv" -o "$(OutDir)resources" -srgbi -f R8G8B8A8_UNORM_SRGB "..\..\..\..\external\textures\debug_green.jpg"

	for (int z = 0; z < height; z++){
		for (int x = 0; x < width; x++){
			SimpleVertex tmp;
			tmp.Pos.x = x * 4;
			tmp.Pos.z = z * 4;
			tmp.Pos.y = heightMap.getPixel(x, z);
			tmp.Pos.w = 1.0f;

			tmp.Normal.x = 0.0f;
			tmp.Normal.z = 0.0f;
			tmp.Normal.y = 1.0f;

			tmp.UV.x = (x*1.0f) / width;
			tmp.UV.y = (z*1.0f) / height;

			vertexFloatVector[vertexCnt++] = x * 4;
			vertexFloatVector[vertexCnt++] = heightMapVector[IDX(x, z, width)] * (2*width);
			vertexFloatVector[vertexCnt++] = z * 4;
			vertexFloatVector[vertexCnt++] = 1.0f;

			Vec3f tmpNormal = calculateNormal(x, z, width);
			vertexFloatVector[vertexCnt++] = tmpNormal.x;
			vertexFloatVector[vertexCnt++] = tmpNormal.y;
			vertexFloatVector[vertexCnt++] = tmpNormal.z;
			vertexFloatVector[vertexCnt++] = 0.0f;

			vertexFloatVector[vertexCnt++] = (x) / (width-1.0f);
			vertexFloatVector[vertexCnt++] = (z) / (height-1.0f);

			// doesn't have to add triangles for the  last row/col
			if (x < width - 1 && z < height - 1){

				// first triangle
				indexVector[indexCnt++] = IDX(x, z, width);
				indexVector[indexCnt++] = IDX(x + 1, z, width);
				indexVector[indexCnt++] = IDX(x, z + 1, width);

				// second triangle
				indexVector[indexCnt++] = IDX(x + 1, z, width);
				indexVector[indexCnt++] = IDX(x + 1, z + 1, width);
				indexVector[indexCnt++] = IDX(x, z + 1, width);
			}
		}
	}
	//float triangle[] = {
	//	// Vertex 0
	//	-400.0f, 0.0f, -400.0f, 1.0f, // Position
	//	0.0f, 1.0f, 0.0f, 0.0f, // Normal
	//	0.0f, 0.0f,                 // Texcoords

	//	// Vertex 1
	//	400.0f, 0.0f, -400.0f, 1.0f, // Position
	//	0.0f, 1.0f, 0.0f, 0.0f, // Normal
	//	0.0f, 1.0f,                // Texcoords

	//	// Vertex 2
	//	-400.0f, 0.0f, 400.0f, 1.0f, // Position
	//	0.0f, 1.0f, 0.0f, 0.0f, // Normal
	//	1.0f, 0.0f,                 // Texcoords
	//};

	//vertexFloatVector.assign(triangle, triangle + 30);
	//int indices[] = {2,1,0};
	//indexVector.assign(indices, indices + 3);
	// Create index buffer
	// TODO: Insert your code to create the index buffer
	V(device->CreateBuffer(&bdi, &idi, &indexBuffer));
	V(device->CreateBuffer(&bd, &id, &vertexBuffer)); // http://msdn.microsoft.com/en-us/library/ff476899%28v=vs.85%29.aspx

	// Load color texture (color map)
	// TODO: Insert your code to load the color texture and create
	// the texture "diffuseTexture" as well as the shader resource view
	// "diffuseTextureSRV"

	return hr;
}


void Terrain::destroy()
{
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(debugSRV);
	SAFE_RELEASE(diffuseTextureSRV);
	// TODO: Release the terrain's shader resource view and texture
}


void Terrain::render(ID3D11DeviceContext* context, ID3DX11EffectPass* pass)
{
	HRESULT hr;

	// Bind the terrain vertex buffer to the input assembler stage 
	ID3D11Buffer* vbs[] = { vertexBuffer, };
	unsigned int strides[] = { 10 * sizeof(float), }, offsets[] = { 0, };
	context->IASetVertexBuffers(0, 1, vbs, strides, offsets);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Tell the input assembler stage which primitive topology to use
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// TODO: Bind the SRV of the terrain diffuse texture to the effect variable
	// (instead of the SRV of the debug texture)
	V(g_gameEffect.diffuseEV->SetResource(diffuseTextureSRV));

	// Apply the rendering pass in order to submit the necessary render state changes to the device
	V(pass->Apply(0, context));

	// Draw
	// TODO: Use DrawIndexed to draw the terrain geometry using as shared vertex list
	// (instead of drawing only the vertex buffer)
	context->DrawIndexed(indexLength , 0, 0);
	//context->Draw(3, 0);
}

Vec3f Terrain::calculateNormal(int x, int z, int resolution){

	Vec3f normalAtCurPoint(0.0f,0.0f,0.0f);
	Vec3f vecXAxes(0.0f,0.0f,0.0f);
	Vec3f vecYAxes(0.0f, 0.0f, 0.0f);

	normalAtCurPoint.x = normalAtCurPoint.y = normalAtCurPoint.z = 0;
	vecXAxes.x = 1.0f;
	vecXAxes.y = 0.0f;
	vecYAxes.x = 0.0f;
	vecYAxes.y = 1.0f;

	//Central Differecnes
	if (x == 0)
	{
		//Forward Differences
		vecXAxes.z = (heightMapVector[IDX(x + 1, z, resolution)] - heightMapVector[IDX(x, z, resolution)]);
	}
	else if (x == resolution - 1)
	{
		//Backward Differences
		vecXAxes.z = (heightMapVector[IDX(x, z, resolution)] - heightMapVector[IDX(x - 1, z, resolution)]);
	}
	else
	{
		//Central Differences
		vecXAxes.z = (heightMapVector[IDX(x + 1, z, resolution)] - heightMapVector[IDX(x - 1, z, resolution)]) / 2.0f;
	}

	if (z == 0)
	{
		//Forward Differences
		vecYAxes.z = (heightMapVector[IDX(x, z + 1, resolution)] - heightMapVector[IDX(x, z, resolution)]);
	}
	else if (z == resolution - 1)
	{
		//Backward Differences
		vecYAxes.z = (heightMapVector[IDX(x, z, resolution)] - heightMapVector[IDX(x, z - 1, resolution)]);
	}
	else
	{
		//Central Differences
		vecYAxes.z = (heightMapVector[IDX(x, z + 1, resolution)] - heightMapVector[IDX(x, z - 1, resolution)]) / 2.0f;
	}

	normalAtCurPoint = vecXAxes.cross(vecYAxes);

	//Rescaling because of different dimensions
	normalAtCurPoint.x *= resolution;
	normalAtCurPoint.y *= resolution;

	normalAtCurPoint.normalize();

	return normalAtCurPoint;
}

