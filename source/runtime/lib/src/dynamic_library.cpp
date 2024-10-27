// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/runtime/dynamic_library.hpp"

#if TACTILE_OS_LINUX || TACTILE_OS_APPLE
  #include <dlfcn.h>
#endif

#if TACTILE_OS_WINDOWS
  #include <windows.h>
#endif

#include "tactile/runtime/logging.hpp"

namespace tactile::runtime {
namespace {

#if TACTILE_OS_LINUX || TACTILE_OS_APPLE

class UnixDynamicLibrary final : public IDynamicLibrary
{
 public:
  TACTILE_DELETE_COPY(UnixDynamicLibrary);
  TACTILE_DELETE_MOVE(UnixDynamicLibrary);

  explicit UnixDynamicLibrary(void* handle) noexcept
    : mHandle {handle}
  {}

  ~UnixDynamicLibrary() noexcept override
  {
    if (mHandle) {
      dlclose(mHandle);
      mHandle = nullptr;
    }
  }

  [[nodiscard]]
  static auto load(const std::filesystem::path& path) -> std::unique_ptr<UnixDynamicLibrary>
  {
    void* handle = dlopen(path.c_str(), RTLD_LAZY);

    if (!handle) {
      TACTILE_RUNTIME_ERROR("Could not load dynamic library: {}", dlerror());
      return nullptr;
    }

    return std::make_unique<UnixDynamicLibrary>(handle);
  }

  [[nodiscard]]
  auto find_symbol(const char* name) const -> void* override
  {
    return dlsym(mHandle, name);
  }

 private:
  void* mHandle;
};

#endif  // TACTILE_OS_LINUX || TACTILE_OS_APPLE

#if TACTILE_OS_WINDOWS

class Win32DynamicLibrary final : public IDynamicLibrary
{
 public:
  TACTILE_DELETE_COPY(Win32DynamicLibrary);
  TACTILE_DELETE_MOVE(Win32DynamicLibrary);

  explicit Win32DynamicLibrary(HMODULE handle)
    : mHandle {handle}
  {}

  ~Win32DynamicLibrary() noexcept override
  {
    if (mHandle) {
      FreeLibrary(mHandle);
      mHandle = nullptr;
    }
  }

  [[nodiscard]]
  static auto load(const std::filesystem::path& path) -> std::unique_ptr<Win32DynamicLibrary>
  {
    const auto handle = LoadLibraryA(path.string().c_str());

    if (!handle) {
      TACTILE_RUNTIME_ERROR("Could not load dynamic library: {}", GetLastError());
      return nullptr;
    }

    return std::make_unique<Win32DynamicLibrary>(handle);
  }

  [[nodiscard]]
  auto find_symbol(const char* name) const -> void* override
  {
    return static_cast<void*>(GetProcAddress(mHandle, name));
  }

 private:
  HMODULE mHandle;
};

#endif  // TACTILE_OS_WINDOWS

}  // namespace

auto load_library(const std::filesystem::path& path) -> std::unique_ptr<IDynamicLibrary>
{
#if TACTILE_OS_LINUX || TACTILE_OS_APPLE
  return UnixDynamicLibrary::load(path);
#elif TACTILE_OS_WINDOWS
  return Win32DynamicLibrary::load(path);
#else
  return nullptr;
#endif  // TACTILE_OS_WINDOWS
}

}  // namespace tactile::runtime
