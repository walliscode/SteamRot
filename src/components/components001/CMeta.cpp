#include "CMeta.h"



const bool CMeta::getActive() const {

	return m_active;
}

void CMeta::activate() {

	m_active = true;
}

void CMeta::setDead() {

	m_active = false;
}