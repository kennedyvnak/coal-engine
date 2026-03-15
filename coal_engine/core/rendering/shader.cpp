#include "enpch.h"
#include "shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

namespace engine {
    Shader::Shader(const std::string& filename)
        : Asset(filename), _id(0) {
        ShaderProgramSource source = parse_shader(filename);
        _id = create_shader(source.vertex_source, source.fragment_source);
    }

    Shader::~Shader() {
        glDeleteProgram(_id);
    }

    void Shader::bind() const {
        glUseProgram(_id);
    }

    void Shader::unbind() const {
        glUseProgram(0);
    }

    int Shader::get_uniform_location(const std::string& name) const {
        auto location_search = _uniform_location_cache.find(name);
        if (location_search != _uniform_location_cache.end()) {
            return location_search->second;
        }

        int location = glGetUniformLocation(_id, name.c_str());
        EN_ASSERT(location != -1, "Uniform {} doesn't exist.", name);
        _uniform_location_cache[name] = location;
        return location;
    }

    void Shader::set_uniform_int(const std::string& name, int value) {
        glUniform1i(get_uniform_location(name), value);
    }

    void Shader::set_uniform_int_buffer(const std::string& name, unsigned int count, int* values) {
        glUniform1iv(get_uniform_location(name), count, values);
    }

    void Shader::set_uniform_float(const std::string& name, float value) {
        glUniform1f(get_uniform_location(name), value);
    }

    void Shader::set_uniform_vector2(const std::string& name, const glm::vec2& value) {
        glUniform2f(get_uniform_location(name), value.x, value.y);
    }

    void Shader::set_uniform_vector3(const std::string& name, const glm::vec3& value) {
        glUniform3f(get_uniform_location(name), value.x, value.y, value.z);
    }

    void Shader::set_uniform_vector4(const std::string& name, const glm::vec4& value) {
        glUniform4f(get_uniform_location(name), value.x, value.y, value.z, value.w);
    }

    void Shader::set_uniform_matrix(const std::string& name, const glm::mat4& value) {
        glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    ShaderProgramSource Shader::parse_shader(const std::string& filename) {
        std::ifstream stream(filename);

        enum class ShaderType {
            NONE = -1, VERTEX = 0, FRAGMENT = 1
        };

        std::string line;
        std::stringstream ss[2];
        ShaderType type = ShaderType::NONE;
        while (getline(stream, line)) {
            if (line.find("#shader") != std::string::npos) {
                if (line.find("vertex") != std::string::npos) {
                    type = ShaderType::VERTEX;
                } else if (line.find("fragment") != std::string::npos) {
                    type = ShaderType::FRAGMENT;
                }
            } else {
                ss[(int)type] << line << '\n';
            }
        }

        return { ss[0].str(), ss[1].str() };
    }

    unsigned int Shader::compile_shader(unsigned int type, const std::string& source) {
        unsigned int id = glCreateShader(type);

        const char* src = source.c_str();
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);

        int result;
        glGetShaderiv(id, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE) {
            int length;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
            length -= 1; // Removes \n character at the end of string
            char* message = (char*)malloc(length * sizeof(char));
            glGetShaderInfoLog(id, length, &length, message);
            message[length - 1] = 0;
            EN_LOG_ERROR("Failed to compile {} shader. '{}'", (type == GL_VERTEX_SHADER ? "vertex" : "fragment"), message);
            glDeleteShader(id);
            return 0;
        }

        return id;
    }

    unsigned int Shader::create_shader(const std::string& vert_shader_source, const std::string& frag_shader_source) {
        unsigned int program = glCreateProgram();
        unsigned int vertex_shader = compile_shader(GL_VERTEX_SHADER, vert_shader_source);
        unsigned int fragment_shader = compile_shader(GL_FRAGMENT_SHADER, frag_shader_source);

        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);
        glValidateProgram(program);

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        return program;
    }
}
