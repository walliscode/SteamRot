/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the ILogicStep interface
///
/// This interface abstracts execution steps (Logic classes, free functions,
/// lambdas) for use in both game and test contexts.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Logic.h"
#include "SceneContext.h"
#include <functional>
#include <memory>
#include <string>

namespace steamrot {

/////////////////////////////////////////////////
/// @interface ILogicStep
/// @brief Interface for execution steps in the tick loop
///
/// This interface provides a uniform way to execute steps,
/// whether they are:
/// - Logic class instances
/// - Free functions
/// - Lambdas
///
/// Used primarily by TestEngine for flexible test configuration.
/////////////////////////////////////////////////
class ILogicStep {
public:
  /////////////////////////////////////////////////
  /// @brief Virtual destructor for proper cleanup
  /////////////////////////////////////////////////
  virtual ~ILogicStep() = default;

  /////////////////////////////////////////////////
  /// @brief Execute this step with the given scene context
  ///
  /// @param ctx Scene context providing access to entities and resources
  /////////////////////////////////////////////////
  virtual void Execute(SceneContext &ctx) = 0;

  /////////////////////////////////////////////////
  /// @brief Get the name of this step (for logging/debugging)
  ///
  /// @return Human-readable name of this step
  /////////////////////////////////////////////////
  virtual std::string GetName() const = 0;
};

/////////////////////////////////////////////////
/// @class LogicClassStep
/// @brief Wraps a Logic class as an ILogicStep
///
/// @tparam TLogic The Logic class to wrap (must derive from Logic)
/////////////////////////////////////////////////
template <typename TLogic>
class LogicClassStep : public ILogicStep {
  static_assert(std::is_base_of_v<Logic, TLogic>,
                "TLogic must derive from Logic");

private:
  std::unique_ptr<Logic> m_logic;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor that creates the Logic instance
  ///
  /// @param scene_context Context to pass to the Logic constructor
  /////////////////////////////////////////////////
  explicit LogicClassStep(const SceneContext &scene_context)
      : m_logic(std::make_unique<TLogic>(scene_context)) {}

  /////////////////////////////////////////////////
  /// @brief Execute the wrapped Logic
  ///
  /// @param ctx Scene context (unused - Logic has its own context)
  /////////////////////////////////////////////////
  void Execute(SceneContext &ctx) override {
    (void)ctx; // Logic uses its internal context
    m_logic->RunLogic();
  }

  /////////////////////////////////////////////////
  /// @brief Get the name of the wrapped Logic class
  ///
  /// @return Class name of the Logic
  /////////////////////////////////////////////////
  std::string GetName() const override {
    // Use typeid to get the class name
    return typeid(TLogic).name();
  }
};

/////////////////////////////////////////////////
/// @class FunctionStep
/// @brief Wraps a free function or lambda as an ILogicStep
/////////////////////////////////////////////////
class FunctionStep : public ILogicStep {
private:
  std::function<void(SceneContext &)> m_function;
  std::string m_name;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor taking a function and optional name
  ///
  /// @param func Function to execute
  /// @param name Optional name for debugging (defaults to "FunctionStep")
  /////////////////////////////////////////////////
  explicit FunctionStep(std::function<void(SceneContext &)> func,
                        std::string name = "FunctionStep")
      : m_function(std::move(func)), m_name(std::move(name)) {}

  /////////////////////////////////////////////////
  /// @brief Execute the wrapped function
  ///
  /// @param ctx Scene context passed to the function
  /////////////////////////////////////////////////
  void Execute(SceneContext &ctx) override { m_function(ctx); }

  /////////////////////////////////////////////////
  /// @brief Get the name of this function step
  ///
  /// @return Name provided at construction
  /////////////////////////////////////////////////
  std::string GetName() const override { return m_name; }
};

} // namespace steamrot
