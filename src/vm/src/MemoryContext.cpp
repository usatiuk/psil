//
// Created by Stepan Usatiuk on 25.12.2023.
//

#include "MemoryContext.h"

#include <chrono>
#include <exception>
#include <iostream>
#include <unordered_set>

std::atomic<MemoryContext *> CURRENT_MC = nullptr;

MemoryContext::MemoryContext() {
    MemoryContext *expected = nullptr;
    if (!CURRENT_MC.compare_exchange_strong(expected, this)) throw std::runtime_error("MC already exists!");

    _gc_thread = std::thread(std::bind(&MemoryContext::gc_thread_entry, this));
}

MemoryContext::~MemoryContext() {
    MemoryContext *expected = this;
    if (!CURRENT_MC.compare_exchange_strong(expected, nullptr)) {
        std::cerr << "Global MC pointer was overwritten!" << std::endl;
        std::abort();
    }
    _gc_thread_stop = true;
    _gc_request_cv.notify_all();
    _gc_thread.join();
}


void MemoryContext::add_root(Cell *c) {
    {
        std::lock_guard l(_new_roots_lock);
        _new_roots[c]++;
        //        std::cerr << "new root: " << c << " T: " << static_cast<int>(c->_type) << " NUM: " << _new_roots[c] << "\n";
    }
}
void MemoryContext::remove_root(Cell *c) {
    {
        std::lock_guard l(_new_roots_lock);
        _new_roots[c]--;
        //        std::cerr << "del root: " << c << " T: " << static_cast<int>(c->_type) << " NUM: " << _new_roots[c] << "\n";
        if (_new_roots[c] == 0) _new_roots.erase(c);
    }
}

void MemoryContext::gc_thread_entry() {
    while (true) {
        {
            std::unique_lock l(_gc_request_m);
            _gc_request_cv.wait(l, [&] { return _gc_request || _gc_thread_stop; });
        }
        if (_gc_thread_stop) return;

        std::cerr << "gc start " << '\n';

        std::unordered_set<Cell *> seenroots;
        std::queue<Cell *> toVisit;

        auto visitAll = [&]() {
            while (!toVisit.empty()) {
                Cell *c = toVisit.front();
                toVisit.pop();

                if (c == nullptr) continue;

                if (!_cells.contains(c)) _cells.emplace(c);

                if (c->live) continue;

                c->live = true;
                //                std::cerr << "processing c " << c << " " << static_cast<int>(c->_type) << "\n";

                if (c->_type == CellType::CONS) {
                    ConsCell &cc = dynamic_cast<ConsCell &>(*c);
                    //                    std::cerr << "processing car " << cc._car << "\n";
                    toVisit.emplace(cc._car);
                    //                    std::cerr << "real car " << toVisit.back() << "\n";
                    //                    std::cerr << "processing cdr " << cc._cdr << "\n";
                    toVisit.emplace(cc._cdr);
                    //                    std::cerr << "real cdr " << toVisit.back() << "\n";
                }
            }
        };

        {
            std::lock_guard cl(_cells_lock);
            auto start = std::chrono::high_resolution_clock::now();
            {
                decltype(_new_roots) new_roots;
                {
                    decltype(_temp_cells) temp_cells;
                    {
                        std::lock_guard l(_new_roots_lock);
                        std::swap(new_roots, _new_roots);
                        std::swap(temp_cells, _temp_cells);
                    }
                    _cells.insert(temp_cells.begin(), temp_cells.end());
                }


                for (auto const &r: new_roots) {
                    //                    std::cerr << "processing new " << r.first << " diff " << r.second << "\n";
                    if (r.second == 0) continue;
                    _roots[r.first] += r.second;
                    if (_roots[r.first] <= 0)
                        _roots.erase(r.first);
                }
            }
            auto stop = std::chrono::high_resolution_clock::now();
            std::cerr << "New roots time: " << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() << "\n";

            assert(std::none_of(_cells.begin(), _cells.end(), [](const auto &p) { return p->live.load(); }));

            start = std::chrono::high_resolution_clock::now();
            for (const auto &r: _roots) {
                //                std::cerr << "processing r " << r.first << " diff " << r.second << "\n";
                seenroots.emplace(r.first);
                toVisit.emplace(r.first);
            }
            visitAll();
            stop = std::chrono::high_resolution_clock::now();
            //            std::cerr << "Scanned " << _roots.size() << " roots" << std::endl;
            std::cerr << "Roots scan time: " << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() << "\n";

            {
                decltype(_gc_dirty_notif_queue) dirtied;
                std::unique_lock dql(_gc_dirty_notif_queue_lock);
                std::swap(dirtied, _gc_dirty_notif_queue);

                start = std::chrono::high_resolution_clock::now();
                while (!dirtied.empty()) {
                    dql.unlock();
                    for (const auto &r: dirtied) {
                        //                        std::cerr << "processing dirty " << r << "\n";
                        if (seenroots.contains(r)) continue;
                        seenroots.emplace(r);
                        toVisit.emplace(r);
                    }
                    visitAll();

                    dirtied = {};
                    dql.lock();
                    std::swap(dirtied, _gc_dirty_notif_queue);
                }

                stop = std::chrono::high_resolution_clock::now();
                std::cerr << "Dirty mark time: " << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() << "\n";

                assert(dql.owns_lock());

                uint64_t freed = 0;
                start = std::chrono::high_resolution_clock::now();

                std::vector<Cell *> toremove;
                for (const auto &l: _cells) {
                    if (!l->live) {
                        freed += 1;

                        if (l->_type == CellType::NUMATOM) {
                            std::lock_guard il(_indexes_lock);
                            //                            std::cerr << "deleting num: " << l << "\n";
                            _numatom_index.erase(dynamic_cast<NumAtomCell &>(*l)._val);
                        } else if (l->_type == CellType::STRATOM) {
                            std::lock_guard il(_indexes_lock);
                            //                            std::cerr << "deleting str: " << l << "\n";
                            _stratom_index.erase(dynamic_cast<StrAtomCell &>(*l)._val);
                        }

                        assert(!_roots.contains(l));
                        //                        std::cerr << "deleting: " << l << "\n";
                        toremove.emplace_back(l);
                        delete l;
                    } else {
                        //                        std::cerr << "resetting num: " << l << "\n";
                        l->live = false;
                    }
                }

                for (const auto &l: toremove) {
                    _cells.erase(l);
                }

                stop = std::chrono::high_resolution_clock::now();
                std::cerr << "Sweep time: " << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() << "\n";
                _cells_num = _cells.size();
                std::cerr << "GC Freed " << freed << " cells left: " << _cells_num << " \n";
            }

            {
                std::unique_lock l(_gc_done_m);
                std::unique_lock l2(_gc_request_m);
                _gc_done = true;
                _gc_request = false;
                _gc_done_cv.notify_all();
            }
        }
    }
}