/////////////////////////////////////////////////
/// @file
/// @brief Example demonstrating DataProvider conversion pattern usage
/////////////////////////////////////////////////

#include "FlatbuffersDataLoader.h"
#include "FlatbuffersSceneDataProvider.h"
#include "FbsSceneData.h"
#include <iostream>

namespace steamrot::examples {

/////////////////////////////////////////////////
/// @brief Example 1: Traditional usage (load and convert together)
/////////////////////////////////////////////////
void TraditionalUsage() {
  std::cout << "=== Example 1: Traditional Usage ===" << std::endl;

  // Create provider
  FlatbuffersSceneDataProvider provider;

  // Load and convert in one step
  auto result = provider.ProvideDefaultSceneData(SceneType::SceneType_TITLE);

  if (result.has_value()) {
    std::unique_ptr<SceneData> scene_data = std::move(result.value());
    std::cout << "Successfully loaded and converted scene data" << std::endl;
    std::cout << "Scene type: " << scene_data->scene_info.type << std::endl;
  } else {
    std::cerr << "Error: " << result.error().message << std::endl;
  }
}

/////////////////////////////////////////////////
/// @brief Example 2: Separate loading and conversion
/////////////////////////////////////////////////
void SeparateLoadingAndConversion() {
  std::cout << "\n=== Example 2: Separate Loading and Conversion ===" << std::endl;

  // Step 1: Load FlatBuffers data
  FlatbuffersDataLoader loader;
  auto load_result = loader.ProvideDefaultSceneData(SceneType::SceneType_TITLE);

  if (!load_result.has_value()) {
    std::cerr << "Load error: " << load_result.error().message << std::endl;
    return;
  }

  const SceneDataFbs *fb_scene_data = load_result.value();
  std::cout << "Successfully loaded FlatBuffers data" << std::endl;

  // Step 2: Convert FlatBuffers to native type
  FlatbuffersSceneDataProvider provider;
  auto convert_result = provider.ConvertSceneData(fb_scene_data);

  if (convert_result.has_value()) {
    std::unique_ptr<SceneData> scene_data = std::move(convert_result.value());
    std::cout << "Successfully converted to native data" << std::endl;
    std::cout << "Scene type: " << scene_data->scene_info.type << std::endl;
  } else {
    std::cerr << "Conversion error: " << convert_result.error().message
              << std::endl;
  }
}

/////////////////////////////////////////////////
/// @brief Example 3: Converting same data multiple times
/////////////////////////////////////////////////
void MultipleConversions() {
  std::cout << "\n=== Example 3: Multiple Conversions ===" << std::endl;

  // Load once
  FlatbuffersDataLoader loader;
  auto load_result = loader.ProvideDefaultSceneData(SceneType::SceneType_TITLE);

  if (!load_result.has_value()) {
    std::cerr << "Load error: " << load_result.error().message << std::endl;
    return;
  }

  const SceneDataFbs *fb_scene_data = load_result.value();
  std::cout << "Loaded FlatBuffers data once" << std::endl;

  // Convert multiple times (useful for creating multiple instances)
  FlatbuffersSceneDataProvider provider;

  for (int i = 0; i < 3; ++i) {
    auto convert_result = provider.ConvertSceneData(fb_scene_data);
    if (convert_result.has_value()) {
      std::unique_ptr<SceneData> scene_data = std::move(convert_result.value());
      std::cout << "Conversion " << (i + 1) << " successful" << std::endl;
    }
  }
}

/////////////////////////////////////////////////
/// @brief Example 4: Error handling with null data
/////////////////////////////////////////////////
void ErrorHandling() {
  std::cout << "\n=== Example 4: Error Handling ===" << std::endl;

  FlatbuffersSceneDataProvider provider;

  // Try to convert null data
  auto result = provider.ConvertSceneData(nullptr);

  if (!result.has_value()) {
    std::cout << "Expected error caught:" << std::endl;
    std::cout << "  Mode: " << static_cast<int>(result.error().mode)
              << std::endl;
    std::cout << "  Message: " << result.error().message << std::endl;
  }
}

} // namespace steamrot::examples

/////////////////////////////////////////////////
/// @brief Main function to run all examples
/////////////////////////////////////////////////
int main() {
  std::cout << "DataProvider Conversion Pattern Examples\n" << std::endl;

  steamrot::examples::TraditionalUsage();
  steamrot::examples::SeparateLoadingAndConversion();
  steamrot::examples::MultipleConversions();
  steamrot::examples::ErrorHandling();

  std::cout << "\n=== All examples completed ===" << std::endl;
  return 0;
}
