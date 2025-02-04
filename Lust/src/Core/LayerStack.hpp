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

		std::list<Layer*>::iterator advance(size_t index)
		{
			std::list<Layer*>::iterator iterator = m_Layers.begin();
			for (size_t i = 0; (i < index) && iterator != m_Layers.end(); i++)
				iterator++;
			return iterator;
		}

	private:
		std::list<Layer*> m_Layers;
		size_t m_LayerInsert;
	};

}