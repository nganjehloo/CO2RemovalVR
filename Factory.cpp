#include "Factory.h"

#include <iostream>
#include <ctime>
#include <cstdlib>

bool Factory::gameLost = false;
bool Factory::gameWon = false;

Molecule* Factory::createMolecule()
{
	if (Molecule::modelLoaded)
		return new Molecule();
	else
		return new Molecule(true);
}

Factory::Factory() : Model(FACTORY_PATH)
{
	cout << "\nCreating Factory..." << endl;
	numCO2Molecules = NUM_MOL_INIT;

	// Create the first five molecules
	for (int i = 0; i < NUM_MOL_INIT; ++i)
	{
		molecules.push_back(createMolecule());
	}

	timer = std::clock();
}

Factory::~Factory()
{
	for (int i = 0; i < molecules.size(); ++i)
	{
		delete molecules[i];
		molecules[i] = NULL;
	}
	molecules.clear();
	Molecule::cleanup();
}

void Factory::draw(GLuint shaderProgram)
{
	Model::draw(shaderProgram);

	for (int i = 0; i < molecules.size(); ++i)
	{
		molecules[i]->draw(shaderProgram);
	}
}

void Factory::update()
{
	// YOU WIN!
	if (!gameWon && numCO2Molecules <= 0)
	{
		// change the background color to light blue
		glClearColor(0.1f, 0.1f, 1.0f, 1.0f);
		gameWon = true;

		cout << "*************** YOU WIN!!!! *****************" << endl;
	}

	// Emit a new CO2 molecule every second
	else if (!gameWon && numCO2Molecules <= MAX_MOLS)
	{
		if ((std::clock() - timer) / (double)CLOCKS_PER_SEC >= SECS_BTWN_EMIT)
		{
			molecules.push_back(createMolecule());
			++numCO2Molecules;
			timer = std::clock();
		}

		for (int i = 0; i < molecules.size(); ++i)
		{
			molecules[i]->update();
		}
	}

	// YOU LOSE
	else if (!gameLost && !gameWon)
	{
		// spawn a bunch of molecules because you hate the environment
		for (int i = 0; i < MOLS_ON_LOSE; ++i)
		{
			Molecule* mol = createMolecule();
			mol->randomizePosition();
			molecules.push_back(mol);
		}
		gameLost = true;

		cout << "*************** YOU LOSE!!!! *****************" << endl;
	}
}

// called after game win/loss and user presses a button
void Factory::restart()
{
	cout << "\n\n\nRestarting game..." << endl << endl;

	numCO2Molecules = NUM_MOL_INIT;

	for (int i = 0; i < molecules.size(); ++i)
	{
		delete molecules[i];
		molecules[i] = NULL;
	}
	molecules.clear();
	gameWon = false;
	gameLost = false;

	// recreate the first five molecules
	for (int i = 0; i < NUM_MOL_INIT; ++i)
	{
		molecules.push_back(createMolecule());
	}

	timer = std::clock();
}