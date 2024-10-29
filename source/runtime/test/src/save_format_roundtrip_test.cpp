// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include <array>        // array
#include <filesystem>   // current_path, create_directories
#include <optional>     // optional
#include <ostream>      // ostream
#include <string_view>  // string_view

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "tactile/base/io/save/save_format.hpp"
#include "tactile/null_renderer/null_renderer_plugin.hpp"
#include "tactile/runtime/command_line_options.hpp"
#include "tactile/runtime/document_factory.hpp"
#include "tactile/runtime/runtime_impl.hpp"
#include "tactile/test_util/document_view_mocks.hpp"
#include "tactile/test_util/ir.hpp"
#include "tactile/test_util/ir_eq.hpp"
#include "tactile/test_util/ir_presets.hpp"

#ifdef TACTILE_HAS_TILED_TMX
  #include "tactile/tiled_tmx/tmx_format_plugin.hpp"
#endif

#ifdef TACTILE_HAS_TILED_TMJ
  #include "tactile/tiled_tmj/tmj_format_plugin.hpp"
#endif

#ifdef TACTILE_HAS_ZLIB
  #include "tactile/zlib/zlib_compression_plugin.hpp"
#endif

#ifdef TACTILE_HAS_ZSTD
  #include "tactile/zstd/zstd_compression_plugin.hpp"
#endif

namespace tactile::runtime {
namespace {

struct SaveFormatRoundtripCfg final
{
  SaveFormatId format_id;
  std::string_view map_filename;
  TileEncoding encoding;
  std::optional<CompressionFormatId> compression;
  bool use_external_tilesets;
};

auto operator<<(std::ostream& stream, const SaveFormatRoundtripCfg& config) -> std::ostream&
{
  stream << config.map_filename;

  stream << " + " << (config.encoding == TileEncoding::kBase64 ? "base64" : "plain text")
         << " encoding";

  if (config.compression == CompressionFormatId::kZlib) {
    stream << " + zlib compression";
  }
  else if (config.compression == CompressionFormatId::kZstd) {
    stream << " + zstd compression";
  }
  else {
    stream << " + no compression";
  }

  stream << " + " << (config.use_external_tilesets ? "external" : "embedded") << " tilesets";

  return stream;
}

class SaveFormatRoundtripTest : public testing::TestWithParam<SaveFormatRoundtripCfg>
{
 public:
  void SetUp() override
  {
    m_null_renderer_plugin.load(&m_runtime);

#ifdef TACTILE_HAS_ZLIB
    m_zlib_compression_plugin.load(&m_runtime);
#endif  // TACTILE_HAS_ZLIB

#ifdef TACTILE_HAS_ZSTD
    m_zstd_compression_plugin.load(&m_runtime);
#endif  // TACTILE_HAS_ZSTD

#ifdef TACTILE_HAS_TILED_TMX
    m_tmx_format_plugin.load(&m_runtime);
#endif

#ifdef TACTILE_HAS_TILED_TMJ
    m_tmj_format_plugin.load(&m_runtime);
#endif

    m_renderer = m_runtime.get_renderer();
    ASSERT_NE(m_renderer, nullptr);
  }

  void TearDown() override
  {
#ifdef TACTILE_HAS_TILED_TMX
    m_tmx_format_plugin.unload();
#endif

#ifdef TACTILE_HAS_TILED_TMJ
    m_tmj_format_plugin.unload();
#endif

#ifdef TACTILE_HAS_ZSTD
    m_zstd_compression_plugin.unload();
#endif

#ifdef TACTILE_HAS_ZLIB
    m_zlib_compression_plugin.unload();
#endif

    m_null_renderer_plugin.unload();
  }

 protected:
  RuntimeImpl m_runtime {_get_command_line_options()};

  NullRendererPlugin m_null_renderer_plugin {};

#ifdef TACTILE_HAS_ZLIB
  zlib::ZlibCompressionPlugin m_zlib_compression_plugin {};
#endif  // TACTILE_HAS_ZLIB

#ifdef TACTILE_HAS_ZSTD
  zstd::ZstdCompressionPlugin m_zstd_compression_plugin {};
#endif  // TACTILE_HAS_ZSTD

#ifdef TACTILE_HAS_TILED_TMX
  tiled_tmx::TmxFormatPlugin m_tmx_format_plugin {};
#endif

#ifdef TACTILE_HAS_TILED_TMJ
  tiled_tmj::TmjFormatPlugin m_tmj_format_plugin {};
#endif

  IRenderer* m_renderer {};

  [[nodiscard]]
  static auto _get_command_line_options() noexcept -> CommandLineOptions
  {
    auto options = get_default_command_line_options();
    options.log_level = log::LogLevel::kTrace;
    return options;
  }
};

#if TACTILE_HAS_TILED_TMJ

inline constexpr std::array kTiledTmjRoundtripTests = {
  SaveFormatRoundtripCfg {
    .format_id = SaveFormatId::kTiledTmj,
    .map_filename = "map_with_embedded_tilesets.tmj",
    .encoding = TileEncoding::kPlainText,
    .compression = std::nullopt,
    .use_external_tilesets = false,
  },
  SaveFormatRoundtripCfg {
    .format_id = SaveFormatId::kTiledTmj,
    .map_filename = "map_with_external_tilesets.tmj",
    .encoding = TileEncoding::kPlainText,
    .compression = std::nullopt,
    .use_external_tilesets = true,
  },
  SaveFormatRoundtripCfg {
    .format_id = SaveFormatId::kTiledTmj,
    .map_filename = "map_with_base64_tiles.tmj",
    .encoding = TileEncoding::kBase64,
    .compression = std::nullopt,
    .use_external_tilesets = false,
  },
  SaveFormatRoundtripCfg {
    .format_id = SaveFormatId::kTiledTmj,
    .map_filename = "map_with_base64_zlib_tiles.tmj",
    .encoding = TileEncoding::kBase64,
    .compression = CompressionFormatId::kZlib,
    .use_external_tilesets = false,
  },
  SaveFormatRoundtripCfg {
    .format_id = SaveFormatId::kTiledTmj,
    .map_filename = "map_with_base64_zstd_tiles.tmj",
    .encoding = TileEncoding::kBase64,
    .compression = CompressionFormatId::kZstd,
    .use_external_tilesets = false,
  },
};

INSTANTIATE_TEST_SUITE_P(TiledTMJ,
                         SaveFormatRoundtripTest,
                         testing::ValuesIn(kTiledTmjRoundtripTests));

#endif

#if TACTILE_HAS_TILED_TMX

inline constexpr std::array kTiledTmxRoundtripTests = {
  SaveFormatRoundtripCfg {
    .format_id = SaveFormatId::kTiledTmx,
    .map_filename = "map_with_embedded_tilesets.tmx",
    .encoding = TileEncoding::kPlainText,
    .compression = std::nullopt,
    .use_external_tilesets = false,
  },
  SaveFormatRoundtripCfg {
    .format_id = SaveFormatId::kTiledTmx,
    .map_filename = "map_with_external_tilesets.tmx",
    .encoding = TileEncoding::kPlainText,
    .compression = std::nullopt,
    .use_external_tilesets = true,
  },
  SaveFormatRoundtripCfg {
    .format_id = SaveFormatId::kTiledTmx,
    .map_filename = "map_with_base64_tiles.tmx",
    .encoding = TileEncoding::kBase64,
    .compression = std::nullopt,
    .use_external_tilesets = false,
  },
  SaveFormatRoundtripCfg {
    .format_id = SaveFormatId::kTiledTmx,
    .map_filename = "map_with_base64_zlib_tiles.tmx",
    .encoding = TileEncoding::kBase64,
    .compression = CompressionFormatId::kZlib,
    .use_external_tilesets = false,
  },
  SaveFormatRoundtripCfg {
    .format_id = SaveFormatId::kTiledTmx,
    .map_filename = "map_with_base64_zstd_tiles.tmx",
    .encoding = TileEncoding::kBase64,
    .compression = CompressionFormatId::kZstd,
    .use_external_tilesets = false,
  },
};

INSTANTIATE_TEST_SUITE_P(TiledTMX,
                         SaveFormatRoundtripTest,
                         testing::ValuesIn(kTiledTmxRoundtripTests));

#endif

TEST_P(SaveFormatRoundtripTest, SaveAndLoadMap)
{
  const auto& config = GetParam();

  const auto* save_format = m_runtime.get_save_format(config.format_id);
  ASSERT_NE(save_format, nullptr);

  auto ir_map = test::make_complex_ir_map(ir::TileFormat {
    .encoding = config.encoding,
    .compression = config.compression,
    .compression_level = std::nullopt,
  });

  for (auto& ir_tileset_ref : ir_map.tilesets) {
    ir_tileset_ref.tileset.is_embedded = !config.use_external_tilesets;
  }

  const auto map_document = make_map_document(*m_renderer, ir_map);
  ASSERT_NE(map_document, nullptr);

  const auto map_view = make_map_view(*map_document);
  ASSERT_NE(map_view, nullptr);

  const auto map_dir = std::filesystem::current_path() / "tests" / "runtime" / "roundtrip";
  std::filesystem::create_directories(map_dir);

  const auto map_path = map_dir / config.map_filename;
  map_document->set_path(map_path);

  const SaveFormatWriteOptions write_options {
    .base_dir = map_dir,
    .use_external_tilesets = config.use_external_tilesets,
    .use_indentation = true,
    .fold_tile_layer_data = false,
  };

  const auto save_result = save_format->save_map(*map_view, write_options);
  ASSERT_TRUE(save_result.has_value()) << "Error: " << to_string(save_result.error());

  const SaveFormatReadOptions read_options {
    .base_dir = write_options.base_dir,
    .strict_mode = false,
  };

  const auto parsed_map = save_format->load_map(map_path, read_options);
  ASSERT_TRUE(parsed_map.has_value()) << "Error: " << to_string(parsed_map.error());

  test::expect_eq(ir_map,
                  *parsed_map,
                  test::kSkipMetadataNameBit | test::kSkipVectorPropertiesBit);
}

}  // namespace
}  // namespace tactile::runtime
