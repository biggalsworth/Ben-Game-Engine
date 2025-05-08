#include "Engine_PCH.h"
#include "Layer.h"

namespace Engine
{
	//define the layer name
	Layer::Layer(const std::string& debugName)
		: m_DebugName(debugName)
	{
		std::cout << "New layer created: " << debugName << std::endl;
	}

	Layer::~Layer()
	{

	}
}