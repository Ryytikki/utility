/** @file SS.h
 Solar System structures in C++ format.
 @author Graeme Lufkin (gwl@u.washington.edu)
 @date Created February 13, 2004
 @version 1.0
 */

#ifndef SS_H__89y053twv9023478bg24tq9h4t2q398
#define SS_H__89y053twv9023478bg24tq9h4t2q398

#include <fstream>
#include <string>

#include "Vector3D.h"
#include "OrientedBox.h"

namespace SS {

struct Header {
	static const unsigned int sizeBytes = 16;
	
	double time;
	int n_data;
	int pad;
	
	Header() : time(0), n_data(0), pad(0) { }
	
	/// Output operator, used for formatted display
	friend std::ostream& operator<<(std::ostream& os, const Header& h) {
		return os << "Time: " << h.time
				<< "\nn_data: " << h.n_data;
	}
};

class Particle {
public:
	static const unsigned int sizeBytes = 96;

	double mass;
	double radius;
	Vector3D<double> pos;
	Vector3D<double> vel;
	Vector3D<double> spin;
	int color;
	int org_idx;
	
	Particle() : mass(0), radius(0), color(0), org_idx(0) { }
	
	/// Output operator, used for formatted display
	friend std::ostream& operator<<(std::ostream& os, const Particle& p) {
		return os << "Mass: " << p.mass
				<< "\nRadius: " << p.radius
				<< "\nPosition: " << p.pos
				<< "\nVelocity: " << p.vel
				<< "\nSpin: " << p.spin
				<< "\nColor: " << p.color
				<< "\nOriginal index: " << p.org_idx;
	}
};

class SSReader {
	bool ok;
	int numParticlesRead;
	
	Header h;
	
	bool responsible;
	std::istream* ssStream;
	
	bool loadHeader();
		
	//copy constructor and equals operator are private to prevent their use (use takeOverStream instead)
	SSReader(const SSReader& r);
	SSReader& operator=(const SSReader& r);
	
public:
		
	SSReader() : ok(false), numParticlesRead(0), responsible(false), ssStream(0) { }
	
	SSReader(const std::string& filename) : ok(false), numParticlesRead(0), responsible(true) {
		ssStream = new std::ifstream(filename.c_str(), std::ios::in | std::ios::binary);
		loadHeader();
	}
	
	void takeOverStream(SSReader& r) {
		ok = r.ok;
		numParticlesRead = r.numParticlesRead;
		h = r.h;
		if(responsible)
			delete ssStream;
		responsible = true;
		r.responsible = false;
		ssStream = r.ssStream;
	}
	
	bool reload(const std::string& filename) {
		if(responsible)
			delete ssStream;
		ssStream = new std::ifstream(filename.c_str(), std::ios::in | std::ios::binary);
		responsible = true;
		return loadHeader();
	}
	
	/** Reload from a stream.
	 @note When using the stream interface, the given stream's buffer must
	 outlast your use of the reader object, since the original stream owns
	 the buffer.  For the standard stream cin this is guaranteed.
	 */
	bool reload(std::istream& is) {
		if(responsible)
			delete ssStream;
		ssStream = new std::istream(is.rdbuf());
		responsible = true;
		return loadHeader();		
	}
	
	~SSReader() {
		if(responsible)
			delete ssStream;
	}
	
	Header getHeader() {
		return h;
	}
	
	bool status() const {
		return ok;
	}
	
	bool getNextParticle(Particle& p);
	
	//bool readAllParticles(Particle* particles);
	//bool readAllParticles(std::vector<Particle>& particles);

	template <typename InputIterator>
	bool readAllParticles(InputIterator begin) {
		if(!ok || !(*ssStream))
			return false;

		//go back to the beginning of the file
		if(!seekParticleNum(0))
			return false;
		
		Particle p;
		for(int i = 0; i < h.n_data; ++i, ++begin) {
			if(!getNextParticle(p))
				return false;
			*begin = p;
		}
		return true;
	}
	
	bool seekParticleNum(unsigned int num);
	
	/// Returns the index of the next particle to be read.
	unsigned int tellParticleNum() const {
		return numParticlesRead;
	}
};

/** This class holds statistics about a SS file. */
class SSStats {
	int numParticles;
public:
	
	SSStats() {
		clear();
	}
	
	double total_mass;
	double volume;
	double density;
	OrientedBox<double> bounding_box;
	OrientedBox<double> velocityBox;
	OrientedBox<double> spinBox;
	
	Vector3D<double> center;
	Vector3D<double> size;
	
	Vector3D<double> center_of_mass;
	Vector3D<double> center_of_mass_velocity;
	Vector3D<double> angular_momentum;
	
	double min_mass, max_mass;
	//note that radius means something different here than in TipsyStats
	double min_radius, max_radius;
	int min_color, max_color;
	int min_org_idx, max_org_idx;
	
	void clear();
	void contribute(const Particle& p);
	void finalize();
	
	/// Output operator, used for formatted display
	friend std::ostream& operator<<(std::ostream& os, const SSStats& p);
};

} //close namespace SS

#endif //SS_H__89y053twv9023478bg24tq9h4t2q398
