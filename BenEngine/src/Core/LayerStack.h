#pragma once
#include "Core/Core.h"
#include "Layer.h"
#include <vector>

namespace Engine
{
	//this will manage a stack of layers - including the overlays
	//this will make up the different layers and seperate the functionality of them
	//can seperate functionality of layers to handle rendering, UI and input handling
	class LayerStack
	{
	public:
		LayerStack() = default;
		~LayerStack(); 

		void PushLayer(Layer* layer); //adds layer onto stack - the position is indicated by m_LayerInsetIndex
		void PushOverlay(Layer* layer); //inserts overlay to the top of the stack
		void PopLayer(Layer* layer); //removes layer from stack
		void PopOverlay(Layer* layer);

		//these iterators will allow us to loop over the stack

		std::vector<Layer*>::iterator begin()			{ return m_Layers.begin(); }
		std::vector<Layer*>::iterator end()				{ return m_Layers.end(); }
		std::vector<Layer*>::reverse_iterator rbegin()	{ return m_Layers.rbegin(); }
		std::vector<Layer*>::reverse_iterator rend()	{ return m_Layers.rend(); }

		//loop through stacks without modifying them
		std::vector<Layer*>::const_iterator begin()				const { return m_Layers.begin(); }
		std::vector<Layer*>::const_iterator end()				const { return m_Layers.end(); }
		std::vector<Layer*>::const_reverse_iterator rbegin()	const { return m_Layers.rbegin(); }
		std::vector<Layer*>::const_reverse_iterator rend()		const { return m_Layers.rend(); }


	private:
		std::vector<Layer*> m_Layers;
		unsigned int m_LayerInsertIndex = 0;
	};
}