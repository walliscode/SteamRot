#include "CMeta.h"



const bool CMeta::getActive() const {

	return m_active;
}

void CMeta::activate() {

	m_active = true;
}

void CMeta::deactivate() {

	m_active = false;
}

void CMeta::fromFlatbuffers(const SteamRot::rawData::MetaComponent* meta) {

	m_active = meta->active();
}

json CMeta::toJSON() {

	json j;
	j["active"] = m_active;
	return j;
}