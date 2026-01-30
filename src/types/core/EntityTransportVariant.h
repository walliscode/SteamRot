/////////////////////////////////////////////////
/// @file
/// @brief Declaration of EntityTransportVariant type
///
/// This variant provides compile-time type-safe transport of entity data in
/// multiple formats. It's a zero-cost abstraction that supports extensibility
/// while maintaining type safety.
///
/// **Design Rationale**:
/// - All transport types are known at compile time (no runtime type inspection)
/// - std::variant provides type-safe access with no virtual dispatch overhead
/// - New transport formats can be added without breaking existing code
/// - Different formats serve different use cases (testing vs production)
///
/// **Usage Patterns**:
/// - Production: Holds const EntityCollectionFbs * (FlatBuffers pointer)
/// - Testing: Holds EntityMemoryPool or shared_ptr<EntityMemoryPool>
/// - Uninitialized: Holds std::monostate
///
/// The variant is processed by IEntityConfigurator implementations, which
/// extract the appropriate type and configure entities accordingly.
///
/// See documentation/architecture/ENTITY_CONFIGURATOR_DESIGN_ANALYSIS.md for
/// detailed analysis.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "containers.h"
#include "entities_generated.h"
#include <memory>
#include <variant>
namespace steamrot {

//////////////////////////////////////////////////
/// @brief Variant type for transporting entity data in multiple formats
///
/// This variant supports compile-time type-safe transport of entity data:
///
/// **Type Options**:
/// - std::monostate: Empty/uninitialized state (scene has no entities)
/// - EntityMemoryPool: Direct entity data (used in tests for convenience)
/// - std::shared_ptr<EntityMemoryPool>: Shared ownership (used when multiple
///   consumers need the same data)
/// - const EntityCollectionFbs *: FlatBuffers pointer (production use - data
///   loaded from disk, lives in FlatBuffers buffer)
///
/// **Benefits**:
/// - Compile-time type safety: All types known at compile time
/// - Zero-cost abstraction: No virtual dispatch or runtime overhead
/// - Extensible: New transport formats can be added to the variant
/// - Testing flexibility: Tests can inject EntityMemoryPool directly
/// - Production efficiency: FlatBuffers pointer avoids copying large data
///
/// **Access Pattern**:
/// ```cpp
/// if (std::holds_alternative<const EntityCollectionFbs *>(variant)) {
///   auto* fbs_data = std::get<const EntityCollectionFbs *>(variant);
///   // Process FlatBuffers data
/// }
/// ```
/////////////////////////////////////////////////
using EntityTransportVariant = std ::variant<std::monostate, EntityMemoryPool,
                                             std::shared_ptr<EntityMemoryPool>,
                                             const EntityCollectionFbs *>;
} // namespace steamrot
