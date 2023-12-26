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

    template<>
    Handle create_cell<NumAtomCell>(CellValType val) {
        if (numatom_index.contains(val))
            return numatom_index.at(val);

        NumAtomCell *cell = new NumAtomCell(val);
        _cells.emplace_back(cell);
        numatom_index.emplace(val, cell);
        return cell;
    }

    template<>
    Handle create_cell<StrAtomCell>(std::string val) {
        if (stratom_index.contains(val))
            return stratom_index.at(val);

        StrAtomCell *cell = new StrAtomCell(std::move(val));
        _cells.emplace_back(cell);
        stratom_index.emplace(cell->_val, cell);
        return cell;
    }

    class Handle {
    public:
        Handle(Cell *target) : _target(target) {}

        Cell *operator->() const { return _target; }
        Cell &operator*() const { return *_target; }
        Cell *get() const noexcept { return _target; }

    private:
        Cell *_target = nullptr;
    };

private:
    std::list<Cell *> _cells;
    std::unordered_map<CellValType, Cell *> numatom_index;
    std::unordered_map<std::string, Cell *> stratom_index;
};

using MCHandle = MemoryContext::Handle;

extern std::atomic<MemoryContext *> CURRENT_MC;

#endif//PSIL_MEMORYCONTEXT_H
