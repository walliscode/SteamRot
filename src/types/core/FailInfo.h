/////////////////////////////////////////////////
/// @file
/// @brief decleration of FailInfo struct
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <string>
namespace steamrot {

enum class FailMode {
  None = 0,
  NotImplemented,
  DirectoryNotFound,
  FileNotFound,
  FileOpenFailure,
  FileReadFailure,
  FileSystemError,
  FlatbuffersDataNotFound,
  ParameterOutOfBounds,
  IndexOutOfBounds,
  NonExistentEnumValue,
  SceneTypeNotFound,
  NotAddedToMap,
  EnumValueNotHandled,
  VariantTypeMismatch,
  NullPointer,
  InvalidCast,
  InvalidUUID,
  MissingRequiredField,
  ResourceCreationFailure,
  InvalidInput,
  FileOperationFailed,
  BadValue,
  MissingData
};

struct FailInfo {
  /////////////////////////////////////////////////
  /// @brief Member variable capturing the fail mode as an enum
  /////////////////////////////////////////////////
  FailMode mode;

  /////////////////////////////////////////////////
  /// @brief Message describing the failure
  /////////////////////////////////////////////////
  std::string message;

  /////////////////////////////////////////////////
  /// @brief Default constructor for FailInfo
  ///
  /// @param fail_mode Enum representing the fail mode
  /// @param msg Message describing the failure
  /////////////////////////////////////////////////
  FailInfo(FailMode fail_mode, const std::string &msg)
      : mode(fail_mode), message(msg) {}

  /////////////////////////////////////////////////
  /// @brief Delete
  /////////////////////////////////////////////////
  FailInfo() = delete;
};
} // namespace steamrot
