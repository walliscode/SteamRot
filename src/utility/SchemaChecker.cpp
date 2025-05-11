////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "SchemaChecker.h"
#include "log_handler.h"
#include <unordered_set>

namespace steamrot {
////////////////////////////////////////////////////////////
SchemaChecker::SchemaChecker() {};

////////////////////////////////////////////////////////////
void SchemaChecker::CreateJSONSchema(const nlohmann::json &schema) {

  // check if schema is not an object, handle as error
  if (!schema.is_object()) {

    log_handler::ProcessErrorLog(log_handler::LogCode::kInvalidJSONStructure,
                                 "Schema must be a JSON object.");
  }
  // create keys to check against
  std::unordered_set<std::string> valid_keys{"field_name", "type", "children"};
}

////////////////////////////////////////////////////////////
bool SchemaChecker::CheckJSON(nlohmann::json j) { return false; }

} // namespace steamrot
