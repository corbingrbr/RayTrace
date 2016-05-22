#include "HitRecord.h"
#include "Object.h"

using namespace std;

HitRecord::HitRecord(float t, shared_ptr<Object> object)
    : t(t),
      object(object)
{
}

HitRecord::~HitRecord()
{
}

float HitRecord::getT()
{
    return t;
}

shared_ptr<Object> HitRecord::getObject()
{
    return object;
}
