#pragma once

#include "core/base.h"
#include <glm/glm.hpp>
#include "core/rendering/texture.h"
#include "core/rendering/camera.h"
#include "core/components/transform.h"

namespace engine {
	namespace entities {
		class TextureRenderer {
		public:
			TextureRenderer(Ref<Texture> texture = nullptr, const Transform& transform = Transform(), glm::vec4 color = glm::vec4(1.0f));
			~TextureRenderer();

			void update();
			void draw();

			inline Ref<Texture> get_texture() const { return _texture; }
			inline Transform& get_transform()& { return _transform; }

			void set_position(glm::vec2 position);
			void set_scale(glm::vec2 scale);
			void set_rotation(float rotation);
			void set_color(glm::vec4 color);

			void set_texture(Ref<Texture> texture) { _texture = texture; }
			void set_transform(const Transform& transform) { _transform = transform; }
		private:
			Ref<Texture> _texture;
			Transform _transform;
			glm::vec4 _color;
			float _time_offset;
			float _time_speed;
			float _rotation_speed;
		};
	}
}
