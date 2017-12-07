#include "MyOctant.h"
using namespace Simplex;

uint MyOctant::m_uOctantCount = 0;
uint MyOctant::m_uMaxLevel = 3;
uint MyOctant::m_uIdealEntityCount = 5; 

void Simplex::MyOctant::Init(void)
{
	m_uChildren = 0;
	m_fSize = 0.0f;
	m_uID = m_uOctantCount;
	m_uLevel = 0;
	m_v3Center = vector3(0.0f); 
	m_v3Min = vector3(0.0f);
	m_v3Max = vector3(0.0f);
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();
	m_pRoot = nullptr;
	m_pParent = nullptr;
	for (int i = 0; i < 8; i++) {
		m_pChild[i] = nullptr;
	}
}

void Simplex::MyOctant::Release(void)
{
	if (m_uLevel == 0) {
		KillBranches();
	}
	m_uChildren = 0;
	m_fSize = 0.0f;
	ClearEntityList();
	m_lChild.clear();
}

Simplex::MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	Init();

	m_uOctantCount = 0;
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
	m_uID = m_uOctantCount;
	m_pRoot = this;
	m_lChild.clear();

	std::vector<vector3> minMax;

	uint nObjects = m_pEntityMngr->GetEntityCount();
	for (uint i = 0; i < nObjects; i++) {
		MyEntity* pEntity = m_pEntityMngr->GetEntity(i);
		MyRigidBody* pRigidBody = pEntity->GetRigidBody();
		minMax.push_back(pRigidBody->GetMinGlobal());
		minMax.push_back(pRigidBody->GetMaxGlobal());
	}
	MyRigidBody* pRigidBody = new MyRigidBody(minMax);

	vector3 vHalfWidth = pRigidBody->GetHalfWidth();
	float fMax = 0.0f;
	for (int i = 0; i < 3; i++) {
		if (fMax < vHalfWidth[i]) {
			fMax = vHalfWidth[i];
		}
	}
	vector3 v3Center = pRigidBody->GetCenterLocal();
	m_fSize = fMax * 2.0f;
	m_v3Center = v3Center;
	m_v3Min = m_v3Center - (vector3(fMax));
	m_v3Max = m_v3Center + (vector3(fMax));

	m_uOctantCount++;

	ConstructTree(m_uMaxLevel);
}

Simplex::MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	Init();
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;

	m_v3Min = m_v3Center - (vector3(m_fSize) / 2.0f);
	m_v3Max = m_v3Center + (vector3(m_fSize) / 2.0f);

	m_uOctantCount++;
}

Simplex::MyOctant::MyOctant(MyOctant const & other)
{
	m_uChildren = other.m_uChildren;
	m_fSize = other.m_fSize;

	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;

	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;
	m_pRoot = other.m_pRoot;
	m_pParent = other.m_pParent;
	m_lChild = other.m_lChild;

	//No need to swap the singletons.
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	for (int i = 0; i < 8; i++) {
		m_pChild[i] = other.m_pChild[i];
	}
}

MyOctant& MyOctant::operator=(MyOctant const & other)
{
	if (this != &other) {
		Release();
		Init();
		MyOctant temp = MyOctant(other);
		Swap(temp);
	}
	return *this;
}

Simplex::MyOctant::~MyOctant(void)
{
	Release();
}


void Simplex::MyOctant::Swap(MyOctant & other)
{
	//std::swap changes the contents of 2 arguments, so I just did that a bunch to switch the contents.
	std::swap(m_uChildren, other.m_uChildren);
	std::swap(m_fSize, other.m_fSize);
	std::swap(m_uID, other.m_uID);
	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);
	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_lChild, other.m_lChild);
	std::swap(m_pParent, other.m_pParent);

	//Again with the lack of switching singletons
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	for (int i = 0; i < 8; i++) {
		std::swap(m_pChild[i], other.m_pChild[i]);
	}
}

//Here's some accessors
float Simplex::MyOctant::GetSize(void)
{
	return m_fSize;
}

vector3 Simplex::MyOctant::GetCenterGlobal(void)
{
	return m_v3Center;
}

vector3 Simplex::MyOctant::GetMinGlobal(void)
{
	return m_v3Min;
}

vector3 Simplex::MyOctant::GetMaxGlobal(void)
{
	return m_v3Max;
}

uint Simplex::MyOctant::GetOctantCount(void)
{
	return m_uOctantCount;
}

//Collision checking method
bool Simplex::MyOctant::IsColliding(uint a_uRBIndex)
{
	//If the specified index is too big, exit out of method.
	uint objectCount = m_pEntityMngr->GetEntityCount();
	if (a_uRBIndex >= objectCount) {
		return false;
	}
	//Get the entity at the index, and get its rigid body coordinates.
	MyEntity* testEntity = m_pEntityMngr->GetEntity(a_uRBIndex);
	MyRigidBody* testRigidBody = testEntity->GetRigidBody();
	vector3 v3Min = testRigidBody->GetMinGlobal();
	vector3 v3Max = testRigidBody->GetMaxGlobal();

	//AABB Collision checking, because the objects are not rotating at all.

	//X check
	if (m_v3Max.x < v3Min.x) {
		return false;
	}
	if (m_v3Min.x > v3Max.x) {
		return false;
	}

	//Y check
	if (m_v3Max.y < v3Min.y) {
		return false;
	}
	if (m_v3Min.y > v3Max.y) {
		return false;
	}

	//Z check
	if (m_v3Max.z < v3Min.z) {
		return false;
	}
	if (m_v3Min.z > v3Max.z) {
		return false;
	}
	return true;
}

//Display methods for displaying the wire frame of the subdivisions.
//Recursive methods that keep checking for valid children to display the wire frames.
void Simplex::MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
	//Matching ID = displayed wire frame, and exits out of the method.
	if (m_uID == a_nIndex) {
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
		return;
	}
	//Otherwise, make each of this node's children try to display.
	for (uint i = 0; i < m_uChildren; i++) {
		m_pChild[i]->Display(a_nIndex);
	}
}

//This display method keeps looping until the nodes no longer have children.
void Simplex::MyOctant::Display(vector3 a_v3Color)
{
	for (uint i = 0; i < m_uChildren; i++) {
		m_pChild[i]->Display(a_v3Color);
	}
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}

//Gets rid of contents of the node's entity list.
void Simplex::MyOctant::ClearEntityList(void)
{
	for (int i = 0; i < m_uChildren; i++) {
		m_pChild[i]->ClearEntityList();
	}
	m_EntityList.clear();
}

void Simplex::MyOctant::Subdivide(void)
{
	//Don't subdivide if it's past the max level, or if the node already has children via subdivision
	if (m_uLevel >= m_uMaxLevel || m_uChildren != 0) {
		return;
	}
	m_uChildren = 8;

	float fSize = m_fSize / 4.0f; //Gets the half length of the subdivisions
	float doubledSize = fSize * 2.0f; //Get double the calculated size to get the max size of the subdivision 
	vector3 newCenter = m_v3Center; //Will alter this to calculate new centers for subdivisions

	//Initializing subdivisions
	//First subdivision: Bottom left back
	newCenter.x -= fSize;
	newCenter.y -= fSize;
	newCenter.z -= fSize;
	m_pChild[0] = new MyOctant(newCenter, doubledSize);

	//Second subdivision: Bottom left front
	newCenter.z += doubledSize;
	m_pChild[1] = new MyOctant(newCenter, doubledSize);

	//Third subdivision: Bottom right front
	newCenter.x += doubledSize;
	m_pChild[2] = new MyOctant(newCenter, doubledSize);

	//Fourth subdivision: Bottom right back
	newCenter.z -= doubledSize;
	m_pChild[3] = new MyOctant(newCenter, doubledSize);

	//Fifth subdivision: top right back
	newCenter.y += doubledSize;
	m_pChild[4] = new MyOctant(newCenter, doubledSize);

	//Sixth subdivision: top left back
	newCenter.x -= doubledSize;
	m_pChild[5] = new MyOctant(newCenter, doubledSize);

	//Seventh subdivision: top left front
	newCenter.z += doubledSize;
	m_pChild[6] = new MyOctant(newCenter, doubledSize);

	//Eight subdivision: top right front
	newCenter.x += doubledSize;
	m_pChild[7] = new MyOctant(newCenter, doubledSize);

	//For loop to assign parameters to subdivisions.
	for (int i = 0; i < 8; i++) {
		m_pChild[i]->m_pRoot = m_pRoot;
		m_pChild[i]->m_pParent = this;
		m_pChild[i]->m_uLevel = m_uLevel + 1;
		//If there are more objects than the ideal count, subdivide again.
		if (m_pChild[i]->ContainsMoreThan(m_uIdealEntityCount)) {
			m_pChild[i]->Subdivide();
		}
	}
}

MyOctant * Simplex::MyOctant::GetChild(uint a_nChild)
{
	if (a_nChild > 7) {
		return nullptr;
	}
	return m_pChild[a_nChild];
}

MyOctant * Simplex::MyOctant::GetParent(void)
{
	return m_pParent;
}

//If more than the alloted amount of entities are within the subdivision, return true.
//Used to check if a node is allowed to subdivide again.
bool Simplex::MyOctant::ContainsMoreThan(uint a_nEntities)
{
	int myCount = 0;
	int entityCount = m_pEntityMngr->GetEntityCount();
	for (int i = 0; i < entityCount; i++) {
		if (IsColliding(i)) {
			myCount++;
		}
	}
	if (myCount > a_nEntities) {
		return true;
	}
	return false;
}

//Recursively call the method for the children, so they start deleting their own children too.
//It'll keep going until it reaches a node with no children, then it'll go back to the parent.
void Simplex::MyOctant::KillBranches(void)
{
	for (int i = 0; i < m_uChildren; i++) {
		m_pChild[i]->KillBranches();
		SafeDelete(m_pChild[i]);
		m_pChild[i] = nullptr;
	}
	m_uChildren = 0;
}

void Simplex::MyOctant::ConstructTree(uint a_nMaxLevel)
{
	//Only the root should be able to construct a tree
	if (m_uLevel != 0) {
		return;
	}
	m_uMaxLevel = a_nMaxLevel;

	m_uOctantCount = 1;
	
	//If the section has more entities than ideal
	if (ContainsMoreThan(m_uIdealEntityCount)) {
		//Call subdivision. But, it will only subdivide if the maxlevel hasn't been reached yet.
		Subdivide();
	}
	//Afterwards, assign ids to the entities within the node, and add the new nodes to a list.
	AssignIDtoEntity();
	ConstructList();
}

//Traverses down the tree, and assigns ids to all entitiies.
void Simplex::MyOctant::AssignIDtoEntity(void)
{
	for (int i = 0; i < m_uChildren; i++) {
		m_pChild[i]->AssignIDtoEntity();
	}
	if (m_uChildren == 0) {
		uint entities = m_pEntityMngr->GetEntityCount();
		for (int i = 0; i < entities; i++) {
			if (IsColliding(i)) {
				m_EntityList.push_back(i);
				m_pEntityMngr->AddDimension(i, m_uID);
			}
		}
	}
}

//Creates a list of all children that contain something.
void Simplex::MyOctant::ConstructList(void)
{
	for (int i = 0; i < m_uChildren; i++) {
		m_pChild[i]->ConstructList();
	}
	if (m_EntityList.size() > 0) {
		m_pRoot->m_lChild.push_back(this);
	}
}
