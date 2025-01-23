#include <Input.hpp>
#include <Console.hpp>
#include <Layer.hpp>

namespace Lust
{
	class ExampleLayer : public Layer
	{
	public:
		ExampleLayer();

		void OnUpdate(Timestep ts) override;

		virtual void OnImGuiRender() override;

		void OnEvent(Event& event) override;
	private:

	};
}