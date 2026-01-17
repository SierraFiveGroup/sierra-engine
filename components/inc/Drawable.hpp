#pragma once

#include "scene/component/component.hpp"
#include "resource_manager/resources/resources.hpp"

namespace Sierra{
    class Object;
} // stupid fucking c++

SIERRA_CORE_COMP(Drawable)
    friend class ::Sierra::Component::PrivateAccess; //so it can get the data offset
    public:
        struct Data { // declare ALL Of the members inside of this, I want to know the layout of the class inside the engine
            //but I cant use the class itself because I dont link it
            Res::ResID modelID;
            Res::ResID shaderID;
        };

        Drawable();

        void update() override;

        //void setShader(Res::ResID shaderID); TODO ill know once I work out the renderer design

        Res::ResID getShader();
        Res::ResID getModel();

    protected:
        Drawable(Res::ResID modelID, Res::ResID shaderID);

        Data data;

END_SIERRA_COMP