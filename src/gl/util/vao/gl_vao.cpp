#include "gl_vao.hpp"

namespace Sierra {

    GLVAO::GLVAO(): vao(0){

    }
    GLVAO::GLVAO(GLAttribPointer* attrib_pointers, size_t attrib_pointer_count) {
        createVAO();
        createAttribPointers(attrib_pointers, attrib_pointer_count);
    }

    GLVAO::GLVAO(GLVAO&& other) noexcept{
        move(other);
    }

    void GLVAO::operator=(GLVAO&& other) noexcept{
        move(other);
    }

    GLVAO::~GLVAO() {
        if ( !isValid() ) {
            return;
        }

        glDeleteVertexArrays(1, &vao);
    }

    bool GLVAO::isValid() {
        return vao != 0;
    }

    GLuint GLVAO::getVAO() {
        return vao;
    }
    
    void GLVAO::createVAO() {
        glCreateVertexArrays(1, &vao);
    }
    
    void GLVAO::createAttribPointers(GLAttribPointer* attrib_pointers, size_t attrib_pointer_count) {
        glBindVertexArray(vao);

        for (size_t i = 0; i < attrib_pointer_count; i++) {
            GLAttribPointer *attrib = &attrib_pointers[i];

            if (attrib->buff->getType() != GLBuffer::Type::VERTEX) {
                ERROR("Tried to set a vertex attrib with a non-vertex buffer");
                continue;
            } 
            if (!attrib->buff->isValid()) {
                ERROR("Tried to set a vertex attrib with an invalid vertex buffer");
                continue;
            } 

            glBindBuffer(GL_ARRAY_BUFFER, attrib->buff->getBuffer());

            if(attrib->type == GL_FLOAT) {
                glVertexAttribPointer(attrib->index, attrib->componentCount, attrib->type, GL_FALSE, attrib->stride, (const void*)attrib->offset);
            } else if (attrib->type == GL_INT || attrib->type == GL_UNSIGNED_INT) {
                glVertexAttribIPointer(attrib->index, attrib->componentCount, attrib->type, attrib->stride, (const void*)attrib->offset);
            } else {
                throw std::runtime_error("UNIMPLEMENTED"); 
            }

            glEnableVertexAttribArray(attrib->index);

            if(attrib->isInstanced) {
                glVertexAttribDivisor(attrib->index, 1);
            }

            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        glBindVertexArray(0);
    } 

    void GLVAO::addEBO(GLBuffer& buff) {
        if (buff.getType() != GLBuffer::Type::INDEX) {
            ERROR("Tried to set the index buffer in VAO with non index buffer");
            return;
        }
        if (!buff.isValid()) {
            return;
            ERROR("Tried to bind an invalid EBO to a VAO");
        }

        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buff.getBuffer());

        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void GLVAO::move(GLVAO& other) noexcept{
        vao = other.vao;

        other.vao = 0;
    }
    
}