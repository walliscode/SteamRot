/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the DataAccessFactory class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "DataType.h"
#include "EventHandler.h"
#include "FailInfo.h"
#include "IEngineDataProvider.h"
#include "IEntityConfigurator.h"
#include "ISceneDataProvider.h"
#include "ISceneManagerDataProvider.h"
#include <expected>
#include <memory>

namespace steamrot {
class DataAccessFactory {

private:
  /////////////////////////////////////////////////
  /// @brief Reference to the global Event Handler
  /////////////////////////////////////////////////
  EventHandler &m_event_handler;

  /////////////////////////////////////////////////
  /// @brief Enum determing which conrete classes to return
  /////////////////////////////////////////////////
  DataType m_data_type{DataType::Flatbuffers};

  /////////////////////////////////////////////////
  /// @brief Instance of the Engine Data Provider for the factory/game
  /////////////////////////////////////////////////
  std::unique_ptr<IEngineDataProvider> m_engine_data_provider{nullptr};

  /////////////////////////////////////////////////
  /// @brief Instance of the Scene Data Provider for the factory/game
  /////////////////////////////////////////////////
  std::unique_ptr<ISceneManagerDataProvider> m_scene_manager_data_provider{
      nullptr};

  /////////////////////////////////////////////////
  /// @brief Instance of the Scene Data Provider for the factory/game
  /////////////////////////////////////////////////
  std::unique_ptr<ISceneDataProvider> m_scene_data_provider{nullptr};

  /////////////////////////////////////////////////
  /// @brief Instance of the Entity Configurator for the factory/game
  /////////////////////////////////////////////////
  std::unique_ptr<IEntityConfigurator> m_entity_configurator{nullptr};

  /////////////////////////////////////////////////
  /// @brief Set all data providers to Flatbuffers implementations
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> SetFlatbuffersDataProviders();

  /////////////////////////////////////////////////
  /// @brief When called sets up the data providers based on the data type
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> SetDataProviders();

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for DataAccessFactory
  ///
  /// @param data_type DataYpe enum to determine which concrete classes to
  /// return, default is Flatbuffers
  /////////////////////////////////////////////////
  DataAccessFactory(EventHandler &event_handler,
                    const DataType data_type = DataType::Flatbuffers);

  /////////////////////////////////////////////////
  /// @brief Returns a raw pointer to the Engine Data Provider
  /////////////////////////////////////////////////
  std::expected<IEngineDataProvider *, FailInfo> GetEngineDataProvider();

  /////////////////////////////////////////////////
  /// @brief Returns a raw pointer to the Scene Manager Data Provider
  /////////////////////////////////////////////////
  std::expected<ISceneManagerDataProvider *, FailInfo>
  GetSceneManagerDataProvider();

  /////////////////////////////////////////////////
  /// @brief Returns a raw pointer to the Scene Data Provider
  /////////////////////////////////////////////////
  std::expected<ISceneDataProvider *, FailInfo> GetSceneDataProvider();

  /////////////////////////////////////////////////
  /// @brief Sets the data type for the factory
  ///
  /// When called, this will have a cascading effect of resetting all data
  /// @param data_type DataType enum to determine which concrete classes to
  /// instantiate
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> SetDataType(const DataType data_type);
};
} // namespace steamrot
