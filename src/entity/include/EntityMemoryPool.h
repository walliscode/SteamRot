#pragma once
#include <tuple> 
#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>

#include "CTransform.h"

typedef std::tuple<
	std::vector<sf::Uint16>,	//bool to track if pool position is active
	std::vector<CTransform> //CTransform vector list
> EntityComponentVectorTuple;

class EntityMemoryPool
{
	size_t						m_numEntities; //total number of entitys
	std::shared_ptr<EntityComponentVectorTuple>  m_data; //store of component vectors (all component data for this pool)

public:
	EntityMemoryPool(int poolSize); //constructor for setting pool size

	//Entity pool functions
	int getNextEntityIndex(); //return the next free index for a new entity

	std::shared_ptr<EntityComponentVectorTuple> getData(); //return the data pool for quering

	//Entity pool templates
	template <typename T>
	void defineElem(T& x, int size) //for a passed vector and size, initialise the vector to the specified size
	{
		x.resize(size); //Resize the vector
		using vecType = typename T::value_type; //Get the variable type to populate the vector with
		std::fill(x.begin(), x.end(), vecType()); //Populate the vector with empty components
		std::cout << "-> Empty vector of type: '" << typeid(vecType).name() << "' defined, resized to: " << x.size() << " items\n";
	};

	template <typename TupleT, std::size_t... Is>
	void defineTupleElements(TupleT& tp, std::index_sequence<Is...>, size_t size)   //unfold function to call define element for each tuple element
	{
		std::cout << "\nMemory pool tuple split...\n";
		(defineElem(std::get<Is>(tp), size), ...);
	}

	template <typename TupleT, std::size_t TupSize = std::tuple_size_v<TupleT>>
	void defineFreshTuple(TupleT& tp, const size_t size) {
		defineTupleElements(tp, std::make_index_sequence<TupSize>{}, size); //call the define tuple elements function with the size of the tuple being passed
		std::cout << "Tuple definition complete\n\n";
	}

	template <typename T>
	void resetVal(T& x, const size_t index)  //for a passed vector and index, reset the component at the index to a default value
	{
		using vecType = typename T::value_type; //Get the variable type to populate the vector with
		x[index] = vecType(); //Set the index to a default constructed value
		std::cout << "-> Index of type: '" << typeid(vecType).name() << "' reset\n";
	};

	template <typename TupleT, std::size_t... Is>
	void resetTupleElements(TupleT& tp, std::index_sequence<Is...>, const size_t index)  //unfold function to call reset value for each tuple element
	{
		std::cout << "\nIndexed tuple split...\n";
		(resetVal(std::get<Is>(tp), index), ...);
	}

	template <typename TupleT, std::size_t TupSize = std::tuple_size_v<TupleT>>
	void refreshEntity(TupleT& tp, const size_t index) {
		resetTupleElements(tp, std::make_index_sequence<TupSize>{}, index); //call the reset tuple elements function with the size of the tuple being passed
		std::cout << "Tuple index reset complete\n\n";
	}
};