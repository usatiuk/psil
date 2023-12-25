//
// Created by Stepan Usatiuk on 25.12.2023.
//

#include "MemoryContext.h"

#include <exception>
#include <iostream>

std::atomic<MemoryContext *> CURRENT_MC = nullptr;

MemoryContext::MemoryContext() {
    MemoryContext *expected = nullptr;
    if (!CURRENT_MC.compare_exchange_strong(expected, this)) throw std::runtime_error("MC already exists!");
}

MemoryContext::~MemoryContext() {
    MemoryContext *expected = this;
    if (!CURRENT_MC.compare_exchange_strong(expected, nullptr)) {
        std::cerr << "Global MC pointer was overwritten!" << std::endl;
        std::abort();
    }
}
