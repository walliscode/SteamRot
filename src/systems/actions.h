#include <cstddef>
namespace steamrot {

enum class Actions : size_t {
  kNoAction = 0,
  kJumpBitch = 1 << 0,
};
}
