// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/zlib/zlib_compression_plugin.hpp"

#include <new>  // nothrow

#include "tactile/base/runtime/runtime.hpp"
#include "tactile/zlib/logging.hpp"
#include "tactile/zlib/zlib_compression_format.hpp"

namespace tactile::zlib {

void ZlibCompressionPlugin::load(IRuntime* runtime)
{
  m_runtime = runtime;
  set_logger(m_runtime->get_logger());

  m_format = std::make_unique<ZlibCompressionFormat>();
  m_runtime->set_compression_format(CompressionFormatId::kZlib, m_format.get());
}

void ZlibCompressionPlugin::unload()
{
  set_logger(nullptr);

  m_runtime->set_compression_format(CompressionFormatId::kZlib, nullptr);
  m_runtime = nullptr;

  m_format.reset();
}

auto tactile_make_plugin() -> IPlugin*
{
  return new (std::nothrow) ZlibCompressionPlugin {};
}

void tactile_free_plugin(IPlugin* plugin)
{
  delete plugin;
}

}  // namespace tactile::zlib
