/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Scene test helpers
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "scene_helpers.h"
#include "uuid.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
const uuids::uuid create_uuid() {
  // taken straight from the uuid library example
  std::random_device rd;
  auto seed_data = std::array<int, std::mt19937::state_size>{};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
  std::mt19937 generator(seq);
  uuids::uuid_random_generator gen{generator};
  uuids::uuid const id = gen();
  return id;
}
} // namespace steamrot::tests
