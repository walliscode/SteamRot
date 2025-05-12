////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "SchemaChecker.h"
#include "log_handler.h"
#include <unordered_set>

namespace steamrot {
////////////////////////////////////////////////////////////
SchemaChecker::SchemaChecker(nlohmann::json schema) {

  // check schema on construction, will cause runtime error if not correct
  CreateJSONSchema(schema);
}

////////////////////////////////////////////////////////////
void SchemaChecker::ValidateJSONSchema(const nlohmann::json &schema) {
  // check if schema is not an object, handle as error
  if (!schema.is_object()) {

    log_handler::ProcessErrorLog(log_handler::LogCode::kInvalidJSONStructure,

                                 "Schema must be a JSON object.");
  }

  // create keys to check against
  std::unordered_set<std::string> valid_keys{"fieldname", "type", "children",
                                             "child"};

  // create types to check against
  std::unordered_set<std::string> valid_types{"string", "int",   "float",
                                              "bool",   "array", "object"};

  // check schema recursively
  for (const auto &item : schema.items()) {
    // check if key is valid
    if (valid_keys.find(item.key()) == valid_keys.end()) {
      log_handler::ProcessErrorLog(log_handler::LogCode::kInvalidJSONValue,
                                   "Invalid key in schema: " + item.key());
    }
    // check if type is valid
    if (item.key() == "type") {
      if (valid_types.find(item.value()) == valid_types.end()) {
        log_handler::ProcessErrorLog(log_handler::LogCode::kInvalidJSONValue,
                                     &"Invalid type in schema: "[item.value()]);
      }
    }
    // if type is array, check children
    if (item.key() == "children") {
      if (!item.value().is_array()) {
        log_handler::ProcessErrorLog(log_handler::LogCode::kInvalidJSONValue,
                                     "Children must be an array.");
      } else {
        for (const auto &child : item.value()) {
          ValidateJSONSchema(child);
        }
      }
    }

    // if type is object, check child
    if (item.key() == "child") {
      if (!item.value().is_object()) {
        log_handler::ProcessErrorLog(log_handler::LogCode::kInvalidJSONValue,
                                     "Child must be an object.");
      } else {
        ValidateJSONSchema(item.value());
      }
    }
  }
}
////////////////////////////////////////////////////////////
void SchemaChecker::CreateJSONSchema(const nlohmann::json &schema) {
  // check schema, error handling is done in ValidateJSONSchema
  ValidateJSONSchema(schema);

  // if error is not thrown, then we can assign the schema to the member
  // variable
  m_jsonSchema = schema;
}

////////////////////////////////////////////////////////////
bool SchemaChecker::CheckJSON(nlohmann::json j) { return false; }

} // namespace steamrot
