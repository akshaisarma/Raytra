#include <iostream>
#include <cstdlib>
#include <vector>
#include <cassert>
#include "readscene.h"

using namespace std;

int main(int argc, char **argv) {
	int pSamples, sSamples;
	bool useBBox = false;
	switch (argc) {
	case 5:
		pSamples = atoi(argv[3]);
		sSamples = atoi(argv[4]);
		break;
	case 6:
		pSamples = atoi(argv[3]);
		sSamples = atoi(argv[4]);
		useBBox = true;
		break;
	default:
		cout << "Usage: raytra scenefilename outputexrfilename pixelSamples shadowSamples [useBBox] \n";
		return 1;
	}
	char *sceneFile = argv[1];
	char *outputFile = argv[2];

	/* Assert samples are valid */
	assert (pSamples >= 1 && sSamples >= 1);

	sceneobjects objs;
	// Put a default material in
	objs.materials.push_back(new material());

	// Parse the scene file
	parseSceneFile(sceneFile, objs);
	cout << "Parsed scene and loaded objects. Rendering ..." <<endl;

	/* Do we have a camera */
	assert (objs.getCamera());

	// Render the scene
	objs.getCamera()->renderScene(objs, pSamples, sSamples, useBBox);

	// Write the output image
	objs.getCamera()->writeEXR(outputFile);

	cout << "\nDone" << endl;

	return 0;
}
