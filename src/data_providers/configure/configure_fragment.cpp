/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions to configure Fragment
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_fragment.h"

namespace steamrot::data::configure {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureFragment(Fragment &fragment, const FragmentFbs *fragment_fbs) {

  // Check for null pointer
  if (fragment_fbs == nullptr) {
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "FragmentFbs pointer is null"});
  }

  // configure name
  if (fragment_fbs->name()) {
    fragment.name = fragment_fbs->name()->str();
  } else {
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "Fragment name is missing"});
  }
  return std::monostate{};
}
} // namespace steamrot::data::configure
