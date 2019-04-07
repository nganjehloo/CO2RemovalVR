#ifndef _FACTORY_H
#define _FACTORY_H

#include <vector>
#include <ctime>

#include "Molecule.h"

#define FACTORY_PATH "../Assets/factory1/factory1.obj"
#define NUM_MOL_INIT 5
#define SECS_BTWN_EMIT 1
#define MAX_MOLS 10
#define MOLS_ON_LOSE 50

class Factory : protected Model
{
public:
	Factory();
	~Factory();

	void draw(GLuint shaderProgram);
	void update();
	void restart();

	int getNumCO2Molecules() { return numCO2Molecules; }
	vector<Molecule*> getMolecules() { return molecules; }

	static bool gameLost;
	static bool gameWon;

private:
	Molecule* createMolecule();

	vector<Molecule*> molecules;
	int numCO2Molecules;

	clock_t timer;
};

#endif
