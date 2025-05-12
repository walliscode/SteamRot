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
    // check if key is valid
    if (valid_schema_keys.find(item.key()) == valid_schema_keys.end()) {
      log_handler::ProcessErrorLog(log_handler::LogCode::kInvalidJSONValue,
                                   "Invalid key in schema: " + item.key());
    }
  }

  // check schema contains fieldname
  if (!schema.contains("fieldname")) {
    log_handler::ProcessErrorLog(log_handler::LogCode::kInvalidJSONValue,
                                 "Schema must contain fieldname.");
  }
  // check schema contains type
  if (!schema.contains("type")) {
    log_handler::ProcessErrorLog(log_handler::LogCode::kInvalidJSONValue,
                                 "Schema must contain type.");
  }

  // if type is array, check it contains children and this is an array
  if (schema["type"] == "array") {
    if (!schema.contains("children")) {
      log_handler::ProcessErrorLog(log_handler::LogCode::kInvalidJSONValue,
                                   "Array type must contain children.");
    } else if (!schema["children"].is_array()) {
      log_handler::ProcessErrorLog(log_handler::LogCode::kInvalidJSONValue,
                                   "Children must be an array.");
    }

    // recursively check children
    for (const auto &child : schema["children"]) {
      ValidateJSONSchema(child);
    }
  }

  // if type is object, check it contains child and this is an object
  if (schema["type"] == "object") {
    if (!schema.contains("child")) {
      log_handler::ProcessErrorLog(log_handler::LogCode::kInvalidJSONValue,
                                   "Object type must contain child.");
    } else if (!schema["child"].is_object()) {
      log_handler::ProcessErrorLog(log_handler::LogCode::kInvalidJSONValue,
                                   "Child must be an object.");
    }

    // recursively check child
    ValidateJSONSchema(schema["child"]);
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
void SchemaChecker::ValidateJSON(const nlohmann::json &game_data) {

  // check if game_data is not an object, handle as error

  if (!game_data.is_object()) {
  }
}
////////////////////////////////////////////////////////////
void SchemaChecker::CheckJSON(nlohmann::json game_data) {}

} // namespace steamrot
