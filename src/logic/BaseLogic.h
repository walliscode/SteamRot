/**
 * @file
 * @brief Asbtract base class for Template Logic classes.
 *
 */
#pragma once

#include "containers.h"
namespace steamrot {
using EntityIndicies = std::vector<size_t>;

/**
 * @class BaseLogic
 * @brief Allows for polymorphic behaviour of template logic classes.
 *
 */
class BaseLogic {
protected:
  /**
   * @brief Collates all logic for the derived logic classes.
   *
   * @param entities Data structure containing all component data.
   * @param entity_indicies Entity mask for required entities.
   */
  virtual void ProcessLogic(components::containers::EntityMemoryPool &entities,
                            const EntityIndicies &entity_indicies) = 0;

  /**
   * @brief How often the logic should be updated.
   */
  size_t m_update_frequency{2};

  /**
   * @brief Current cycle count for the logic. increased by RunLogic()
   */
  size_t m_cycle_count{0};

public:
  /**
   * @brief Default constructor
   */
  BaseLogic() = default;

  /**
   * @brief Default destructor
   */
  virtual ~BaseLogic() = default;
  /**
   * @brief Guard function that to only update on the required frequency.
   *
   * @param entities Data structure containing all component data.
   * @param entity_indicies Entity mask for required entities.
   */
  void RunLogic(components::containers::EntityMemoryPool &entities,
                const EntityIndicies &entity_indicies);
};
} // namespace steamrot
