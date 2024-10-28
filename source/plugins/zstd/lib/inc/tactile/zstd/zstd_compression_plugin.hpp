// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <memory>  // unique_ptr

#include "tactile/base/io/compress/compression_format.hpp"
#include "tactile/base/runtime/plugin.hpp"
#include "tactile/zstd/api.hpp"

namespace tactile::zstd {

/**
 * Manages the Zstd compression plugin.
 */
class TACTILE_ZSTD_API ZstdCompressionPlugin final : public IPlugin
{
 public:
  void load(IRuntime* runtime) override;

  void unload() override;

 private:
  IRuntime* m_runtime {};
  std::unique_ptr<ICompressionFormat> m_format {};
};

extern "C"
{
  TACTILE_ZSTD_API auto tactile_make_plugin() -> IPlugin*;
  TACTILE_ZSTD_API void tactile_free_plugin(IPlugin* plugin);
}

}  // namespace tactile::zstd
