# C++ DLL Boundary — Memory, STL and ABI Issues

## Core Problem
When a DLL and consuming executable use different CRT heaps (mixed /MT//MD,
Debug/Release, different compiler versions, Unreal Engine), new/delete across
the boundary causes crashes.

## Memory: new/delete across boundaries

### Option 1: Static library (.lib)
No boundary. Simplest fix, no code changes needed.

### Option 2: shared_ptr throughout
shared_ptr captures the deleter at construction time in its control block.
If constructed inside the DLL, deletion always routes back into the DLL. ✅
Overhead: atomic ref count + control block allocation.

### Option 3: unique_ptr with custom deleter
  struct DllDeleter {
      void operator()(T* p) const noexcept { dll_free(p); }
  };
  using CommandPtr = std::unique_ptr<Command, DllDeleter>;
Stateless struct deleter = zero size overhead.
Implicitly converts to shared_ptr, preserving the deleter. ✅

### Option 4: Override operator new/delete on base class
  class Command {
      void* operator new(std::size_t n)      { return dll_alloc(n); }
      void  operator delete(void* p) noexcept{ dll_free(p); }
  };
- dll_alloc/dll_free use malloc/free (bypass CRT entirely, safe with Unreal).
- Inherited by all derived classes — user-defined and plugin-defined commands
  automatically allocate on the DLL heap.
- virtual destructor guarantees operator delete lookup follows the vtable,
  so delete through base pointer routes correctly.
- No common base class needed — apply per class independently.
✅ Recommended for polymorphic class hierarchies.

## STL members in exported classes
std::string, std::deque, std::map etc. have CRT-dependent layout and allocation.
Exporting a class with STL members is unsafe across mismatched CRTs.

### Solutions:
- Make the class header-only (STL instantiated in consumer's TU, no boundary).
- Use a custom allocator routing through dll_alloc/dll_free:

  template<typename T>
  struct DllAllocator {
      using value_type = T;
      DllAllocator() = default;
      template<typename U>
      DllAllocator(const DllAllocator<U>&) noexcept {}  // rebind
      T*   allocate(std::size_t n)              { return static_cast<T*>(dll_alloc(n*sizeof(T))); }
      void deallocate(T* p, std::size_t) noexcept { dll_free(p); }
      template<typename U>
      friend bool operator==(const DllAllocator&, const DllAllocator<U>&) noexcept { return true; }
      template<typename U>
      friend bool operator!=(const DllAllocator&, const DllAllocator<U>&) noexcept { return false; }
  };

  // Usage:
  std::deque<Command*, DllAllocator<Command*>> m_Commands;

  Notes:
  - Rebind constructor required: STL containers internally allocate
    bookkeeping types (e.g. std::_Container_proxy), not just T.
  - operator==/!= required: STL swap/move calls operator== on allocators
    to decide propagation (POCS). Stateless allocator = always equal.

## String and other types across virtual methods

  virtual std::string Name()              // ❌ heap + layout issues
  virtual const char* Name()              // ✅ plain pointer, no ownership
  virtual void Foo(const std::string& s)  // ❌ layout differs Debug/Release
  virtual void Foo(const char* s)         // ✅ safe

  const char* MyCommand::Name() const { return "MyCommand"; } // literal = static storage ✅

  // If concatenation needed, use thread_local buffer in DLL's .cpp:
  const char* Command::Call() const {
      thread_local std::string buffer;
      buffer = Name(); buffer += "()";
      return buffer.c_str(); // valid until next Call() on this thread
  }

## std::istream across boundaries
std::istream has vtable + CRT-tied internal state. Unsafe to pass into
virtual methods implemented in DLL.
Solution: abstract reader interface:
  struct IParameterReader {
      virtual bool ReadInt(int& out) = 0;
      virtual bool ReadFloat(float& out) = 0;
      virtual bool ReadString(const char*& out) = 0;
      virtual ~IParameterReader() = default;
  };
Concrete IStreamReader wrapping std::istream lives header-only in consumer. ✅

## std::pair across boundaries
Safe if contained types are stable (int, float, const char*).
Unsafe if it contains std::string.
Prefer a named POD struct over std::pair for virtual method returns:
  struct Range { int min; int max; };  // trivial, stable, register-friendly ✅
  auto [min, max] = command->GetRange(); // structured bindings still work

## Summary: what to apply to Command/Macro

  Command:
  - operator new/delete overridden → all derived classes (user + plugins) safe ✅
  - virtual const char* Name()/Call() instead of std::string ✅
  - IParameterReader instead of std::istream& ✅
  - dclspc on class still needed for vtable export and non-inline methods

  Macro:
  - Header-only → STL members instantiated in consumer, no boundary ✅
  - DllAllocator on deque/map → safe when Macro itself is constructed in DLL ✅
  - Helper functions (operator<<, operator>>, ReadCommandFrom) inline in header ✅
  - Template method Remove<T>() already header-only by necessity ✅