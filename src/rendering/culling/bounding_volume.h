#pragma once
class BoundingVolume
{
public:
	virtual bool intersectsFrustum() {};
};

class AABB : BoundingVolume 
{
public:
	bool intersectsFrustum();
};
