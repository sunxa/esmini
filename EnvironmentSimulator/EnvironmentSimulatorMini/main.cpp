#include <iostream>
#include <string>
#include <random>
#include <thread>
#include <chrono>

#include "ScenarioEngine.hpp"

#include "viewer.hpp"
#include "RoadManager.hpp"
#include "RubberbandManipulator.h"

double deltaSimTime;

static const double stepSize = 0.01;
static const double maxStepSize = 0.01;
static const double minStepSize = 0.001;
static const bool freerun = true;
static std::mt19937 mt_rand;

int main(int argc, char *argv[])
{	
	mt_rand.seed(time(0));

	// Simulation constants
	double endTime = 100;
	double simulationTime = 0;
	double timeStep = 1;

	// use an ArgumentParser object to manage the program arguments.
	osg::ArgumentParser arguments(&argc, argv);

	arguments.getApplicationUsage()->setApplicationName(arguments.getApplicationName());
	arguments.getApplicationUsage()->setDescription(arguments.getApplicationName());
	arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName() + " [options]\n");
	arguments.getApplicationUsage()->addCommandLineOption("--osc <filename>", "OpenSCENARIO filename");

	if (arguments.argc() < 2)
	{
		arguments.getApplicationUsage()->write(std::cout, 1, 80, true);
		return -1;
	}

	std::string oscFilename;
	arguments.read("--osc", oscFilename);

	// Create scenario engine
	ScenarioEngine scenarioEngine(oscFilename, simulationTime);

	// Create viewer
	viewer::Viewer *viewer = new viewer::Viewer(roadmanager::Position::GetOpenDrive(), scenarioEngine.getSceneGraphFilename().c_str(), arguments);

	// ScenarioGateway
	ScenarioGateway *scenarioGateway = scenarioEngine.getScenarioGateway();

	//  Create cars for visualization
	for (int i = 0; i < scenarioEngine.cars.getNum(); i++)
	{
		int carModelID = (double(viewer->carModels_.size()) * mt_rand()) / (std::mt19937::max)();
		viewer->AddCar(carModelID);
	}

	__int64 now, lastTimeStamp = 0;
	double simTime = 0;

	while (!viewer->osgViewer_->done())
	{
		// Get milliseconds since Jan 1 1970
		now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		deltaSimTime = (now - lastTimeStamp) / 1000.0;  // step size in seconds
		lastTimeStamp = now;

		if (deltaSimTime > maxStepSize) // limit step size
		{
			deltaSimTime = maxStepSize;
		}
		else if (deltaSimTime < minStepSize)  // avoid CPU rush, sleep for a while
		{
			std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000 * (minStepSize - deltaSimTime))));
			deltaSimTime = minStepSize;
		}

		// Time operations
		simTime = simTime + deltaSimTime;
		scenarioEngine.setSimulationTime(simTime);
		scenarioEngine.setTimeStep(deltaSimTime);

		// ScenarioEngine
		scenarioEngine.step(deltaSimTime);

		// Visualize cars
		for (int i = 0; i<scenarioEngine.cars.getNum(); i++)
		{
			viewer::CarModel *car = viewer->cars_[i];
			roadmanager::Position pos = scenarioEngine.cars.getPosition(i);

			car->SetPosition(pos.GetX(), pos.GetY(), pos.GetZ());
			car->SetRotation(pos.GetH(), pos.GetR(), pos.GetP());
		}
		
		viewer->osgViewer_->frame();
	}

	return 1;
}

