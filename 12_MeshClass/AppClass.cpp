#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	m_sProgrammer = "HanSoo Lee";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));

	//Make MyMesh object
	/*m_pMesh = new MyMesh();
	m_pMesh->GenerateCube(2.0f, C_BROWN);

	//Make MyMesh object
	m_pMesh1 = new MyMesh();
	m_pMesh1->GenerateCube(1.0f, C_WHITE);*/

	//Generating all of the MyMesh objects, and creating initial matrix4s
	for (int i = 0; i < 46; i++) {
		MeshVector.push_back(new MyMesh());
		MeshVector[i]->GenerateCube(1.0f, C_BLACK);
		m_m4Objects.push_back(glm::translate(0.0f, 0.0f, 0.0f));
	}

	//Generating all of the initial positions of the cubes. This is pretty tedious.
	m_m4Objects[0] = glm::translate(-7.0f, 7.0f, 0.0f);
	m_m4Objects[1] = glm::translate(-1.0f, 7.0f, 0.0f);
	m_m4Objects[2] = glm::translate(-6.0f, 6.0f, 0.0f);
	m_m4Objects[3] = glm::translate(-2.0f, 6.0f, 0.0f);
	m_m4Objects[4] = glm::translate(-7.0f, 5.0f, 0.0f);
	m_m4Objects[5] = glm::translate(-6.0f, 5.0f, 0.0f);
	m_m4Objects[6] = glm::translate(-5.0f, 5.0f, 0.0f);
	m_m4Objects[7] = glm::translate(-4.0f, 5.0f, 0.0f);
	m_m4Objects[8] = glm::translate(-3.0f, 5.0f, 0.0f);
	m_m4Objects[9] = glm::translate(-2.0f, 5.0f, 0.0f);
	m_m4Objects[10] = glm::translate(-1.0f, 5.0f, 0.0f);
	m_m4Objects[11] = glm::translate(-8.0f, 4.0f, 0.0f);
	m_m4Objects[12] = glm::translate(-7.0f, 4.0f, 0.0f);
	m_m4Objects[13] = glm::translate(-5.0f, 4.0f, 0.0f);
	m_m4Objects[14] = glm::translate(-4.0f, 4.0f, 0.0f);
	m_m4Objects[15] = glm::translate(-3.0f, 4.0f, 0.0f);
	m_m4Objects[16] = glm::translate(-1.0f, 4.0f, 0.0f);
	m_m4Objects[17] = glm::translate(0.0f, 4.0f, 0.0f);
	m_m4Objects[18] = glm::translate(-9.0f, 3.0f, 0.0f);
	m_m4Objects[19] = glm::translate(-8.0f, 3.0f, 0.0f);
	m_m4Objects[20] = glm::translate(-7.0f, 3.0f, 0.0f);
	m_m4Objects[21] = glm::translate(-6.0f, 3.0f, 0.0f);
	m_m4Objects[22] = glm::translate(-5.0f, 3.0f, 0.0f);
	m_m4Objects[23] = glm::translate(-4.0f, 3.0f, 0.0f);
	m_m4Objects[24] = glm::translate(-3.0f, 3.0f, 0.0f);
	m_m4Objects[25] = glm::translate(-2.0f, 3.0f, 0.0f);
	m_m4Objects[26] = glm::translate(-1.0f, 3.0f, 0.0f);
	m_m4Objects[27] = glm::translate(0.0f, 3.0f, 0.0f);
	m_m4Objects[28] = glm::translate(1.0f, 3.0f, 0.0f);
	m_m4Objects[29] = glm::translate(-9.0f, 2.0f, 0.0f);
	m_m4Objects[30] = glm::translate(-7.0f, 2.0f, 0.0f);
	m_m4Objects[31] = glm::translate(-6.0f, 2.0f, 0.0f);
	m_m4Objects[32] = glm::translate(-5.0f, 2.0f, 0.0f);
	m_m4Objects[33] = glm::translate(-4.0f, 2.0f, 0.0f);
	m_m4Objects[34] = glm::translate(-3.0f, 2.0f, 0.0f);
	m_m4Objects[35] = glm::translate(-2.0f, 2.0f, 0.0f);
	m_m4Objects[36] = glm::translate(-1.0f, 2.0f, 0.0f);
	m_m4Objects[37] = glm::translate(1.0f, 2.0f, 0.0f);
	m_m4Objects[38] = glm::translate(-9.0f, 1.0f, 0.0f);
	m_m4Objects[39] = glm::translate(-7.0f, 1.0f, 0.0f);
	m_m4Objects[40] = glm::translate(-1.0f, 1.0f, 0.0f);
	m_m4Objects[41] = glm::translate(1.0f, 1.0f, 0.0f);
	m_m4Objects[42] = glm::translate(-6.0f, 0.0f, 0.0f);
	m_m4Objects[43] = glm::translate(-5.0f, 0.0f, 0.0f);
	m_m4Objects[44] = glm::translate(-2.0f, 0.0f, 0.0f);
	m_m4Objects[45] = glm::translate(-3.0f, 0.0f, 0.0f);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	matrix4 m4View = m_pCameraMngr->GetViewMatrix();

	//m_m4Object1 = m_m4Object1 * glm::rotate(matrix4(), 0.01f, vector3(0.0f, 0.0f, 1.0f));

	//m_pMesh->Render(m4Projection, m4View, glm::translate(vector3(3.0f, 0.0f, 0.0f))* ToMatrix4(m_qArcBall));
	
	//m_pMesh->Render(m4Projection, m4View, m_m4Object1);
	//m_pMesh1->Render(m4Projection, m4View, glm::translate(vector3( 3.0f, 0.0f, 0.0f)));
	
	//Changes the matrices to translate to the right.
	for (int i = 0; i < m_m4Objects.size(); i++) {
		m_m4Objects[i] = m_m4Objects[i] * glm::translate(0.01f, 0.0f, 0.0f);
	}

	//Renders the objects to the screen.
	for (int i = 0; i < MeshVector.size(); i++) {
		MeshVector[i]->Render(m4Projection, m4View, m_m4Objects[i]);
	}

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	if (m_pMesh != nullptr)
	{
		delete m_pMesh;
		m_pMesh = nullptr;
	}
	SafeDelete(m_pMesh1);
	//release GUI
	ShutdownGUI();
}