// minimal_ecs.hpp
// A single-header minimal ECS suitable for small projects and learning.
// - Entity: uint32 id
// - Components: plain structs
// - Add/remove/get components via Registry
// - view<...>(fn) iterates entities having given components and passes references
// - Up to 64 distinct component types (change MAX_COMPONENTS if you need more)

#ifndef MINIMAL_ECS_HPP
#define MINIMAL_ECS_HPP

#include <cstdint>
#include <bitset>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <typeindex>
#include <type_traits>
#include <functional>
#include <stdexcept>

namespace minimal_ecs {

    using Entity = std::uint32_t;
    static constexpr Entity INVALID_ENTITY = static_cast<Entity>(-1);
    static constexpr std::size_t MAX_COMPONENTS = 64;
    using Signature = std::bitset<MAX_COMPONENTS>;

    // --- component type id generation -------------------------------------------------
    inline std::size_t next_component_type_id() {
        static std::size_t id = 0;
        return id++;
    }

    template<typename T>
    inline std::size_t component_type_id() noexcept {
        static const std::size_t id = next_component_type_id();
        return id;
    }

    // --- component storage interface --------------------------------------------------
    struct IComponentArray {
        virtual ~IComponentArray() = default;
        virtual void remove(Entity e) = 0;
        virtual bool has(Entity e) const = 0;
    };

    template<typename T>
    struct ComponentArray : IComponentArray {
        std::unordered_map<Entity, T> data;

        void insert(Entity e, T component) { data.emplace(e, std::move(component)); }
        void remove(Entity e) override { data.erase(e); }
        T& get(Entity e) { return data.at(e); }
        bool has(Entity e) const override { return data.find(e) != data.end(); }
    };

    // --- Registry --------------------------------------------------------------------
    class Registry {
    public:
        Registry() : next_entity(0) {}

        // Entities
        Entity create() {
            if (!free_ids.empty()) {
                Entity e = free_ids.back(); free_ids.pop_back();
                signatures[e].reset();
                alive.insert(e);
                return e;
            }
            Entity e = next_entity++;
            if (e >= signatures.size()) signatures.resize(e + 1);
            alive.insert(e);
            return e;
        }

        void destroy(Entity e) {
            if (alive.find(e) == alive.end()) return;
            // remove components
            for (auto& kv : component_arrays) {
                kv.second->remove(e);
            }
            signatures[e].reset();
            alive.erase(e);
            free_ids.push_back(e);
        }

        // Components
        template<typename T>
        void add(Entity e, T component) {
            auto id = component_type_id<T>();
            ensure_component_array<T>(id);
            auto* arr = static_cast<ComponentArray<T>*>(component_arrays[typeid(T)].get());
            arr->insert(e, std::move(component));
            signatures[e].set(id);
        }

        template<typename T>
        void remove(Entity e) {
            auto it = component_arrays.find(typeid(T));
            if (it == component_arrays.end()) return;
            static_cast<ComponentArray<T>*>(it->second.get())->remove(e);
            signatures[e].reset(component_type_id<T>());
        }

        template<typename T>
        bool has(Entity e) const {
            auto it = component_arrays.find(typeid(T));
            if (it == component_arrays.end()) return false;
            return static_cast<ComponentArray<T>*>(it->second.get())->has(e);
        }

        template<typename T>
        T& get(Entity e) {
            auto it = component_arrays.find(typeid(T));
            if (it == component_arrays.end()) throw std::runtime_error("Component type not registered");
            return static_cast<ComponentArray<T>*>(it->second.get())->get(e);
        }

        // View: iterate all entities that have all component types Ts
        template<typename... Ts, typename Fn>
        void view(Fn fn) {
            static_assert(sizeof...(Ts) > 0, "view requires at least one component type");
            Signature want;
            (void)std::initializer_list<int>{(want.set(component_type_id<Ts>()), 0)...};

            for (Entity e : alive) {
                if ((signatures[e] & want) == want) {
                    // call fn with references to each component
                    call_with_components<Ts...>(e, fn);
                }
            }
        }

        // simple iteration over entities
        template<typename Fn>
        void each_entity(Fn fn) {
            for (Entity e : alive) fn(e);
        }

    private:
        Entity next_entity;
        std::vector<Entity> free_ids;
        std::unordered_set<Entity> alive;
        std::vector<Signature> signatures;
        std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> component_arrays;

        template<typename T>
        void ensure_component_array(std::size_t id) {
            std::type_index ti = typeid(T);
            if (component_arrays.find(ti) == component_arrays.end()) {
                component_arrays[ti] = std::make_unique<ComponentArray<T>>();
                // if someone requests > MAX_COMPONENTS types, it's a user error
                if (id >= MAX_COMPONENTS) throw std::runtime_error("Exceeded MAX_COMPONENTS");
            }
        }

        template<typename T>
        void ensure_component_array() { ensure_component_array<T>(component_type_id<T>()); }

        // helper to call a function with component references
        template<typename... Ts, typename Fn, std::size_t... I>
        void call_with_components_impl(Entity e, Fn& fn, std::index_sequence<I...>) {
            fn(std::declval<Ts&>()...); // not used; helps deduce types
        }

        template<typename... Ts, typename Fn>
        void call_with_components(Entity e, Fn& fn) {
            // fetch references in the correct order and invoke fn
            fn(get<Ts>(e)...);
        }
    };

} // namespace minimal_ecs

#endif // MINIMAL_ECS_HPP

/*
Usage example (also included in header comments):

#include "ecs.hpp"
#include <iostream>

struct Position { float x,y; };
struct Velocity { float x,y; };

int main() {
    using namespace minimal_ecs;
    Registry registry;

    Entity e = registry.create();
    registry.add<Position>(e, Position{0.f,0.f});
    registry.add<Velocity>(e, Velocity{1.f,2.f});

    registry.view<Position,Velocity>([&](Position &p, Velocity &v){
        p.x += v.x; p.y += v.y;
    });

    auto &p = registry.get<Position>(e);
    std::cout << p.x << ", " << p.y << "\n";
}

Notes:
- This implementation favors simplicity over maximum performance.
- To scale, swap ComponentArray to use contiguous dense storage (sparse set) and prefer iterating the smallest component array in view.
- Increase MAX_COMPONENTS if you need more component types.
*/
