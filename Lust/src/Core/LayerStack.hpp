#pragma once

#include "LustDLLMacro.hpp"

#include "Layer.hpp"

#include <list>

namespace Lust
{
	/**
	* @brief %LayerStack class is a container for layers
	* @details , where you can add, remove and iterate layers
	*/
	class LUST_API LayerStack
	{
	public:
		/**
		* @brief LayerStack constructor
		*/
		LayerStack();
		/**
		* @brief LayerStack destructor
		*/
		~LayerStack();

		/**
		* @brief Push a %layer to the list
		* @param layer %layer to be pushed
		*/
		void PushLayer(Layer* layer);
		/**
		* @brief Push a %layer to the last element of the list
		* @param overlay %layer to be pushed
		*/
		void PushOverlay(Layer* overlay);
		/**
		* @brief Remove a %layer from the list
		* @param layer %layer to be removed
		*/
		void PopLayer(Layer* layer);
		/**
		* @brief Remove a %layer from the last element of the list
		* @param overlay %layer to be removed
		*/
		void PopOverlay(Layer* overlay);

		/**
		* @brief Get the iterator to the beginning of the list
		* @return std::list<Layer*>::iterator
		*/
		std::list<Layer*>::iterator begin() { return m_Layers.begin(); }
		/**
		* @brief Get the iterator to the end of the list
		* @return std::list<Layer*>::iterator
		*/
		std::list<Layer*>::iterator end() { return m_Layers.end(); }
		/**
		* @brief Get the reverse iterator to the beginning of the list
		* @return std::list<Layer*>::reverse_iterator
		*/
		std::list<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
		/**
		* @brief Get the reverse iterator to the end of the list
		* @return std::list<Layer*>::reverse_iterator
		*/
		std::list<Layer*>::reverse_iterator rend() { return m_Layers.rend(); }

		/**
		* @brief Get the iterator to the index of the list
		* @param index index of the iterator
		* @return std::list<Layer*>::iterator
		*/
		std::list<Layer*>::iterator advance(size_t index)
		{
			std::list<Layer*>::iterator iterator = m_Layers.begin();
			for (size_t i = 0; (i < index) && iterator != m_Layers.end(); i++)
				iterator++;
			return iterator;
		}

		/**
		* @brief Get the reverse iterator to the index of the list
		* @param index index of the iterator
		* @return std::list<Layer*>::reverse_iterator
		*/
		std::list<Layer*>::reverse_iterator radvance(size_t index)
		{
			std::list<Layer*>::reverse_iterator iterator = m_Layers.rbegin();
			for (size_t i = 0; (i < index) && iterator != m_Layers.rend(); i++)
				iterator++;
			return iterator;
		}

	private:
		std::list<Layer*> m_Layers;
		size_t m_LayerInsert;
	};

}