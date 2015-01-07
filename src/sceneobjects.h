#ifndef SCENEOBJECTS_H
#define SCENEOBJECTS_H

#include <vector>
#include "camera.h"
#include "surface.h"
#include "light.h"
#include "material.h"

using namespace std;

class sceneobjects {
		camera *pov;
	public:
		sceneobjects () {
			pov = 0;
		}
		~sceneobjects () {
			delete pov;
			for (vector<surface*>::iterator iter = surfaces.begin(); iter != surfaces.end(); ++iter)
				delete (*iter);
			for (vector<light*>::iterator iter = lights.begin(); iter != lights.end(); ++iter)
				delete (*iter);
			for (vector<material*>::iterator iter = materials.begin(); iter != materials.end(); ++iter)
				delete (*iter);
		}
		void setCamera (camera *c) {
			if (!c)
				return;
			if (pov)
				delete pov;
			pov = c;
		}
		camera* getCamera() {
			return pov;
		}
		vector<surface*> surfaces;
		vector<light*> lights;
		vector<material*> materials;
		a_light al;
};

#endif
