#ifndef _HIT_RECORD_H_
#define _HIT_RECORD_H_

#include <memory>

class Object;

class HitRecord {
    
public:
    
    HitRecord(float t, std::shared_ptr<Object> object);
    virtual ~HitRecord();
    float getT();
    std::shared_ptr<Object> getObject();

private:

    float t;
    std::shared_ptr<Object> object;
    
};

#endif
