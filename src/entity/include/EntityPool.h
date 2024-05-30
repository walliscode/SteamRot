#include "tuple"
#include "vector"

template<typename... ComponentType>
using EntityComponentVectors = std::tuple<std::vector<ComponentType>...>; //"using" is required when using typedefs with templates

class EntityPool {
	friend class TestEntityPool;

private:

	EntityComponentVectors<>   m_data;
	EntityPool() = default;
};