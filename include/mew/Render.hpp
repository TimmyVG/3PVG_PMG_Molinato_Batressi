#ifndef __RENDER_H__
#define __RENDER_H__ 1
#include <optional>
#include <string>
#include <vector>
#include "Shader.hpp"
#include "glm/mat4x4.hpp"
#include "Mesh.hpp"
#include "Model.hpp"
#include "mew/Object.hpp"
#include <memory>
#include "mew/Shader.hpp"


namespace MEW {
	struct RenderComponent {
		RenderComponent();
		std::shared_ptr<Object> object;

	};

	class RenderSystem {
	public:

		void Draw(RenderComponent *rc, TransformComponent *tc);
	};


}

#endif //__Render__