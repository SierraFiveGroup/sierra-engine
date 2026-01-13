
template<typename T>
T* Object::getComponent() {
    return (T*)(row + blueprint->getComponentTypeOffset(typeid(T).hash_code(), typeid(T).name()));
}