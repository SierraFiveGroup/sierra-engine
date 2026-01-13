
template<typename T>
T* Object::getComponent(uint32_t compID) {
    return (T*)(row + compID);
}