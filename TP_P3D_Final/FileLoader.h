#pragma once
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#include <iostream>
#include <vector>
#include<string>
#include<fstream>
#include<sstream>

#define GLEW_STATIC
#include <GL\glew.h>

#include <GLFW\glfw3.h>
#include <glm/glm/glm.hpp> 
#include <glm/glm/gtc/matrix_transform.hpp> 
#include <glm/glm/gtc/type_ptr.hpp> 

// structer from the model to link the info from the OBJ
struct Model_Atributes
{
	std::vector<glm::vec3> vboV;
	std::vector<glm::vec2> vboVT;
	std::vector<glm::vec3> vboVN;
	std::string MTLFile;
	int countVertexPos = 0;
	int countNormals = 0;
	int countVertexTexture = 0;
	int countFaces = 0;
};

//Struct for the lights atributes from MTL
struct Lights
{
	glm::vec3 Ka;
	glm::vec3 kd;
	glm::vec3 ks;
	float Ns;
	float Ni;
	std::string string;
};


class Read
{
public:
	Read(const char* fileOBJ, const char* fileMTL);
	//mtl
	Lights loadMTL(const char* fileMTL);//get info from OBJ

	//obj
	Model_Atributes LoadOBJ(const char* file1);//Get info from mtl
	Model_Atributes atributesOBJ;
	Lights atributesLights;

};

//Construtor
Read::Read(const char* fileOBJ, const char* fileMTL)
{
	//Guardar atributos
	atributesOBJ = Read::LoadOBJ(fileOBJ);
	atributesLights = Read::loadMTL(fileMTL);
}

Model_Atributes Read::LoadOBJ(const char* fileOBJ)
{
	//Variables
	Model_Atributes tempAtribute;
	Model_Atributes atribute;

	std::ifstream myFile(fileOBJ);//ifstream para ler o ficheiro
	std::string currentLine = "";
	std::string firstLetter = "";
	std::string MTLFile;
	std::stringstream tempString;
	glm::vec3  vec3Temp;
	int countVertexPos = 0;
	int countNormals = 0;
	glm::vec2  vec2Temp;
	int countVertexTexture = 0;
	int countFaces = 0;
	int v1, vT1, vN1 = 0;
	int v2, vT2, vN2 = 0;
	int v3, vT3, vN3 = 0;
	char tempChar;

	if (!myFile.is_open())
	{
		std::cout << "ERROR! File couldnt be open." << std::endl;
	}

	//Get one line at the time 
	//we are using the getline strategy
	while (std::getline(myFile, currentLine))
	{
		tempString.clear();
		tempString.str(currentLine);
		tempString >> firstLetter;

		// se a primeira letra for v
		if (firstLetter == "v") //vertex pos
		{
			tempString >> vec3Temp.x >> vec3Temp.y >> vec3Temp.z;
			countVertexPos++;
			tempAtribute.vboV.push_back(vec3Temp);

		}
		else if (firstLetter == "vt") //vertex texture
		{
			tempString >> vec2Temp.x >> vec2Temp.y;
			countVertexTexture++;
			tempAtribute.vboVT.push_back(vec2Temp);
		}
		else if (firstLetter == "vn") //vertex normal
		{
			tempString >> vec3Temp.x >> vec3Temp.y >> vec3Temp.z;
			countNormals++;
			tempAtribute.vboVN.push_back(vec3Temp);
		}
		else if (firstLetter == "mtllib")
		{
			tempString >> MTLFile;
			atribute.MTLFile = MTLFile;
			//std::cout << "MTL-" << MTLFile << "\n";
		}
		else if (firstLetter == "f") //vertices
		{
			//get info from the vertex/texture/ normal index 
			tempString >> v1 >> tempChar >> vT1 >> tempChar >> vN1 >> v2 >> tempChar >> vT2 >> tempChar >> vN2 >> v3 >> tempChar >> vT3 >> tempChar >> vN3;
			//std::cout << "teste faces " << v1 << "/" << vT1 << "/" << vN1 << " " << v2 << "/" << vT2 << "/" << vN2 << " " << v3 << "/" << vT3 << "/" << vN3 <<"\n";

			//First vertex
			atribute.vboV.push_back(tempAtribute.vboV[v1 - 1]);
			atribute.vboVT.push_back(tempAtribute.vboVT[vT1 - 1]);
			atribute.vboVN.push_back(tempAtribute.vboVN[vN1 - 1]);

			//Second vertex
			atribute.vboV.push_back(tempAtribute.vboV[v2 - 1]);
			atribute.vboVT.push_back(tempAtribute.vboVT[vT2 - 1]);
			atribute.vboVN.push_back(tempAtribute.vboVN[vN2 - 1]);

			//Third vertex
			atribute.vboV.push_back(tempAtribute.vboV[v3 - 1]);
			atribute.vboVT.push_back(tempAtribute.vboVT[vT3 - 1]);
			atribute.vboVN.push_back(tempAtribute.vboVN[vN3 - 1]);

			// debug info inside atrib 
			//std::cout << "teste atrib " << tempAtrib.vboV[v1].x << "/" << tempAtrib.vboVT[vT1].x << "/" << tempAtrib.vboVN[vN1].x << " " << v2 << "/" << vT2 << "/" << vN2 << " " << v3 << "/" << vT3 << "/" << vN3 << "\n";

			//count vertex from faces 
			countFaces += 3;
		}
	}

	atribute.countVertexPos = countVertexPos;
	atribute.countVertexTexture = countVertexTexture;
	atribute.countNormals = countNormals;
	atribute.countFaces = countFaces;


	std::cout << "count V -" << atribute.countVertexPos << "\n";
	std::cout << "count VT -" << atribute.countVertexTexture << "\n";
	std::cout << "count VN -" << atribute.countNormals << "\n";
	std::cout << "count F -" << atribute.countFaces << "\n"; //vertices

	return atribute;
}

Lights Read::loadMTL(const char* fileMTL)
{
	std::stringstream temporaryString;
	std::ifstream mtlFile(fileMTL);
	std::string line = "";
	std::string letter = "";
	std::string TAGMap;
	glm::vec3  vec3Temp;
	float temporario;

	Lights atribute;

	if (!mtlFile.is_open())
	{
		std::cout << "Nao foi possivel abrir o ficheiro" << std::endl;
	}

	//ler uma linha de cada vez
	while (std::getline(mtlFile, line))
	{
		//obter a primeira letra de cada linha
		temporaryString.clear();
		temporaryString.str(line);
		temporaryString >> letter;
		//Leitura dos coeficientes da superficie do objecto
		if (letter == "Ka") // coeficiente de reflexão ambiente 
		{
			temporaryString >> vec3Temp.x >> vec3Temp.y >> vec3Temp.z;
			atribute.Ka = vec3Temp;
		}
		if (letter == "Kd") // coeficiente da reflexão difusa
		{
			temporaryString >> vec3Temp.x >> vec3Temp.y >> vec3Temp.z;
			atribute.kd = vec3Temp;
		}
		else if (letter == "Ks") // coeficiente de reflexão especular 
		{
			temporaryString >> vec3Temp.x >> vec3Temp.y >> vec3Temp.z;
			atribute.ks = vec3Temp;
		}
		else if (letter == "Ns") // 
		{
			temporaryString >> temporario;
			atribute.Ns = temporario;

		}
		else if (letter == "Ni")
		{
			temporaryString >> temporario;
			atribute.Ni = temporario;

		}
		else if (letter == "map_Kd")
		{
			temporaryString >> TAGMap;
			atribute.string = TAGMap;

		}

	}

	return atribute;

}

