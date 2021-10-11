// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: settings.proto

#include "settings.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG
namespace Tactile {
namespace Proto {
constexpr Settings::Settings(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : preferred_format_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , theme_(0)

  , preferred_tile_width_(0)
  , viewport_overlay_pos_(0)

  , preferred_tile_height_(0)
  , show_grid_(false)
  , window_border_(false)
  , restore_last_session_(false)
  , restore_layout_(false)
  , command_capacity_(uint64_t{0u})
  , show_layer_dock_(false)
  , show_properties_dock_(false)
  , show_tileset_dock_(false)
  , embed_tilesets_(false)
  , human_readable_output_(false){}
struct SettingsDefaultTypeInternal {
  constexpr SettingsDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~SettingsDefaultTypeInternal() {}
  union {
    Settings _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT SettingsDefaultTypeInternal _Settings_default_instance_;
}  // namespace Proto
}  // namespace Tactile
static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_settings_2eproto[1];
static const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* file_level_enum_descriptors_settings_2eproto[2];
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_settings_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_settings_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  PROTOBUF_FIELD_OFFSET(::Tactile::Proto::Settings, _has_bits_),
  PROTOBUF_FIELD_OFFSET(::Tactile::Proto::Settings, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::Tactile::Proto::Settings, theme_),
  PROTOBUF_FIELD_OFFSET(::Tactile::Proto::Settings, preferred_tile_width_),
  PROTOBUF_FIELD_OFFSET(::Tactile::Proto::Settings, preferred_tile_height_),
  PROTOBUF_FIELD_OFFSET(::Tactile::Proto::Settings, preferred_format_),
  PROTOBUF_FIELD_OFFSET(::Tactile::Proto::Settings, command_capacity_),
  PROTOBUF_FIELD_OFFSET(::Tactile::Proto::Settings, show_grid_),
  PROTOBUF_FIELD_OFFSET(::Tactile::Proto::Settings, window_border_),
  PROTOBUF_FIELD_OFFSET(::Tactile::Proto::Settings, restore_last_session_),
  PROTOBUF_FIELD_OFFSET(::Tactile::Proto::Settings, restore_layout_),
  PROTOBUF_FIELD_OFFSET(::Tactile::Proto::Settings, show_layer_dock_),
  PROTOBUF_FIELD_OFFSET(::Tactile::Proto::Settings, show_properties_dock_),
  PROTOBUF_FIELD_OFFSET(::Tactile::Proto::Settings, show_tileset_dock_),
  PROTOBUF_FIELD_OFFSET(::Tactile::Proto::Settings, embed_tilesets_),
  PROTOBUF_FIELD_OFFSET(::Tactile::Proto::Settings, human_readable_output_),
  PROTOBUF_FIELD_OFFSET(::Tactile::Proto::Settings, viewport_overlay_pos_),
  1,
  2,
  4,
  0,
  9,
  5,
  6,
  7,
  8,
  10,
  11,
  12,
  13,
  14,
  3,
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, 21, -1, sizeof(::Tactile::Proto::Settings)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::Tactile::Proto::_Settings_default_instance_),
};

const char descriptor_table_protodef_settings_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\016settings.proto\022\rTactile.Proto\"\311\006\n\010Sett"
  "ings\022(\n\005theme\030\001 \001(\0162\024.Tactile.Proto.Them"
  "eH\000\210\001\001\022!\n\024preferred_tile_width\030\002 \001(\005H\001\210\001"
  "\001\022\"\n\025preferred_tile_height\030\003 \001(\005H\002\210\001\001\022\035\n"
  "\020preferred_format\030\020 \001(\tH\003\210\001\001\022\035\n\020command_"
  "capacity\030\021 \001(\004H\004\210\001\001\022\026\n\tshow_grid\030\022 \001(\010H\005"
  "\210\001\001\022\032\n\rwindow_border\030\023 \001(\010H\006\210\001\001\022!\n\024resto"
  "re_last_session\030\024 \001(\010H\007\210\001\001\022\033\n\016restore_la"
  "yout\030\025 \001(\010H\010\210\001\001\022\034\n\017show_layer_dock\030\026 \001(\010"
  "H\t\210\001\001\022!\n\024show_properties_dock\030\027 \001(\010H\n\210\001\001"
  "\022\036\n\021show_tileset_dock\030\030 \001(\010H\013\210\001\001\022\033\n\016embe"
  "d_tilesets\030\031 \001(\010H\014\210\001\001\022\"\n\025human_readable_"
  "output\030\032 \001(\010H\r\210\001\001\022<\n\024viewport_overlay_po"
  "s\030\033 \001(\0162\031.Tactile.Proto.OverlayPosH\016\210\001\001B"
  "\010\n\006_themeB\027\n\025_preferred_tile_widthB\030\n\026_p"
  "referred_tile_heightB\023\n\021_preferred_forma"
  "tB\023\n\021_command_capacityB\014\n\n_show_gridB\020\n\016"
  "_window_borderB\027\n\025_restore_last_sessionB"
  "\021\n\017_restore_layoutB\022\n\020_show_layer_dockB\027"
  "\n\025_show_properties_dockB\024\n\022_show_tileset"
  "_dockB\021\n\017_embed_tilesetsB\030\n\026_human_reada"
  "ble_outputB\027\n\025_viewport_overlay_pos*L\n\nO"
  "verlayPos\022\014\n\010TOP_LEFT\020\000\022\r\n\tTOP_RIGHT\020\001\022\017"
  "\n\013BOTTOM_LEFT\020\002\022\020\n\014BOTTOM_RIGHT\020\003*|\n\005The"
  "me\022\r\n\tDEAR_DARK\020\000\022\016\n\nDEAR_LIGHT\020\001\022\010\n\004RUB"
  "Y\020\002\022\014\n\010SAPPHIRE\020\003\022\013\n\007EMERALD\020\004\022\014\n\010AMETHY"
  "ST\020\005\022\t\n\005AMBER\020\006\022\r\n\tNOCTURNAL\020\007\022\007\n\003ASH\020\010b"
  "\006proto3"
  ;
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_settings_2eproto_once;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_settings_2eproto = {
  false, false, 1087, descriptor_table_protodef_settings_2eproto, "settings.proto", 
  &descriptor_table_settings_2eproto_once, nullptr, 0, 1,
  schemas, file_default_instances, TableStruct_settings_2eproto::offsets,
  file_level_metadata_settings_2eproto, file_level_enum_descriptors_settings_2eproto, file_level_service_descriptors_settings_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable* descriptor_table_settings_2eproto_getter() {
  return &descriptor_table_settings_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY static ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptorsRunner dynamic_init_dummy_settings_2eproto(&descriptor_table_settings_2eproto);
namespace Tactile {
namespace Proto {
const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* OverlayPos_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_settings_2eproto);
  return file_level_enum_descriptors_settings_2eproto[0];
}
bool OverlayPos_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
      return true;
    default:
      return false;
  }
}

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* Theme_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_settings_2eproto);
  return file_level_enum_descriptors_settings_2eproto[1];
}
bool Theme_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
      return true;
    default:
      return false;
  }
}


// ===================================================================

class Settings::_Internal {
 public:
  using HasBits = decltype(std::declval<Settings>()._has_bits_);
  static void set_has_theme(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_preferred_tile_width(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
  static void set_has_preferred_tile_height(HasBits* has_bits) {
    (*has_bits)[0] |= 16u;
  }
  static void set_has_preferred_format(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_command_capacity(HasBits* has_bits) {
    (*has_bits)[0] |= 512u;
  }
  static void set_has_show_grid(HasBits* has_bits) {
    (*has_bits)[0] |= 32u;
  }
  static void set_has_window_border(HasBits* has_bits) {
    (*has_bits)[0] |= 64u;
  }
  static void set_has_restore_last_session(HasBits* has_bits) {
    (*has_bits)[0] |= 128u;
  }
  static void set_has_restore_layout(HasBits* has_bits) {
    (*has_bits)[0] |= 256u;
  }
  static void set_has_show_layer_dock(HasBits* has_bits) {
    (*has_bits)[0] |= 1024u;
  }
  static void set_has_show_properties_dock(HasBits* has_bits) {
    (*has_bits)[0] |= 2048u;
  }
  static void set_has_show_tileset_dock(HasBits* has_bits) {
    (*has_bits)[0] |= 4096u;
  }
  static void set_has_embed_tilesets(HasBits* has_bits) {
    (*has_bits)[0] |= 8192u;
  }
  static void set_has_human_readable_output(HasBits* has_bits) {
    (*has_bits)[0] |= 16384u;
  }
  static void set_has_viewport_overlay_pos(HasBits* has_bits) {
    (*has_bits)[0] |= 8u;
  }
};

Settings::Settings(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor();
  if (!is_message_owned) {
    RegisterArenaDtor(arena);
  }
  // @@protoc_insertion_point(arena_constructor:Tactile.Proto.Settings)
}
Settings::Settings(const Settings& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  preferred_format_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_preferred_format()) {
    preferred_format_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_preferred_format(), 
      GetArenaForAllocation());
  }
  ::memcpy(&theme_, &from.theme_,
    static_cast<size_t>(reinterpret_cast<char*>(&human_readable_output_) -
    reinterpret_cast<char*>(&theme_)) + sizeof(human_readable_output_));
  // @@protoc_insertion_point(copy_constructor:Tactile.Proto.Settings)
}

void Settings::SharedCtor() {
preferred_format_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&theme_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&human_readable_output_) -
    reinterpret_cast<char*>(&theme_)) + sizeof(human_readable_output_));
}

Settings::~Settings() {
  // @@protoc_insertion_point(destructor:Tactile.Proto.Settings)
  if (GetArenaForAllocation() != nullptr) return;
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

inline void Settings::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  preferred_format_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void Settings::ArenaDtor(void* object) {
  Settings* _this = reinterpret_cast< Settings* >(object);
  (void)_this;
}
void Settings::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void Settings::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void Settings::Clear() {
// @@protoc_insertion_point(message_clear_start:Tactile.Proto.Settings)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    preferred_format_.ClearNonDefaultToEmpty();
  }
  if (cached_has_bits & 0x000000feu) {
    ::memset(&theme_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&restore_last_session_) -
        reinterpret_cast<char*>(&theme_)) + sizeof(restore_last_session_));
  }
  if (cached_has_bits & 0x00007f00u) {
    ::memset(&restore_layout_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&human_readable_output_) -
        reinterpret_cast<char*>(&restore_layout_)) + sizeof(human_readable_output_));
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* Settings::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // optional .Tactile.Proto.Theme theme = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          ::PROTOBUF_NAMESPACE_ID::uint64 val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
          _internal_set_theme(static_cast<::Tactile::Proto::Theme>(val));
        } else
          goto handle_unusual;
        continue;
      // optional int32 preferred_tile_width = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          _Internal::set_has_preferred_tile_width(&has_bits);
          preferred_tile_width_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // optional int32 preferred_tile_height = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 24)) {
          _Internal::set_has_preferred_tile_height(&has_bits);
          preferred_tile_height_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // optional string preferred_format = 16;
      case 16:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 130)) {
          auto str = _internal_mutable_preferred_format();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "Tactile.Proto.Settings.preferred_format"));
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // optional uint64 command_capacity = 17;
      case 17:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 136)) {
          _Internal::set_has_command_capacity(&has_bits);
          command_capacity_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // optional bool show_grid = 18;
      case 18:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 144)) {
          _Internal::set_has_show_grid(&has_bits);
          show_grid_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // optional bool window_border = 19;
      case 19:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 152)) {
          _Internal::set_has_window_border(&has_bits);
          window_border_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // optional bool restore_last_session = 20;
      case 20:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 160)) {
          _Internal::set_has_restore_last_session(&has_bits);
          restore_last_session_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // optional bool restore_layout = 21;
      case 21:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 168)) {
          _Internal::set_has_restore_layout(&has_bits);
          restore_layout_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // optional bool show_layer_dock = 22;
      case 22:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 176)) {
          _Internal::set_has_show_layer_dock(&has_bits);
          show_layer_dock_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // optional bool show_properties_dock = 23;
      case 23:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 184)) {
          _Internal::set_has_show_properties_dock(&has_bits);
          show_properties_dock_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // optional bool show_tileset_dock = 24;
      case 24:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 192)) {
          _Internal::set_has_show_tileset_dock(&has_bits);
          show_tileset_dock_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // optional bool embed_tilesets = 25;
      case 25:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 200)) {
          _Internal::set_has_embed_tilesets(&has_bits);
          embed_tilesets_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // optional bool human_readable_output = 26;
      case 26:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 208)) {
          _Internal::set_has_human_readable_output(&has_bits);
          human_readable_output_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // optional .Tactile.Proto.OverlayPos viewport_overlay_pos = 27;
      case 27:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 216)) {
          ::PROTOBUF_NAMESPACE_ID::uint64 val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
          _internal_set_viewport_overlay_pos(static_cast<::Tactile::Proto::OverlayPos>(val));
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* Settings::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:Tactile.Proto.Settings)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // optional .Tactile.Proto.Theme theme = 1;
  if (_internal_has_theme()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteEnumToArray(
      1, this->_internal_theme(), target);
  }

  // optional int32 preferred_tile_width = 2;
  if (_internal_has_preferred_tile_width()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(2, this->_internal_preferred_tile_width(), target);
  }

  // optional int32 preferred_tile_height = 3;
  if (_internal_has_preferred_tile_height()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(3, this->_internal_preferred_tile_height(), target);
  }

  // optional string preferred_format = 16;
  if (_internal_has_preferred_format()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_preferred_format().data(), static_cast<int>(this->_internal_preferred_format().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "Tactile.Proto.Settings.preferred_format");
    target = stream->WriteStringMaybeAliased(
        16, this->_internal_preferred_format(), target);
  }

  // optional uint64 command_capacity = 17;
  if (_internal_has_command_capacity()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt64ToArray(17, this->_internal_command_capacity(), target);
  }

  // optional bool show_grid = 18;
  if (_internal_has_show_grid()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(18, this->_internal_show_grid(), target);
  }

  // optional bool window_border = 19;
  if (_internal_has_window_border()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(19, this->_internal_window_border(), target);
  }

  // optional bool restore_last_session = 20;
  if (_internal_has_restore_last_session()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(20, this->_internal_restore_last_session(), target);
  }

  // optional bool restore_layout = 21;
  if (_internal_has_restore_layout()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(21, this->_internal_restore_layout(), target);
  }

  // optional bool show_layer_dock = 22;
  if (_internal_has_show_layer_dock()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(22, this->_internal_show_layer_dock(), target);
  }

  // optional bool show_properties_dock = 23;
  if (_internal_has_show_properties_dock()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(23, this->_internal_show_properties_dock(), target);
  }

  // optional bool show_tileset_dock = 24;
  if (_internal_has_show_tileset_dock()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(24, this->_internal_show_tileset_dock(), target);
  }

  // optional bool embed_tilesets = 25;
  if (_internal_has_embed_tilesets()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(25, this->_internal_embed_tilesets(), target);
  }

  // optional bool human_readable_output = 26;
  if (_internal_has_human_readable_output()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(26, this->_internal_human_readable_output(), target);
  }

  // optional .Tactile.Proto.OverlayPos viewport_overlay_pos = 27;
  if (_internal_has_viewport_overlay_pos()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteEnumToArray(
      27, this->_internal_viewport_overlay_pos(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Tactile.Proto.Settings)
  return target;
}

size_t Settings::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:Tactile.Proto.Settings)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {
    // optional string preferred_format = 16;
    if (cached_has_bits & 0x00000001u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_preferred_format());
    }

    // optional .Tactile.Proto.Theme theme = 1;
    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::EnumSize(this->_internal_theme());
    }

    // optional int32 preferred_tile_width = 2;
    if (cached_has_bits & 0x00000004u) {
      total_size += ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32SizePlusOne(this->_internal_preferred_tile_width());
    }

    // optional .Tactile.Proto.OverlayPos viewport_overlay_pos = 27;
    if (cached_has_bits & 0x00000008u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::EnumSize(this->_internal_viewport_overlay_pos());
    }

    // optional int32 preferred_tile_height = 3;
    if (cached_has_bits & 0x00000010u) {
      total_size += ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32SizePlusOne(this->_internal_preferred_tile_height());
    }

    // optional bool show_grid = 18;
    if (cached_has_bits & 0x00000020u) {
      total_size += 2 + 1;
    }

    // optional bool window_border = 19;
    if (cached_has_bits & 0x00000040u) {
      total_size += 2 + 1;
    }

    // optional bool restore_last_session = 20;
    if (cached_has_bits & 0x00000080u) {
      total_size += 2 + 1;
    }

  }
  if (cached_has_bits & 0x00007f00u) {
    // optional bool restore_layout = 21;
    if (cached_has_bits & 0x00000100u) {
      total_size += 2 + 1;
    }

    // optional uint64 command_capacity = 17;
    if (cached_has_bits & 0x00000200u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt64Size(
          this->_internal_command_capacity());
    }

    // optional bool show_layer_dock = 22;
    if (cached_has_bits & 0x00000400u) {
      total_size += 2 + 1;
    }

    // optional bool show_properties_dock = 23;
    if (cached_has_bits & 0x00000800u) {
      total_size += 2 + 1;
    }

    // optional bool show_tileset_dock = 24;
    if (cached_has_bits & 0x00001000u) {
      total_size += 2 + 1;
    }

    // optional bool embed_tilesets = 25;
    if (cached_has_bits & 0x00002000u) {
      total_size += 2 + 1;
    }

    // optional bool human_readable_output = 26;
    if (cached_has_bits & 0x00004000u) {
      total_size += 2 + 1;
    }

  }
  return MaybeComputeUnknownFieldsSize(total_size, &_cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData Settings::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSizeCheck,
    Settings::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*Settings::GetClassData() const { return &_class_data_; }

void Settings::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message* to,
                      const ::PROTOBUF_NAMESPACE_ID::Message& from) {
  static_cast<Settings *>(to)->MergeFrom(
      static_cast<const Settings &>(from));
}


void Settings::MergeFrom(const Settings& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:Tactile.Proto.Settings)
  GOOGLE_DCHECK_NE(&from, this);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {
    if (cached_has_bits & 0x00000001u) {
      _internal_set_preferred_format(from._internal_preferred_format());
    }
    if (cached_has_bits & 0x00000002u) {
      theme_ = from.theme_;
    }
    if (cached_has_bits & 0x00000004u) {
      preferred_tile_width_ = from.preferred_tile_width_;
    }
    if (cached_has_bits & 0x00000008u) {
      viewport_overlay_pos_ = from.viewport_overlay_pos_;
    }
    if (cached_has_bits & 0x00000010u) {
      preferred_tile_height_ = from.preferred_tile_height_;
    }
    if (cached_has_bits & 0x00000020u) {
      show_grid_ = from.show_grid_;
    }
    if (cached_has_bits & 0x00000040u) {
      window_border_ = from.window_border_;
    }
    if (cached_has_bits & 0x00000080u) {
      restore_last_session_ = from.restore_last_session_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
  if (cached_has_bits & 0x00007f00u) {
    if (cached_has_bits & 0x00000100u) {
      restore_layout_ = from.restore_layout_;
    }
    if (cached_has_bits & 0x00000200u) {
      command_capacity_ = from.command_capacity_;
    }
    if (cached_has_bits & 0x00000400u) {
      show_layer_dock_ = from.show_layer_dock_;
    }
    if (cached_has_bits & 0x00000800u) {
      show_properties_dock_ = from.show_properties_dock_;
    }
    if (cached_has_bits & 0x00001000u) {
      show_tileset_dock_ = from.show_tileset_dock_;
    }
    if (cached_has_bits & 0x00002000u) {
      embed_tilesets_ = from.embed_tilesets_;
    }
    if (cached_has_bits & 0x00004000u) {
      human_readable_output_ = from.human_readable_output_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void Settings::CopyFrom(const Settings& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Tactile.Proto.Settings)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Settings::IsInitialized() const {
  return true;
}

void Settings::InternalSwap(Settings* other) {
  using std::swap;
  auto* lhs_arena = GetArenaForAllocation();
  auto* rhs_arena = other->GetArenaForAllocation();
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      &preferred_format_, lhs_arena,
      &other->preferred_format_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(Settings, human_readable_output_)
      + sizeof(Settings::human_readable_output_)
      - PROTOBUF_FIELD_OFFSET(Settings, theme_)>(
          reinterpret_cast<char*>(&theme_),
          reinterpret_cast<char*>(&other->theme_));
}

::PROTOBUF_NAMESPACE_ID::Metadata Settings::GetMetadata() const {
  return ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(
      &descriptor_table_settings_2eproto_getter, &descriptor_table_settings_2eproto_once,
      file_level_metadata_settings_2eproto[0]);
}

// @@protoc_insertion_point(namespace_scope)
}  // namespace Proto
}  // namespace Tactile
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::Tactile::Proto::Settings* Arena::CreateMaybeMessage< ::Tactile::Proto::Settings >(Arena* arena) {
  return Arena::CreateMessageInternal< ::Tactile::Proto::Settings >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
