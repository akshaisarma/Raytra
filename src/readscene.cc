#include <cmath>
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <cstdlib>
#include <vector>
#include <cassert>
#include "readscene.h"
#include "sphere.h"
#include "plane.h"
#include "triangle.h"
#include "camera.h"
#include "basic_constructs.h"

using namespace std;

int getMaterialIndex (vector<material*> &mats, double dr, double dg, double db, double sr, double sg, double sb,
					  double r, double ir, double ig, double ib) {
	int i = 0;
	for (vector<material*>::iterator iter = mats.begin(); iter != mats.end(); ++iter, ++i)
		if ((*iter)->isSame(dr, dg, db, sr, sg, sb, r, ir, ig, ib))
			break;

	if ((size_t)i != mats.size())
		return i;

	mats.push_back(new material(dr, dg, db, sr, sg, sb, r, ir, ig, ib));
	return i;
}

const char* getFileName (string inString) {
	unsigned int i = 1;
	while (i < inString.size()) {
		if (inString[i] != ' ')
			break;
		i++;
	}
	assert(i < inString.size());

	return inString.substr(i).c_str();
}

double getTokenAsFloat (string inString, int whichToken) {

    double thisFloatVal = 0.;

    if (whichToken == 0) {
        cerr << "error: the first token on a line is a character!" << endl;
        exit (-1);
    }

    // c++ string class has no super-easy way to tokenize, let's use c's:
    char *cstr = new char [inString.size () + 1];

    strcpy (cstr, inString.c_str());

    char *p = strtok (cstr, " ");
    if (p == 0) {
        cerr << "error: the line has nothing on it!" << endl;
        exit (-1);
    }

    for (int i = 0; i < whichToken; i++) {
        p = strtok (0, " ");
        if (p == 0 ) {
            cerr << "error: the line is not long enough for your token request!" << endl;
            exit (-1);
        }
    }

    thisFloatVal = atof (p);

    delete[] cstr;

    return thisFloatVal;
}

// Given the name of a wavefront (OBJ) file that consists JUST of
// vertices, triangles, and comments, read it into the tris and verts
// vectors.
//
// tris is a vector of ints that is 3*n long, where n is the number of
// triangles. The ith triangle has vertex indexes 3*i, 3*i+1, and 3*i+2.
//
// The ith triangle has vertices verts[3*i], verts[3*i+1], and verts[3*i+2],
// given in counterclockwise order with respect to the surface normal
//
// If you are using the supplied Parser class, you should probably make this
// a method on it: Parser::read_wavefront_file().
//
void readWavefrontFile (const char *file, std::vector< int > &tris, std::vector< double > &verts)
{
    tris.clear ();
    verts.clear ();

    ifstream in(file);
    char buffer[1025];
    string cmd;


    for (int line=1; in.good(); line++) {
        in.getline(buffer,1024);
        buffer[in.gcount()]=0;

        cmd="";

        istringstream iss (buffer);

        iss >> cmd;

        if (cmd[0]=='#' or cmd.empty()) {
            // ignore comments or blank lines
            continue;
        }
        else if (cmd=="v") {
            // got a vertex:

            // read in the parameters:
            double pa, pb, pc;
            iss >> pa >> pb >> pc;

            verts.push_back (pa);
            verts.push_back (pb);
            verts.push_back (pc);
         }
        else if (cmd=="f") {
            // got a face (triangle)

            // read in the parameters:
            int i, j, k;
            iss >> i >> j >> k;

            // vertex numbers in OBJ files start with 1, but in C++ array
            // indices start with 0, so we're shifting everything down by
            // 1
            tris.push_back (i-1);
            tris.push_back (j-1);
            tris.push_back (k-1);
        }
        else {
            std::cerr << "Parser error: invalid command at line " << line << std::endl;
        }
     }
    in.close();

 //std::cout << "found this many tris, verts: " << tris.size () / 3.0 << "  " << verts.size () / 3.0 << std::endl;
}

void parseSceneFile (const char *filnam, sceneobjects &sObjects) {
    ifstream inFile(filnam);
    string line;

    if (! inFile.is_open ()) {
        cerr << "can't open scene file" << endl;
        exit (-1);
    }

    int lastMaterialLoaded = 0;
    std::vector< int > tris;
    std::vector< double > verts;

    while ( !inFile.eof() ) {
        getline (inFile, line);

        switch (line[0])  {

            case 's': {
                // sphere. Only spheres have opt refraction parameters
                double x, y, z, r;
                x  = getTokenAsFloat (line, 1);
                y  = getTokenAsFloat (line, 2);
                z  = getTokenAsFloat (line, 3);
                r  = getTokenAsFloat (line, 4);
				sphere *sp = new sphere(point(x, y, z), r);
				sp->setMaterial(lastMaterialLoaded);
				sObjects.surfaces.push_back(sp);
                break;
				}
            case 't': {
				// triangle
				double x1, y1, z1, x2, y2, z2, x3, y3, z3;
                x1 = getTokenAsFloat (line, 1);
                y1 = getTokenAsFloat (line, 2);
                z1 = getTokenAsFloat (line, 3);
                x2 = getTokenAsFloat (line, 4);
				y2 = getTokenAsFloat (line, 5);
				z2 = getTokenAsFloat (line, 6);
                x3 = getTokenAsFloat (line, 7);
                y3 = getTokenAsFloat (line, 8);
                z3 = getTokenAsFloat (line, 9);
                point p1 = point (x1, y1, z1);
                point p2 = point (x2, y2, z2);
                point p3 = point (x3, y3, z3);
                triangle *tr = new triangle (p1, p2, p3);
                tr->setMaterial(lastMaterialLoaded);
                sObjects.surfaces.push_back(tr);
                break;
            	}
            case 'p': {
				// plane
            	double nx, ny, nz, d;
            	nx = getTokenAsFloat (line, 1);
            	ny = getTokenAsFloat (line, 2);
            	nz = getTokenAsFloat (line, 3);
            	d = getTokenAsFloat (line, 4);
            	mvector norm = mvector (nx, ny, nz);
            	plane *pl = new plane (norm, d);
            	pl->setMaterial(lastMaterialLoaded);
            	sObjects.surfaces.push_back(pl);
                break;
            	}
            case 'c':   {
            	// camera:
				double xx, yy, zz, vx, vy, vz, dd, iw, ih, pw, ph;
                xx = getTokenAsFloat (line, 1);
                yy = getTokenAsFloat (line, 2);
                zz = getTokenAsFloat (line, 3);
                vx = getTokenAsFloat (line, 4);
				vy = getTokenAsFloat (line, 5);
				vz = getTokenAsFloat (line, 6);
                dd = getTokenAsFloat (line, 7);
                iw = getTokenAsFloat (line, 8);
                ih = getTokenAsFloat (line, 9);
                pw = (int) getTokenAsFloat (line, 10);
                ph = (int) getTokenAsFloat (line, 11);
				camera *cam  = new camera(xx, yy, zz, vx, vy, vz, dd, iw, ih, pw, ph);
				sObjects.setCamera(cam);
            	break;
				}
            case 'l':
				// light
                // slightly different from the rest, we need to examine the second param,
                switch (line[2]) {
                    case 'p': {
						// point light
						double x, y, z, r, g, b;
						x = getTokenAsFloat (line, 2);
						y = getTokenAsFloat (line, 3);
						z = getTokenAsFloat (line, 4);
						r = getTokenAsFloat (line, 5);
						g = getTokenAsFloat (line, 6);
						b = getTokenAsFloat (line, 7);
						p_light *pl = new p_light(point(x, y, z), RGB(r, g, b));
						sObjects.lights.push_back(pl);
                    	break;
						}
                    case 's': {
                    	// square directional light
                    	double x, y, z, dx, dy, dz, ux, uy, uz, len, r, g, b;
						x = getTokenAsFloat (line, 2);
						y = getTokenAsFloat (line, 3);
						z = getTokenAsFloat (line, 4);
						dx = getTokenAsFloat (line, 5);
						dy = getTokenAsFloat (line, 6);
						dz = getTokenAsFloat (line, 7);
						ux = getTokenAsFloat (line, 8);
						uy = getTokenAsFloat (line, 9);
						uz = getTokenAsFloat (line, 10);
						len = getTokenAsFloat (line, 11);
						r = getTokenAsFloat (line, 12);
						g = getTokenAsFloat (line, 13);
						b = getTokenAsFloat (line, 14);
						s_light *sl = new s_light(point(x, y, z), mvector(dx, dy, dz),
													mvector(ux, uy, uz), len, RGB (r, g, b));
						sObjects.lights.push_back(sl);
                        break;
						}
                    case 'a': {
						// ambient light
						double r, g, b;
						r = getTokenAsFloat (line, 2);
						g = getTokenAsFloat (line, 3);
						b = getTokenAsFloat (line, 4);
						sObjects.al.set(r, g, b);
                        break;
						}
                }
                break;
            case 'm': {
				// material
				double dr, dg, db, sr, sg, sb, r, ir, ig, ib;
				dr = getTokenAsFloat (line, 1);
				dg = getTokenAsFloat (line, 2);
				db = getTokenAsFloat (line, 3);
				sr = getTokenAsFloat (line, 4);
				sg = getTokenAsFloat (line, 5);
				sb = getTokenAsFloat (line, 6);
				r  = getTokenAsFloat (line, 7);
				ir = getTokenAsFloat (line, 8);
				ig = getTokenAsFloat (line, 9);
				ib = getTokenAsFloat (line, 10);

				lastMaterialLoaded = getMaterialIndex(sObjects.materials, dr, dg, db, sr, sg, sb,
														r, ir, ig, ib);
                break;
            }
            case 'w': {
            	// WaveFront Obj file
            	readWavefrontFile(getFileName(line), tris, verts);
            	for (unsigned int i = 0; i < tris.size(); i+=3) {
            		int p1_i = 3*tris[i];
            		int p2_i = 3*tris[i+1];
            		int p3_i = 3*tris[i+2];
            		point p1 = point (verts[p1_i], verts[p1_i+1], verts[p1_i+2]);
            		point p2 = point (verts[p2_i], verts[p2_i+1], verts[p2_i+2]);
            		point p3 = point (verts[p3_i], verts[p3_i+1], verts[p3_i+2]);
            		triangle *tr = new triangle (p1, p2, p3);
    				tr->setMaterial(lastMaterialLoaded);
            		sObjects.surfaces.push_back(tr);
            	}
            	break;
            	}
            case '/':
                // comment
                break;

            case 'o':
				// make your own options if you wish
                break;
        }

    }
}
