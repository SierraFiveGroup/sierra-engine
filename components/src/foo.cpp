#include "foo.hpp"

SIERRA_GEN_COMP(Foo);

void Foo::update() {
    DBG("hello from foo!");
}

Foo::Foo(): Component() {
    DBG("Fofoofofo");
}