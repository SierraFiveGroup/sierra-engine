#include "Drawable.hpp"


SIERRA_GEN_COMP(Drawable, SIERRA_COMP_CODE_STR("DRWB"));

Drawable::Drawable(): data() {

}

Drawable::Drawable(Res::ResID modelID, Res::ResID shaderID): data({modelID, shaderID}) {

}

void Drawable::update() {

}

Res::ResID Drawable::getShader() {
    return data.shaderID;
}

Res::ResID Drawable::getModel() {
    return data.modelID;
}