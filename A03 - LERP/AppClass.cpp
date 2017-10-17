#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	m_sProgrammer = "HanSoo Lee - hxl9400@rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUp(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits
	float fRadius = 1.0f; //size to calculate corners of torus for LERP

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		std::vector<vector3> stopList; //create a vector of vector3 in order to add to the main stop list.
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager
		//For the number of sides in this current torus
		for (int j = 0; j < i; j++) {
			//Calculate the corners of the torus using cos and sin.
			stopList.push_back(vector3(fRadius * cos((360 / i) * j * (PI / 180)), fRadius * sin((360 / i) * j * (PI / 180)), 1.0f));
		}
		//Adds the vector of vector3s to the main vector of stops
		masterStopList.push_back(stopList);
		//And adds index numbers for the current vertex and goal vertex of that specific orbit to be used for LERP later.
		currentStopIndex.push_back(0);
		goalStopIndex.push_back(1);
		fSize += 0.5f; //increment the size for the next orbit
		fRadius += 0.475f; //increments to calculate the torus corners and keep up with the size of the torus.
		colorList.push_back(uColor); //store the color int of the torus for color change code in display.
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
	}
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

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	m4Offset = glm::rotate(IDENTITY_M4, 90.0f, AXIS_Z);

	static float fTimer = 0;	//store the new timer
	static uint uClock = m_pSystem->GenClock(); //generate a new clock for that timer
	fTimer += m_pSystem->GetDeltaTime(uClock); //get the delta time for that timer
	float fMax = 1.0f; //maximum amount of time before the timer resets itself
	float fPercent = MapValue(fTimer, 0.0f, fMax, 0.0f, 1.0f); //and the current percentage of time that has passed for calculating LERP position.
	boolean reachedGoal = false; //Boolean for checking if the timer is at it's max, and resetting it accordingly.

	//Taking these lines from InitVariables in order to make the toruses change color after reaching each vertex.
	float fSize = 1.0f; //initial size of orbits
	float decrements = 250.0f / (m_uOrbits > 1 ? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	// draw a shapes
	for (uint i = 0; i < m_uOrbits; i++)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 90.0f, AXIS_X));
		//Calculates the position of the sphere using LERP.
		vector3 v3CurrentPos = glm::lerp(masterStopList[i][currentStopIndex[i]], masterStopList[i][goalStopIndex[i]], fPercent);
		//If the timer exceeds the maximum...
		if (fTimer > fMax) {
			//Change the current and goal indexes so the sphere will seek the next vertex
			currentStopIndex[i] = goalStopIndex[i];
			//If the current stop is the end of the vector...
			if (currentStopIndex[i] == i + 2) {
				//Loop back to the beginning
				goalStopIndex[i] = 0;
			}
			//Else...
			else {
				//Simply go to the next vertex in the vector.
				goalStopIndex[i] += 1;
			}
			//Boolean for checking if the timer is done
			reachedGoal = true;
			
			//Color change code after the timer reaches the maximum
			
			//If the color was violet (minimum wavelength) during this current step...
			if (colorList[i] <= 400 + decrements) {
				//Make it red (maximum wavelength)
				colorList[i] = 650;
			}
			//If not...
			else {
				//Decrement the wavelength so the torus changes to the next color.
				colorList[i] -= static_cast<uint>(decrements);
			}
			//Convert the color to a vector.
			vector3 v3Color = WaveLengthToRGB(colorList[i]);
			//Change the torus in the shape list to be the new one with the new color
			m_shapeList[i] = m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i + 3, v3Color);
			//And adjust the size in order to create the next torus.
			fSize += 0.5f;
		}
		matrix4 m4Model = glm::translate(m4Offset, v3CurrentPos);
		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
		//Once all of the sphere locations are properly calculated with LERP
		if (i == m_uOrbits - 1 && reachedGoal == true) {
			//Reset the timer, so the sphere start seeking the next vertex.
			fTimer = 0;
		}
	}

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
	//release GUI
	ShutdownGUI();
}