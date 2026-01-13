#include "Transform3D.hpp"

SIERRA_GEN_COMP(Transform3D);

Transform3D::Transform3D() {

}

void Transform3D::update() {

}

void Transform3D::setPos(glm::vec3 vec) {
    pos = vec;
}

void Transform3D::setRotation(glm::vec3 vec) {
    rotation = vec;
}

void Transform3D::setScale(glm::vec3 vec) {
    scale = vec; 
}