#define GLM_ENABLE_EXPERIMENTAL

#include "enpch.h"
#include "string_utility.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

namespace engine {
	template<typename T>
	std::string to_string(const T& obj) {
		return NULL;
	}

	template<>
	std::string to_string(const int& obj) { return std::to_string(obj); }

	template<>
	std::string to_string(const float& obj) { return std::format("{}", obj); }

	template<>
	std::string to_string(const double& obj) { return std::format("{}", obj); }

	template<>
	std::string to_string(const std::string& obj) { return obj; }

	template<>
	std::string to_string(const glm::vec2& obj) { return format("({}, {})", obj.x, obj.y); }

	template<>
	std::string to_string(const glm::vec3& obj) { return format("({}, {}, {})", obj.x, obj.y, obj.z); }

	template<>
	std::string to_string(const glm::vec4& obj) { return format("({}, {}, {}, {})", obj.x, obj.y, obj.z, obj.w); }

	template<>
	std::string to_string(const glm::mat4& obj) { return glm::to_string(obj); }

	std::string to_string(const char* obj) { return obj; }

	std::string to_string(Ref<Asset> asset) { return asset->get_path(); }
}
