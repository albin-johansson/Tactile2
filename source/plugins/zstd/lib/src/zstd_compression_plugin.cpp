// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/zstd/zstd_compression_plugin.hpp"

#include <new>  // nothrow

#include "tactile/base/runtime/runtime.hpp"
#include "tactile/zstd/logging.hpp"

namespace tactile {

void ZstdCompressionPlugin::load(IRuntime* runtime)
{
  mRuntime = runtime;
  zstd::set_logger(mRuntime->get_logger());

  mCompressor = std::make_unique<ZstdCompressionFormat>();
  mRuntime->set_compression_format(CompressionFormatId::kZstd, mCompressor.get());
}

void ZstdCompressionPlugin::unload()
{
  mRuntime->set_compression_format(CompressionFormatId::kZstd, nullptr);
  mCompressor.reset();

  zstd::set_logger(nullptr);
  mRuntime = nullptr;
}

auto tactile_make_plugin() -> IPlugin*
{
  return new (std::nothrow) ZstdCompressionPlugin {};
}

void tactile_free_plugin(IPlugin* plugin)
{
  delete plugin;
}

}  // namespace tactile
