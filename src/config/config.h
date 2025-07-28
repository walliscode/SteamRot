/////////////////////////////////////////////////
/// @file
/// @brief Declaration of config namespace functions for application setup
/////////////////////////////////////////////////

namespace config {

enum class EnviromentType {
  Debug,
  Release,
  Install,
  Testing,
};

/////////////////////////////////////////////////
/// @brief Initialises the application environment, cannot be changed later.
///
/// @param env Enum representing the environment type.
/////////////////////////////////////////////////
void InitEnvironment(const EnviromentType env);

/////////////////////////////////////////////////
/// @brief Returns the current environment type.
///
/// @return Enum representing the current environment type.
/////////////////////////////////////////////////
const EnviromentType GetEnvironment();

} // namespace config
