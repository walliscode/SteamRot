#include "Component.h"


class CMeta : public Component {

private:

	bool m_active = false; // used to determine if the entity is active and will be considered in systems

public:

	const bool getActive() const; // used to determine if the entity is active and will be considered in systems

	void setAlive(); // used to set the entity as active
	void deactivate(); // used to set the entity as inactive

	CMeta() = default; // default constructor for memory allocation
	

};