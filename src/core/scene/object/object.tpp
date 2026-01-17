
template<typename T>
T* Object::getComponent(uint32_t code) {
    return (T*)(row + blueprint->getComponentOffset(code));
}