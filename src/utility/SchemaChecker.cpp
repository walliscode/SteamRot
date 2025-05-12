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

  // create types to check against
  // check all keys are valid
  for (const auto &item : schema.items()) {

    // set values
    std::string type = item.value();
    std::string key = item.key();

    // check if value is valid type
    if (valid_schema_types.find(type) == valid_schema_types.end()) {

      log_handler::ProcessErrorLog(log_handler::LogCode::kInvalidJSONValue,
                                   "Invalid type in schema: " + type);
    }

    // if type is array, iterate with a recursive call to check each item
    if (type == "array") {
      for (const auto &object : item.value()) {
        ValidateJSONSchema(object);
      }
    }

    // if type is object, iterate with a recursive call to check each item
    if (type == "object") {
      ValidateJSONSchema(item.value());
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
void SchemaChecker::ValidateJSON(const nlohmann::json &game_data,
                                 const std::string &current_path) {
  // Check if game_data is an object
  if (!game_data.is_object()) {
    log_handler::ProcessErrorLog(log_handler::LogCode::kInvalidJSONStructure,
                                 "Game data must be a JSON object. Path: " +
                                     current_path);
  }
}

////////////////////////////////////////////////////////////
void SchemaChecker::CheckJSON(nlohmann::json game_data) {}

} // namespace steamrot
