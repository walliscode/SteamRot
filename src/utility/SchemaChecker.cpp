////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "SchemaChecker.h"
#include "log_handler.h"
#include <unordered_set>

namespace steamrot {
using std::unordered_map;

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

      // if type is object, iterate with a recursive call to check each item
    } else if (type == "object") {
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
                                 const nlohmann::json &schema,
                                 const std::string &current_path) {
  // Check if game_data is an object
  if (!game_data.is_object()) {
    log_handler::ProcessErrorLog(log_handler::LogCode::kInvalidJSONStructure,
                                 "Game data must be a JSON object. Path: " +
                                     current_path);
  }

  // get the allowed keys from the schema
  std::unordered_map<std::string, std::string> allowed_keys_and_values;
  for (const auto &item : schema.items()) {
    std::string key = item.key();
    std::string type = item.value();
    allowed_keys_and_values[key] = type;
  }

  // for each item in the game_data, check key and type. recurse if array or
  // object
  for (const auto &item : game_data.items()) {
    std::string key = item.key();

    // check if key is in the schema
    if (allowed_keys_and_values.find(key) == allowed_keys_and_values.end()) {
      log_handler::ProcessErrorLog(log_handler::LogCode::kInvalidJSONKey,
                                   "Invalid key in game data: " + key);
    }

    // check if value is the correct type
    std::string type = allowed_keys_and_values[key];

    if (type == "string" && !item.value().is_string()) {
      log_handler::ProcessErrorLog(log_handler::LogCode::kInvalidJSONValue,
                                   "Invalid value type for key: " + key +
                                       ". Expected string.");
    } else if (type == "int" && !item.value().is_number_integer()) {
      log_handler::ProcessErrorLog(log_handler::LogCode::kInvalidJSONValue,
                                   "Invalid value type for key: " + key +
                                       ". Expected int.");
    } else if (type == "float" && !item.value().is_number_float()) {
      log_handler::ProcessErrorLog(log_handler::LogCode::kInvalidJSONValue,
                                   "Invalid value type for key: " + key +
                                       ". Expected float.");
    } else if (type == "bool" && !item.value().is_boolean()) {
      log_handler::ProcessErrorLog(log_handler::LogCode::kInvalidJSONValue,
                                   "Invalid value type for key: " + key +
                                       ". Expected bool.");
    } else if (type == "array") {
      for (const auto &object : item.value()) {
        // recurse to check each item in the array
        ValidateJSON(object, schema[key][0], current_path + "." + key);
      }
    } else if (type == "object") {
      ValidateJSON(item.value(), schema[key], current_path + "." + key);
    } else {
      // if type is not in the schema, then it is an error
      log_handler::ProcessErrorLog(log_handler::LogCode::kInvalidJSONValue,
                                   "Invalid value type for key: " + key +
                                       ". Expected object.");
    }
  }
}

////////////////////////////////////////////////////////////
void SchemaChecker::CheckJSON(nlohmann::json game_data) {
  // validate json against schema
  ValidateJSON(game_data, m_jsonSchema);
}
} // namespace steamrot
