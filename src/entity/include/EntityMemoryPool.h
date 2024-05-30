#pragma once
#include <tuple> 
#include <vector>

#include "CTransform.h"

typedef std::tuple<
	std::vector<bool>,	//bool to track if pool position is active
	std::vector<CTransform> //CTransform vector list
> EntityComponentVectorTuple;

class EntityMemoryPool
{
	size_t						m_numEntities; //total number of entitys
	std::shared_ptr<EntityComponentVectorTuple>  m_data; //store of component vectors (all component data for this pool)
	std::shared_ptr<std::vector<bool>>			m_active; //is this column of the component vectors referencing a live entity

public:
	EntityMemoryPool(int poolSize); //constructor for setting pool size

	//Entity pool functions
	int getNextEntityIndex(); //return the next free index for a new entity

	std::shared_ptr<EntityComponentVectorTuple> getData(); //return the data pool for quering

	//Entity pool templates

	template <typename T>
	void defineElem(const T& x, const int size) { //for a passed vector and size, initialise the vector to the specified size
		x = new std::vector<T>[size];
	};

	template <typename TupleT, std::size_t... Is>
	void defineTupleElements(const TupleT& tp, std::index_sequence<Is...>, const int size) { //unfold function to call define element for each tuple element
		(defineElem(std::get<Is>(tp), size), ...);
	}

	template <typename TupleT, std::size_t TupSize = std::tuple_size_v<TupleT>> 
	void defineFreshTuple(const TupleT& tp, const int size) {
		defineTupleElements(tp, std::make_index_sequence<TupSize>{}, size); //call the define tuple elements function with the size of the tuple being passed
	}

	template <typename T>
	void resetVal(const T& x, const int index) { //for a passed vector and index, reset the component at the index to a default value
		x[index] = new T();
	};

	template <typename TupleT, std::size_t... Is>
	void resetTupleElements(const TupleT& tp, std::index_sequence<Is...>, const int index) { //unfold function to call reset value for each tuple element
		(resetVal(std::get<Is>(tp), index), ...);
	}

	template <typename TupleT, std::size_t TupSize = std::tuple_size_v<TupleT>>
	void refreshEntity(const TupleT& tp, const int index) {
		resetTupleElements(tp, std::make_index_sequence<TupSize>{}, index); //call the reset tuple elements function with the size of the tuple being passed
	}
};