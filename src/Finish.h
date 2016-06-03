#ifndef _FINISH_H_
#define _FINISH_H_

#include <Eigen/Dense>

class Finish {
    
public:
    
    Finish(){}

Finish(float ambient, float diffuse, float specular, float roughness, float reflection, float refraction, float ior)
    : ambient(ambient),
        diffuse(diffuse),
        specular(specular),
        roughness(roughness),
        reflection(reflection),
        refraction(refraction),
        ior(ior) {}

    virtual ~Finish() {};
    float getAmbient() { return ambient; }
    float getDiffuse() { return diffuse; }
    float getSpecular() { return specular; }
    float getRoughness() { return roughness; }
    float getReflection() { return reflection; }
    float getRefraction() { return refraction; }
    float getIOR() { return ior; }

private:
    
    float ambient;
    float diffuse;
    float specular;
    float roughness;
    float reflection;
    float refraction;
    float ior;
    
};

#endif
