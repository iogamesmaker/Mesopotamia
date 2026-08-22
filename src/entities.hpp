// entities.hpp
#pragma once
#ifndef entities_hpp
#define entities_hpp

#include "headers.hpp"
#include "global.hpp"
#include "entity.hpp"

class entityManager {
public:
    entityManager();
    ~entityManager();
    void renderAll();
    void updateAll();
    void remove(int idx);
private:
    std::map<std::string, Entity> entities;
};

#endif
