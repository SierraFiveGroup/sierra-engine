#include "Transform3D.hpp"

SIERRA_GEN_COMP(Transform3D, SIERRA_COMP_CODE_STR("TR3D"));

Transform3D::Transform3D() {

}

void Transform3D::update() {

}

void Transform3D::setPos(glm::vec3 vec) {
    data.pos = vec;
}

void Transform3D::setRotation(glm::vec3 vec) {
    data.rotation = vec;
}

void Transform3D::setScale(glm::vec3 vec) {
    data.scale = vec; 
}