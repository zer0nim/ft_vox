#ifndef MATERIAL_HPP_
#define MATERIAL_HPP_

#include "commonInclude.hpp"

class Material {
	public:
		Material(glm::vec3 const diffuse = glm::vec3(.36f, .34f, .32f), \
		glm::vec3 const specular = glm::vec3(.3f, .3f, .3f), \
		glm::vec3 const ambient = glm::vec3(0.2f, 0.2f, 0.2f), \
		float const shininess = 16.0f);
		Material(Material const &src);
		virtual ~Material();

		Material &operator=(Material const &rhs);

		glm::vec3	diffuse;
		glm::vec3	specular;
		glm::vec3	ambient;
		float		shininess;

	private:
};

std::ostream & operator << (std::ostream &out, Material const &c);

#endif  // MATERIAL_HPP_
