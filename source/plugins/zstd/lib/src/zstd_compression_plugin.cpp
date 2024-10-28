// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/zstd/zstd_compression_plugin.hpp"

#include <new>  // nothrow

#include "tactile/base/runtime/runtime.hpp"
#include "tactile/zstd/logging.hpp"
#include "tactile/zstd/zstd_compression_format.hpp"

namespace tactile::zstd {

void ZstdCompressionPlugin::load(IRuntime* runtime)
{
  m_runtime = runtime;
  set_logger(m_runtime->get_logger());

  m_format = std::make_unique<ZstdCompressionFormat>();
  m_runtime->set_compression_format(CompressionFormatId::kZstd, m_format.get());
}

void ZstdCompressionPlugin::unload()
{
  m_runtime->set_compression_format(CompressionFormatId::kZstd, nullptr);
  m_format.reset();

  set_logger(nullptr);
  m_runtime = nullptr;
}

auto tactile_make_plugin() -> IPlugin*
{
  return new (std::nothrow) ZstdCompressionPlugin {};
}

void tactile_free_plugin(IPlugin* plugin)
{
  delete plugin;
}

}  // namespace tactile::zstd
