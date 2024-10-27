// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/runtime/protobuf_context.hpp"

#include <google/protobuf/stubs/common.h>

#include "tactile/runtime/logging.hpp"

namespace tactile::runtime {

ProtobufContext::ProtobufContext()
{
  TACTILE_RUNTIME_TRACE("Initializing Protobuf library");
  GOOGLE_PROTOBUF_VERIFY_VERSION;
}

ProtobufContext::~ProtobufContext() noexcept
{
  try {
    google::protobuf::ShutdownProtobufLibrary();
  }
  catch (...) {
    TACTILE_RUNTIME_ERROR("Protobuf shutdown failed");
  }
}

}  // namespace tactile::runtime
