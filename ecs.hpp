// minimal_ecs.hpp
// A single-header minimal ECS suitable for small projects and learning.
// Includes a minimal EventBus for event-driven systems.

#pragma once
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

namespace ecs {

    using Entity = std::uint32_t;
    static constexpr Entity INVALID_ENTITY = static_cast<Entity>(-1);
    static constexpr std::size_t MAX_COMPONENTS = 64;
    using Signature = std::bitset<MAX_COMPONENTS>;

    // --- component type id generation ---
    inline std::size_t next_component_type_id() {
        static std::size_t id = 0;
        return id++;
    }

    template<typename T>
    inline std::size_t component_type_id() noexcept {
        static const std::size_t id = next_component_type_id();
        return id;
    }

    // --- component storage ---
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

    // --- Registry ---
    class Registry {
    public:
        Registry() : next_entity(0) {}

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
            if (!alive.count(e)) return;
            for (auto& kv : component_arrays) kv.second->remove(e);
            signatures[e].reset();
            alive.erase(e);
            free_ids.push_back(e);
        }

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
            if (it == component_arrays.end()) throw std::runtime_error("Component not registered");
            return static_cast<ComponentArray<T>*>(it->second.get())->get(e);
        }

        template<typename... Ts, typename Fn>
        void view(Fn fn) {
            static_assert(sizeof...(Ts) > 0, "view requires at least one component type");
            Signature want;
            (void)std::initializer_list<int>{(want.set(component_type_id<Ts>()), 0)...};

            for (Entity e : alive) {
                if ((signatures[e] & want) == want)
                    fn(get<Ts>(e)...);
            }
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
            if (!component_arrays.count(ti)) {
                component_arrays[ti] = std::make_unique<ComponentArray<T>>();
                if (id >= MAX_COMPONENTS) throw std::runtime_error("Exceeded MAX_COMPONENTS");
            }
        }
    };

    // --- Event Bus ---
    class EventBus {
    public:
        template<typename EventType>
        void emit(const EventType& event) {
            auto& q = getQueue<EventType>();
            q.push_back(event);
        }

        template<typename EventType>
        void subscribe(std::function<void(const EventType&)> listener) {
            auto& ls = getListeners<EventType>();
            ls.push_back(listener);
        }

        template<typename EventType>
        void dispatchAll() {
            auto& q = getQueue<EventType>();
            auto& ls = getListeners<EventType>();
            for (auto& e : q) for (auto& fn : ls) fn(e);
            q.clear();
        }

        void dispatchAllEvents() {
            for (auto& [_, dispatcher] : dispatchers)
                dispatcher();
        }

        template<typename EventType>
        void registerEvent() {
            dispatchers[typeid(EventType)] = [this]() { dispatchAll<EventType>(); };
        }

    private:
        template<typename EventType>
        struct QueueHolder { std::vector<EventType> events; };
        template<typename EventType>
        struct ListenerHolder { std::vector<std::function<void(const EventType&)>> funcs; };

        std::unordered_map<std::type_index, std::shared_ptr<void>> queues;
        std::unordered_map<std::type_index, std::shared_ptr<void>> listeners;
        std::unordered_map<std::type_index, std::function<void()>> dispatchers;

        template<typename EventType>
        std::vector<EventType>& getQueue() {
            std::type_index ti = typeid(EventType);
            if (!queues.count(ti)) queues[ti] = std::make_shared<QueueHolder<EventType>>();
            return static_cast<QueueHolder<EventType>*>(queues[ti].get())->events;
        }

        template<typename EventType>
        std::vector<std::function<void(const EventType&)>>& getListeners() {
            std::type_index ti = typeid(EventType);
            if (!listeners.count(ti)) listeners[ti] = std::make_shared<ListenerHolder<EventType>>();
            return static_cast<ListenerHolder<EventType>*>(listeners[ti].get())->funcs;
        }
    };

} // namespace ecs
