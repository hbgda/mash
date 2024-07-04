#include "include/MinHook.h"

#define MAKE_HOOK(r, n, p, c) \
    typedef r (*n) p; \
    n n ## _Call = nullptr; \
    r n ## _Fn p c \
    hooking::Hook<n> n ## _Hook( &n ## _Fn, &n ## _Call ); \

#define INSTALL_AND_ENABLE(n, a, fm, sm) \
    if (!n ## _Hook.Install(reinterpret_cast<void**>(a)) || !n ## _Hook.Enable()) { \
        fm; \
    } \
    else { \
        sm; \
    }

namespace hooking {
    template<typename T>
    class Hook {
        void* m_target = nullptr;
        void* m_detour = nullptr;
        T* m_original = nullptr;
    public:
        Hook() {}
        template<typename T>
        Hook(void* detour, T* original) {
            m_detour = detour;
            m_original = original;
        }
        bool Install(void* target) {
            m_target = target;
            return MH_CreateHook(target, m_detour, reinterpret_cast<void**>(m_original)) == MH_OK;
        }
        bool Enable() {
            return MH_EnableHook(m_target) == MH_OK;
        }
        bool Disable() {
            return MH_DisableHook(m_target) == MH_OK;
        }
    };
}