#pragma once

#include "LustDLLMacro.hpp"

#include "Layer.hpp"

#include <list>

namespace Lust
{
	class LUST_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		std::list<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::list<Layer*>::iterator end() { return m_Layers.end(); }
		std::list<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
		std::list<Layer*>::reverse_iterator rend() { return m_Layers.rend(); }

		std::list<Layer*>::iterator advance(size_t index)
		{
			std::list<Layer*>::iterator iterator = m_Layers.begin();
			for (size_t i = 0; (i < index) && iterator != m_Layers.end(); i++)
				iterator++;
			return iterator;
		}

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