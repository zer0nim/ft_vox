#ifndef SHADER_HPP_
#define SHADER_HPP_

#include <string>
#include <fstream>
#include <sstream>

#include "commonInclude.hpp"

/*
	Shader class used to manage shader compilation
	It also adds some tools to set uniform and activate shader easier

	Warning! before instantiating a Shader object you need to create the opengl contex
	with glfwCreateWindow
*/
class Shader {
	public:
		Shader(std::string const vsPath, std::string const fsPath, std::string const gsPath = "");
		Shader(Shader const &src);
		virtual ~Shader();

		Shader &operator=(Shader const &rhs);

		void	use();
		void	setBool(const std::string &name, bool value) const;
		void	setInt(const std::string &name, int value) const;
		void	setFloat(const std::string &name, float value) const;

		void	setVec2(const std::string &name, float x, float y) const;
		void	setVec2(const std::string &name, const glm::vec2 &vec) const;

		void	setVec3(const std::string &name, float x, float y, float z) const;
		void	setVec3(const std::string &name, const glm::vec3 &vec) const;

		void	setVec4(const std::string &name, float x, float y, float z, float w);
		void	setVec4(const std::string &name, const glm::vec4 &vec);

		void	setMat2(const std::string &name, const glm::mat2 &mat) const;
		void	setMat3(const std::string &name, const glm::mat3 &mat) const;
		void	setMat4(const std::string &name, const glm::mat4 &mat) const;

		class ShaderError : public std::exception {
			public:
				virtual const char* what() const throw() = 0;
		};
		class ShaderCompileException : public ShaderError {
			public:
				virtual const char* what() const throw() {
					return ("Shader failed to compile!");
				}
		};
		class ShaderLinkingException : public ShaderError {
			public:
				virtual const char* what() const throw() {
					return ("Shader program failed to link!");
				}
		};

		u_int32_t	id;

	private:
		void	checkCompileErrors(u_int32_t shader, std::string type);
};

#endif  // SHADER_HPP_
