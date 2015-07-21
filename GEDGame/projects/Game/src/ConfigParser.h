#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <DirectXMath.h>
#include <vector>
#include "Vec3f.h"

struct MeshFiles{

	std::string modelPath;
	std::string diffuseTexturePath;
	std::string specularTexturePath;
	std::string glowTexturePath;
	std::string normalTexturePath;
};

struct TransformData
{
	std::string type;
	std::string name;
	float scale = 0;
	float rotX = 0;		// Use Vec3f????
	float rotY = 0;
	float rotZ = 0;

	float posX = 0;
	float posY = 0;
	float posZ = 0;

};

struct ProjectileData
{
	std::string textureName;
	float speed = 0.0f;
	float firerate = 0.0f;
	float damage = 0.0f;
	float gravity = 0.0f;
	float radius = 0.0f;

	float posX = 0;
	float posY = 0;
	float posZ = 0;
};

struct EnemyData
{
	int hitpoints;
	float size;
	float speed;

	std::string enemyType;

	TransformData transform;
};

struct ExplosionData
{
	std::string explosionName;
	std::string fileName;
	int texIndex;
	float duration;
};

class ConfigParser{
public:
	ConfigParser();
	~ConfigParser();

	void load(std::string filepath);

	//Getter
	float getSpinning();
	float getSpinSpeed();
	std::string getTerrainPath();
	float getTerrainWidth();
	float getTerrainDepth();
	float getTerrainHeight();
	float getIntervall();
	float getMinSpawn();
	float getMaxSpawn();
	std::string getTerrainTexturePath();
	std::string getTerrainNormalPath();
	MeshFiles* getCockpitMeshFiles();
	std::map<std::string, MeshFiles*>		meshPathes;
	std::vector<TransformData*>				objectsData;
	std::map<std::string, EnemyData*>		objectsEnemyData;
	std::map<std::string, ProjectileData*>	projectileData;
	std::map<std::string, ExplosionData*>	explosionData;

private:
	std::string terrainHeightPath, terrainColorPath, terrainNormalPath;
	std::string terrainPath;
	float terrainWidth;
	float terrainDepth;
	float terrainHeight;
	float spinSpeed;
	float spawnInteravall;
	float spawnMinHeight;
	float spawMaxHeight;
};

