#include "Component.h"


class CMeta : public Component {

public:
	bool isActive = false; // used to determine if the entity is active and will be considered in systems

	CMeta() = default; // default constructor for memory allocation
	CMeta(const bool active) : isActive(active) {} // constructor for setting the active state of the entity

};