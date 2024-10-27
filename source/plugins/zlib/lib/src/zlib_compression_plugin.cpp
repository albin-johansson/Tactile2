// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/zlib/zlib_compression_plugin.hpp"

#include <new>  // nothrow

#include "tactile/base/runtime/runtime.hpp"
#include "tactile/zlib/logging.hpp"

namespace tactile {

void ZlibCompressionPlugin::load(IRuntime* runtime)
{
  mRuntime = runtime;
  zlib::set_logger(mRuntime->get_logger());

  mCompressor = std::make_unique<ZlibCompressionFormat>();
  mRuntime->set_compression_format(CompressionFormatId::kZlib, mCompressor.get());
}

void ZlibCompressionPlugin::unload()
{
  zlib::set_logger(nullptr);

  mRuntime->set_compression_format(CompressionFormatId::kZlib, nullptr);
  mRuntime = nullptr;

  mCompressor.reset();
}

auto tactile_make_plugin() -> IPlugin*
{
  return new (std::nothrow) ZlibCompressionPlugin {};
}

void tactile_free_plugin(IPlugin* plugin)
{
  delete plugin;
}

}  // namespace tactile
