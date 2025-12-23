/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the EngineResources struct.
/////////////////////////////////////////////////

#include "EngineResources.h"
#include "DataAccessFactory.h"

namespace steamrot {

/////////////////////////////////////////////////
EngineResources::EngineResources(DataAccessFactory &data_access_factory)
    : asset_manager(data_access_factory) {}

} // namespace steamrot
