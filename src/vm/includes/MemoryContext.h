//
// Created by Stepan Usatiuk on 25.12.2023.
//

#ifndef PSIL_MEMORYCONTEXT_H
#define PSIL_MEMORYCONTEXT_H

#include <atomic>
#include <list>

#include "Cell.h"

class MemoryContext {
public:
    class Handle;

    MemoryContext();
    ~MemoryContext();

    template<typename CT, typename... Args>
    Handle create_cell(Args... args) {
        CT *cell = new CT(std::forward<Args>(args)...);
        _cells.emplace_back(cell);
        return cell;
    }

    class Handle {
    public:
        Handle(Cell *target) : _target(target), _type(target ? target->_type : CellType::NIL) {}

        Cell *operator->() const { return _target; }
        Cell &operator*() const { return *_target; }
        Cell *get() const noexcept { return _target; }

    private:
        Cell *_target = nullptr;
        CellType _type;
    };

private:
    std::list<Cell *> _cells;
};

using MCHandle = MemoryContext::Handle;

extern std::atomic<MemoryContext *> CURRENT_MC;

#endif//PSIL_MEMORYCONTEXT_H
