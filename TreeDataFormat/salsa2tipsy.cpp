// Convert a salsa file into Tipsy format.
// Still very incomplete: just reads dark, and doesn't get all the
// fields.

#include <iostream>
#include <cstdio>

#include "tree_xdr.h"
#include "TipsyFile.h"

using namespace std;
using namespace TypeHandling;
using namespace Tipsy;

int main(int argc, char** argv) {
	if(argc < 2) {
		cerr << "Usage: salsa2tipsy basedir" << endl;
		return 1;
	    }
	FILE* infile;
	XDR xdrs;
	FieldHeader fh, uid_fh;
	const int FILELEN = 256;
	char filename[FILELEN];
	
	strncpy(filename, argv[1], FILELEN);
	strcat(filename, "dark/position");

	infile = fopen(filename, "rb");
	if(!infile) {
		cerr << "Couldn't open field file \"" << filename << "\"" << endl;
		return 2;
	}
	xdrstdio_create(&xdrs, infile, XDR_DECODE);
	if(!xdr_template(&xdrs, &fh)) {
		cerr << "Couldn't read header from file!" << endl;
		return 3;
	}
	if(fh.magic != FieldHeader::MagicNumber) {
		cerr << "This file does not appear to be a field file (magic number doesn't match)." << endl;
		return 4;
	}
	unsigned int numParts = fh.numParticles;
	if(fh.dimensions != 3) {
	    cerr << "Wrong dimension of positions." << endl;
	    return 1;
	    }
	
	void* data;
	data = readField(fh, &xdrs);
	
	if(data == 0) {
		cerr << "Had problems reading in the field" << endl;
		return 6;
	}
	
	xdr_destroy(&xdrs);
	fclose(infile);
	TipsyFile tf("tst", 0, fh.numParticles, 0);
	
	for(unsigned int i = 0; i < fh.numParticles; ++i) {
	    for(unsigned int j = 0; j < fh.dimensions; ++j) {
		switch(fh.code) {
			case float32:
			    tf.darks[i].pos[j] = static_cast<float *>(data)[fh.dimensions * i + j];
				break;
			case float64:
			    tf.darks[i].pos[j] = static_cast<double *>(data)[fh.dimensions * i + j];
				break;
			default:
				cout << "I don't recognize the type of this field!";
				return 7;
		    }
		}
	    }
	
	    // Masses
	strncpy(filename, argv[1], FILELEN);
	strcat(filename, "dark/mass");

	infile = fopen(filename, "rb");
	if(!infile) {
		cerr << "Couldn't open field file \"" << filename << "\"" << endl;
		return 2;
	}
	xdrstdio_create(&xdrs, infile, XDR_DECODE);
	if(!xdr_template(&xdrs, &fh)) {
		cerr << "Couldn't read header from file!" << endl;
		return 3;
	}
	if(fh.magic != FieldHeader::MagicNumber) {
		cerr << "This file does not appear to be a field file (magic number doesn't match)." << endl;
		return 4;
	}
	if(numParts != fh.numParticles) {
	    cerr << "Wrong number of Particles" << endl;
	    return 1;
	    }
	if(fh.dimensions != 1) {
	    cerr << "Wrong dimension of masses." << endl;
	    return 1;
	    }
	
	data = readField(fh, &xdrs);
	
	if(data == 0) {
		cerr << "Had problems reading in the field" << endl;
		return 6;
	}
	
	xdr_destroy(&xdrs);
	fclose(infile);
	
	for(unsigned int i = 0; i < fh.numParticles; ++i) {
	    for(unsigned int j = 0; j < fh.dimensions; ++j) {
		switch(fh.code) {
			case float32:
			    tf.darks[i].mass = static_cast<float *>(data)[fh.dimensions * i + j];
				break;
			case float64:
			    tf.darks[i].mass = static_cast<double *>(data)[fh.dimensions * i + j];
				break;
			default:
				cout << "I don't recognize the type of this field!";
				return 7;
		}
		}
	    }
	    // Velocities
	strncpy(filename, argv[1], FILELEN);
	strcat(filename, "dark/velocity");

	infile = fopen(filename, "rb");
	if(!infile) {
		cerr << "Couldn't open field file \"" << filename << "\"" << endl;
		return 2;
	}
	xdrstdio_create(&xdrs, infile, XDR_DECODE);
	if(!xdr_template(&xdrs, &fh)) {
		cerr << "Couldn't read header from file!" << endl;
		return 3;
	}
	if(fh.magic != FieldHeader::MagicNumber) {
		cerr << "This file does not appear to be a field file (magic number doesn't match)." << endl;
		return 4;
	}
	if(numParts != fh.numParticles) {
	    cerr << "Wrong number of Particles" << endl;
	    return 1;
	    }
	if(fh.dimensions != 3) {
	    cerr << "Wrong dimension of velocities." << endl;
	    return 1;
	    }
	
	data = readField(fh, &xdrs);
	
	if(data == 0) {
		cerr << "Had problems reading in the field" << endl;
		return 6;
	}
	
	xdr_destroy(&xdrs);
	fclose(infile);
	
	for(unsigned int i = 0; i < fh.numParticles; ++i) {
	    for(unsigned int j = 0; j < fh.dimensions; ++j) {
		switch(fh.code) {
			case float32:
			    tf.darks[i].vel[j] = static_cast<float *>(data)[fh.dimensions * i + j];
				break;
			case float64:
			    tf.darks[i].vel[j] = static_cast<double *>(data)[fh.dimensions * i + j];
				break;
			default:
				cout << "I don't recognize the type of this field!";
				return 7;
		}
		}
	    }
	

	tf.saveAll(cout);
	cerr << "Done." << endl;	
    }