// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: settings.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_settings_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_settings_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3018000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3018000 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_settings_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_settings_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxiliaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[1]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_settings_2eproto;
namespace Tactile {
namespace ProtoBuf {
class Settings;
struct SettingsDefaultTypeInternal;
extern SettingsDefaultTypeInternal _Settings_default_instance_;
}  // namespace ProtoBuf
}  // namespace Tactile
PROTOBUF_NAMESPACE_OPEN
template<> ::Tactile::ProtoBuf::Settings* Arena::CreateMaybeMessage<::Tactile::ProtoBuf::Settings>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace Tactile {
namespace ProtoBuf {

enum OverlayPos : int {
  TOP_LEFT = 0,
  TOP_RIGHT = 1,
  BOTTOM_LEFT = 2,
  BOTTOM_RIGHT = 3,
  OverlayPos_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  OverlayPos_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool OverlayPos_IsValid(int value);
constexpr OverlayPos OverlayPos_MIN = TOP_LEFT;
constexpr OverlayPos OverlayPos_MAX = BOTTOM_RIGHT;
constexpr int OverlayPos_ARRAYSIZE = OverlayPos_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* OverlayPos_descriptor();
template<typename T>
inline const std::string& OverlayPos_Name(T enum_t_value) {
  static_assert(::std::is_same<T, OverlayPos>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function OverlayPos_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    OverlayPos_descriptor(), enum_t_value);
}
inline bool OverlayPos_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, OverlayPos* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<OverlayPos>(
    OverlayPos_descriptor(), name, value);
}
// ===================================================================

class Settings final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:Tactile.ProtoBuf.Settings) */ {
 public:
  inline Settings() : Settings(nullptr) {}
  ~Settings() override;
  explicit constexpr Settings(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  Settings(const Settings& from);
  Settings(Settings&& from) noexcept
    : Settings() {
    *this = ::std::move(from);
  }

  inline Settings& operator=(const Settings& from) {
    CopyFrom(from);
    return *this;
  }
  inline Settings& operator=(Settings&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const Settings& default_instance() {
    return *internal_default_instance();
  }
  static inline const Settings* internal_default_instance() {
    return reinterpret_cast<const Settings*>(
               &_Settings_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(Settings& a, Settings& b) {
    a.Swap(&b);
  }
  inline void Swap(Settings* other) {
    if (other == this) return;
    if (GetOwningArena() == other->GetOwningArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(Settings* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline Settings* New() const final {
    return new Settings();
  }

  Settings* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<Settings>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const Settings& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom(const Settings& from);
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message* to, const ::PROTOBUF_NAMESPACE_ID::Message& from);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Settings* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "Tactile.ProtoBuf.Settings";
  }
  protected:
  explicit Settings(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kThemeFieldNumber = 1,
    kPreferredFormatFieldNumber = 16,
    kPreferredTileWidthFieldNumber = 2,
    kPreferredTileHeightFieldNumber = 3,
    kCommandCapacityFieldNumber = 17,
    kShowGridFieldNumber = 18,
    kWindowBorderFieldNumber = 19,
    kRestoreLastSessionFieldNumber = 20,
    kRestoreLayoutFieldNumber = 21,
    kShowLayerDockFieldNumber = 22,
    kShowPropertiesDockFieldNumber = 23,
    kShowTilesetDockFieldNumber = 24,
    kEmbedTilesetsFieldNumber = 25,
    kHumanReadableOutputFieldNumber = 26,
    kViewportOverlayPosFieldNumber = 27,
  };
  // optional string theme = 1;
  bool has_theme() const;
  private:
  bool _internal_has_theme() const;
  public:
  void clear_theme();
  const std::string& theme() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_theme(ArgT0&& arg0, ArgT... args);
  std::string* mutable_theme();
  PROTOBUF_MUST_USE_RESULT std::string* release_theme();
  void set_allocated_theme(std::string* theme);
  private:
  const std::string& _internal_theme() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_theme(const std::string& value);
  std::string* _internal_mutable_theme();
  public:

  // optional string preferred_format = 16;
  bool has_preferred_format() const;
  private:
  bool _internal_has_preferred_format() const;
  public:
  void clear_preferred_format();
  const std::string& preferred_format() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_preferred_format(ArgT0&& arg0, ArgT... args);
  std::string* mutable_preferred_format();
  PROTOBUF_MUST_USE_RESULT std::string* release_preferred_format();
  void set_allocated_preferred_format(std::string* preferred_format);
  private:
  const std::string& _internal_preferred_format() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_preferred_format(const std::string& value);
  std::string* _internal_mutable_preferred_format();
  public:

  // optional int32 preferred_tile_width = 2;
  bool has_preferred_tile_width() const;
  private:
  bool _internal_has_preferred_tile_width() const;
  public:
  void clear_preferred_tile_width();
  ::PROTOBUF_NAMESPACE_ID::int32 preferred_tile_width() const;
  void set_preferred_tile_width(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_preferred_tile_width() const;
  void _internal_set_preferred_tile_width(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // optional int32 preferred_tile_height = 3;
  bool has_preferred_tile_height() const;
  private:
  bool _internal_has_preferred_tile_height() const;
  public:
  void clear_preferred_tile_height();
  ::PROTOBUF_NAMESPACE_ID::int32 preferred_tile_height() const;
  void set_preferred_tile_height(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_preferred_tile_height() const;
  void _internal_set_preferred_tile_height(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // optional uint64 command_capacity = 17;
  bool has_command_capacity() const;
  private:
  bool _internal_has_command_capacity() const;
  public:
  void clear_command_capacity();
  ::PROTOBUF_NAMESPACE_ID::uint64 command_capacity() const;
  void set_command_capacity(::PROTOBUF_NAMESPACE_ID::uint64 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint64 _internal_command_capacity() const;
  void _internal_set_command_capacity(::PROTOBUF_NAMESPACE_ID::uint64 value);
  public:

  // optional bool show_grid = 18;
  bool has_show_grid() const;
  private:
  bool _internal_has_show_grid() const;
  public:
  void clear_show_grid();
  bool show_grid() const;
  void set_show_grid(bool value);
  private:
  bool _internal_show_grid() const;
  void _internal_set_show_grid(bool value);
  public:

  // optional bool window_border = 19;
  bool has_window_border() const;
  private:
  bool _internal_has_window_border() const;
  public:
  void clear_window_border();
  bool window_border() const;
  void set_window_border(bool value);
  private:
  bool _internal_window_border() const;
  void _internal_set_window_border(bool value);
  public:

  // optional bool restore_last_session = 20;
  bool has_restore_last_session() const;
  private:
  bool _internal_has_restore_last_session() const;
  public:
  void clear_restore_last_session();
  bool restore_last_session() const;
  void set_restore_last_session(bool value);
  private:
  bool _internal_restore_last_session() const;
  void _internal_set_restore_last_session(bool value);
  public:

  // optional bool restore_layout = 21;
  bool has_restore_layout() const;
  private:
  bool _internal_has_restore_layout() const;
  public:
  void clear_restore_layout();
  bool restore_layout() const;
  void set_restore_layout(bool value);
  private:
  bool _internal_restore_layout() const;
  void _internal_set_restore_layout(bool value);
  public:

  // optional bool show_layer_dock = 22;
  bool has_show_layer_dock() const;
  private:
  bool _internal_has_show_layer_dock() const;
  public:
  void clear_show_layer_dock();
  bool show_layer_dock() const;
  void set_show_layer_dock(bool value);
  private:
  bool _internal_show_layer_dock() const;
  void _internal_set_show_layer_dock(bool value);
  public:

  // optional bool show_properties_dock = 23;
  bool has_show_properties_dock() const;
  private:
  bool _internal_has_show_properties_dock() const;
  public:
  void clear_show_properties_dock();
  bool show_properties_dock() const;
  void set_show_properties_dock(bool value);
  private:
  bool _internal_show_properties_dock() const;
  void _internal_set_show_properties_dock(bool value);
  public:

  // optional bool show_tileset_dock = 24;
  bool has_show_tileset_dock() const;
  private:
  bool _internal_has_show_tileset_dock() const;
  public:
  void clear_show_tileset_dock();
  bool show_tileset_dock() const;
  void set_show_tileset_dock(bool value);
  private:
  bool _internal_show_tileset_dock() const;
  void _internal_set_show_tileset_dock(bool value);
  public:

  // optional bool embed_tilesets = 25;
  bool has_embed_tilesets() const;
  private:
  bool _internal_has_embed_tilesets() const;
  public:
  void clear_embed_tilesets();
  bool embed_tilesets() const;
  void set_embed_tilesets(bool value);
  private:
  bool _internal_embed_tilesets() const;
  void _internal_set_embed_tilesets(bool value);
  public:

  // optional bool human_readable_output = 26;
  bool has_human_readable_output() const;
  private:
  bool _internal_has_human_readable_output() const;
  public:
  void clear_human_readable_output();
  bool human_readable_output() const;
  void set_human_readable_output(bool value);
  private:
  bool _internal_human_readable_output() const;
  void _internal_set_human_readable_output(bool value);
  public:

  // optional .Tactile.ProtoBuf.OverlayPos viewport_overlay_pos = 27;
  bool has_viewport_overlay_pos() const;
  private:
  bool _internal_has_viewport_overlay_pos() const;
  public:
  void clear_viewport_overlay_pos();
  ::Tactile::ProtoBuf::OverlayPos viewport_overlay_pos() const;
  void set_viewport_overlay_pos(::Tactile::ProtoBuf::OverlayPos value);
  private:
  ::Tactile::ProtoBuf::OverlayPos _internal_viewport_overlay_pos() const;
  void _internal_set_viewport_overlay_pos(::Tactile::ProtoBuf::OverlayPos value);
  public:

  // @@protoc_insertion_point(class_scope:Tactile.ProtoBuf.Settings)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::HasBits<1> _has_bits_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr theme_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr preferred_format_;
  ::PROTOBUF_NAMESPACE_ID::int32 preferred_tile_width_;
  ::PROTOBUF_NAMESPACE_ID::int32 preferred_tile_height_;
  ::PROTOBUF_NAMESPACE_ID::uint64 command_capacity_;
  bool show_grid_;
  bool window_border_;
  bool restore_last_session_;
  bool restore_layout_;
  bool show_layer_dock_;
  bool show_properties_dock_;
  bool show_tileset_dock_;
  bool embed_tilesets_;
  bool human_readable_output_;
  int viewport_overlay_pos_;
  friend struct ::TableStruct_settings_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// Settings

// optional string theme = 1;
inline bool Settings::_internal_has_theme() const {
  bool value = (_has_bits_[0] & 0x00000001u) != 0;
  return value;
}
inline bool Settings::has_theme() const {
  return _internal_has_theme();
}
inline void Settings::clear_theme() {
  theme_.ClearToEmpty();
  _has_bits_[0] &= ~0x00000001u;
}
inline const std::string& Settings::theme() const {
  // @@protoc_insertion_point(field_get:Tactile.ProtoBuf.Settings.theme)
  return _internal_theme();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void Settings::set_theme(ArgT0&& arg0, ArgT... args) {
 _has_bits_[0] |= 0x00000001u;
 theme_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:Tactile.ProtoBuf.Settings.theme)
}
inline std::string* Settings::mutable_theme() {
  std::string* _s = _internal_mutable_theme();
  // @@protoc_insertion_point(field_mutable:Tactile.ProtoBuf.Settings.theme)
  return _s;
}
inline const std::string& Settings::_internal_theme() const {
  return theme_.Get();
}
inline void Settings::_internal_set_theme(const std::string& value) {
  _has_bits_[0] |= 0x00000001u;
  theme_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, value, GetArenaForAllocation());
}
inline std::string* Settings::_internal_mutable_theme() {
  _has_bits_[0] |= 0x00000001u;
  return theme_.Mutable(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, GetArenaForAllocation());
}
inline std::string* Settings::release_theme() {
  // @@protoc_insertion_point(field_release:Tactile.ProtoBuf.Settings.theme)
  if (!_internal_has_theme()) {
    return nullptr;
  }
  _has_bits_[0] &= ~0x00000001u;
  return theme_.ReleaseNonDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArenaForAllocation());
}
inline void Settings::set_allocated_theme(std::string* theme) {
  if (theme != nullptr) {
    _has_bits_[0] |= 0x00000001u;
  } else {
    _has_bits_[0] &= ~0x00000001u;
  }
  theme_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), theme,
      GetArenaForAllocation());
  // @@protoc_insertion_point(field_set_allocated:Tactile.ProtoBuf.Settings.theme)
}

// optional int32 preferred_tile_width = 2;
inline bool Settings::_internal_has_preferred_tile_width() const {
  bool value = (_has_bits_[0] & 0x00000004u) != 0;
  return value;
}
inline bool Settings::has_preferred_tile_width() const {
  return _internal_has_preferred_tile_width();
}
inline void Settings::clear_preferred_tile_width() {
  preferred_tile_width_ = 0;
  _has_bits_[0] &= ~0x00000004u;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 Settings::_internal_preferred_tile_width() const {
  return preferred_tile_width_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 Settings::preferred_tile_width() const {
  // @@protoc_insertion_point(field_get:Tactile.ProtoBuf.Settings.preferred_tile_width)
  return _internal_preferred_tile_width();
}
inline void Settings::_internal_set_preferred_tile_width(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _has_bits_[0] |= 0x00000004u;
  preferred_tile_width_ = value;
}
inline void Settings::set_preferred_tile_width(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_preferred_tile_width(value);
  // @@protoc_insertion_point(field_set:Tactile.ProtoBuf.Settings.preferred_tile_width)
}

// optional int32 preferred_tile_height = 3;
inline bool Settings::_internal_has_preferred_tile_height() const {
  bool value = (_has_bits_[0] & 0x00000008u) != 0;
  return value;
}
inline bool Settings::has_preferred_tile_height() const {
  return _internal_has_preferred_tile_height();
}
inline void Settings::clear_preferred_tile_height() {
  preferred_tile_height_ = 0;
  _has_bits_[0] &= ~0x00000008u;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 Settings::_internal_preferred_tile_height() const {
  return preferred_tile_height_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 Settings::preferred_tile_height() const {
  // @@protoc_insertion_point(field_get:Tactile.ProtoBuf.Settings.preferred_tile_height)
  return _internal_preferred_tile_height();
}
inline void Settings::_internal_set_preferred_tile_height(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _has_bits_[0] |= 0x00000008u;
  preferred_tile_height_ = value;
}
inline void Settings::set_preferred_tile_height(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_preferred_tile_height(value);
  // @@protoc_insertion_point(field_set:Tactile.ProtoBuf.Settings.preferred_tile_height)
}

// optional string preferred_format = 16;
inline bool Settings::_internal_has_preferred_format() const {
  bool value = (_has_bits_[0] & 0x00000002u) != 0;
  return value;
}
inline bool Settings::has_preferred_format() const {
  return _internal_has_preferred_format();
}
inline void Settings::clear_preferred_format() {
  preferred_format_.ClearToEmpty();
  _has_bits_[0] &= ~0x00000002u;
}
inline const std::string& Settings::preferred_format() const {
  // @@protoc_insertion_point(field_get:Tactile.ProtoBuf.Settings.preferred_format)
  return _internal_preferred_format();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void Settings::set_preferred_format(ArgT0&& arg0, ArgT... args) {
 _has_bits_[0] |= 0x00000002u;
 preferred_format_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:Tactile.ProtoBuf.Settings.preferred_format)
}
inline std::string* Settings::mutable_preferred_format() {
  std::string* _s = _internal_mutable_preferred_format();
  // @@protoc_insertion_point(field_mutable:Tactile.ProtoBuf.Settings.preferred_format)
  return _s;
}
inline const std::string& Settings::_internal_preferred_format() const {
  return preferred_format_.Get();
}
inline void Settings::_internal_set_preferred_format(const std::string& value) {
  _has_bits_[0] |= 0x00000002u;
  preferred_format_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, value, GetArenaForAllocation());
}
inline std::string* Settings::_internal_mutable_preferred_format() {
  _has_bits_[0] |= 0x00000002u;
  return preferred_format_.Mutable(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, GetArenaForAllocation());
}
inline std::string* Settings::release_preferred_format() {
  // @@protoc_insertion_point(field_release:Tactile.ProtoBuf.Settings.preferred_format)
  if (!_internal_has_preferred_format()) {
    return nullptr;
  }
  _has_bits_[0] &= ~0x00000002u;
  return preferred_format_.ReleaseNonDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArenaForAllocation());
}
inline void Settings::set_allocated_preferred_format(std::string* preferred_format) {
  if (preferred_format != nullptr) {
    _has_bits_[0] |= 0x00000002u;
  } else {
    _has_bits_[0] &= ~0x00000002u;
  }
  preferred_format_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), preferred_format,
      GetArenaForAllocation());
  // @@protoc_insertion_point(field_set_allocated:Tactile.ProtoBuf.Settings.preferred_format)
}

// optional uint64 command_capacity = 17;
inline bool Settings::_internal_has_command_capacity() const {
  bool value = (_has_bits_[0] & 0x00000010u) != 0;
  return value;
}
inline bool Settings::has_command_capacity() const {
  return _internal_has_command_capacity();
}
inline void Settings::clear_command_capacity() {
  command_capacity_ = uint64_t{0u};
  _has_bits_[0] &= ~0x00000010u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint64 Settings::_internal_command_capacity() const {
  return command_capacity_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint64 Settings::command_capacity() const {
  // @@protoc_insertion_point(field_get:Tactile.ProtoBuf.Settings.command_capacity)
  return _internal_command_capacity();
}
inline void Settings::_internal_set_command_capacity(::PROTOBUF_NAMESPACE_ID::uint64 value) {
  _has_bits_[0] |= 0x00000010u;
  command_capacity_ = value;
}
inline void Settings::set_command_capacity(::PROTOBUF_NAMESPACE_ID::uint64 value) {
  _internal_set_command_capacity(value);
  // @@protoc_insertion_point(field_set:Tactile.ProtoBuf.Settings.command_capacity)
}

// optional bool show_grid = 18;
inline bool Settings::_internal_has_show_grid() const {
  bool value = (_has_bits_[0] & 0x00000020u) != 0;
  return value;
}
inline bool Settings::has_show_grid() const {
  return _internal_has_show_grid();
}
inline void Settings::clear_show_grid() {
  show_grid_ = false;
  _has_bits_[0] &= ~0x00000020u;
}
inline bool Settings::_internal_show_grid() const {
  return show_grid_;
}
inline bool Settings::show_grid() const {
  // @@protoc_insertion_point(field_get:Tactile.ProtoBuf.Settings.show_grid)
  return _internal_show_grid();
}
inline void Settings::_internal_set_show_grid(bool value) {
  _has_bits_[0] |= 0x00000020u;
  show_grid_ = value;
}
inline void Settings::set_show_grid(bool value) {
  _internal_set_show_grid(value);
  // @@protoc_insertion_point(field_set:Tactile.ProtoBuf.Settings.show_grid)
}

// optional bool window_border = 19;
inline bool Settings::_internal_has_window_border() const {
  bool value = (_has_bits_[0] & 0x00000040u) != 0;
  return value;
}
inline bool Settings::has_window_border() const {
  return _internal_has_window_border();
}
inline void Settings::clear_window_border() {
  window_border_ = false;
  _has_bits_[0] &= ~0x00000040u;
}
inline bool Settings::_internal_window_border() const {
  return window_border_;
}
inline bool Settings::window_border() const {
  // @@protoc_insertion_point(field_get:Tactile.ProtoBuf.Settings.window_border)
  return _internal_window_border();
}
inline void Settings::_internal_set_window_border(bool value) {
  _has_bits_[0] |= 0x00000040u;
  window_border_ = value;
}
inline void Settings::set_window_border(bool value) {
  _internal_set_window_border(value);
  // @@protoc_insertion_point(field_set:Tactile.ProtoBuf.Settings.window_border)
}

// optional bool restore_last_session = 20;
inline bool Settings::_internal_has_restore_last_session() const {
  bool value = (_has_bits_[0] & 0x00000080u) != 0;
  return value;
}
inline bool Settings::has_restore_last_session() const {
  return _internal_has_restore_last_session();
}
inline void Settings::clear_restore_last_session() {
  restore_last_session_ = false;
  _has_bits_[0] &= ~0x00000080u;
}
inline bool Settings::_internal_restore_last_session() const {
  return restore_last_session_;
}
inline bool Settings::restore_last_session() const {
  // @@protoc_insertion_point(field_get:Tactile.ProtoBuf.Settings.restore_last_session)
  return _internal_restore_last_session();
}
inline void Settings::_internal_set_restore_last_session(bool value) {
  _has_bits_[0] |= 0x00000080u;
  restore_last_session_ = value;
}
inline void Settings::set_restore_last_session(bool value) {
  _internal_set_restore_last_session(value);
  // @@protoc_insertion_point(field_set:Tactile.ProtoBuf.Settings.restore_last_session)
}

// optional bool restore_layout = 21;
inline bool Settings::_internal_has_restore_layout() const {
  bool value = (_has_bits_[0] & 0x00000100u) != 0;
  return value;
}
inline bool Settings::has_restore_layout() const {
  return _internal_has_restore_layout();
}
inline void Settings::clear_restore_layout() {
  restore_layout_ = false;
  _has_bits_[0] &= ~0x00000100u;
}
inline bool Settings::_internal_restore_layout() const {
  return restore_layout_;
}
inline bool Settings::restore_layout() const {
  // @@protoc_insertion_point(field_get:Tactile.ProtoBuf.Settings.restore_layout)
  return _internal_restore_layout();
}
inline void Settings::_internal_set_restore_layout(bool value) {
  _has_bits_[0] |= 0x00000100u;
  restore_layout_ = value;
}
inline void Settings::set_restore_layout(bool value) {
  _internal_set_restore_layout(value);
  // @@protoc_insertion_point(field_set:Tactile.ProtoBuf.Settings.restore_layout)
}

// optional bool show_layer_dock = 22;
inline bool Settings::_internal_has_show_layer_dock() const {
  bool value = (_has_bits_[0] & 0x00000200u) != 0;
  return value;
}
inline bool Settings::has_show_layer_dock() const {
  return _internal_has_show_layer_dock();
}
inline void Settings::clear_show_layer_dock() {
  show_layer_dock_ = false;
  _has_bits_[0] &= ~0x00000200u;
}
inline bool Settings::_internal_show_layer_dock() const {
  return show_layer_dock_;
}
inline bool Settings::show_layer_dock() const {
  // @@protoc_insertion_point(field_get:Tactile.ProtoBuf.Settings.show_layer_dock)
  return _internal_show_layer_dock();
}
inline void Settings::_internal_set_show_layer_dock(bool value) {
  _has_bits_[0] |= 0x00000200u;
  show_layer_dock_ = value;
}
inline void Settings::set_show_layer_dock(bool value) {
  _internal_set_show_layer_dock(value);
  // @@protoc_insertion_point(field_set:Tactile.ProtoBuf.Settings.show_layer_dock)
}

// optional bool show_properties_dock = 23;
inline bool Settings::_internal_has_show_properties_dock() const {
  bool value = (_has_bits_[0] & 0x00000400u) != 0;
  return value;
}
inline bool Settings::has_show_properties_dock() const {
  return _internal_has_show_properties_dock();
}
inline void Settings::clear_show_properties_dock() {
  show_properties_dock_ = false;
  _has_bits_[0] &= ~0x00000400u;
}
inline bool Settings::_internal_show_properties_dock() const {
  return show_properties_dock_;
}
inline bool Settings::show_properties_dock() const {
  // @@protoc_insertion_point(field_get:Tactile.ProtoBuf.Settings.show_properties_dock)
  return _internal_show_properties_dock();
}
inline void Settings::_internal_set_show_properties_dock(bool value) {
  _has_bits_[0] |= 0x00000400u;
  show_properties_dock_ = value;
}
inline void Settings::set_show_properties_dock(bool value) {
  _internal_set_show_properties_dock(value);
  // @@protoc_insertion_point(field_set:Tactile.ProtoBuf.Settings.show_properties_dock)
}

// optional bool show_tileset_dock = 24;
inline bool Settings::_internal_has_show_tileset_dock() const {
  bool value = (_has_bits_[0] & 0x00000800u) != 0;
  return value;
}
inline bool Settings::has_show_tileset_dock() const {
  return _internal_has_show_tileset_dock();
}
inline void Settings::clear_show_tileset_dock() {
  show_tileset_dock_ = false;
  _has_bits_[0] &= ~0x00000800u;
}
inline bool Settings::_internal_show_tileset_dock() const {
  return show_tileset_dock_;
}
inline bool Settings::show_tileset_dock() const {
  // @@protoc_insertion_point(field_get:Tactile.ProtoBuf.Settings.show_tileset_dock)
  return _internal_show_tileset_dock();
}
inline void Settings::_internal_set_show_tileset_dock(bool value) {
  _has_bits_[0] |= 0x00000800u;
  show_tileset_dock_ = value;
}
inline void Settings::set_show_tileset_dock(bool value) {
  _internal_set_show_tileset_dock(value);
  // @@protoc_insertion_point(field_set:Tactile.ProtoBuf.Settings.show_tileset_dock)
}

// optional bool embed_tilesets = 25;
inline bool Settings::_internal_has_embed_tilesets() const {
  bool value = (_has_bits_[0] & 0x00001000u) != 0;
  return value;
}
inline bool Settings::has_embed_tilesets() const {
  return _internal_has_embed_tilesets();
}
inline void Settings::clear_embed_tilesets() {
  embed_tilesets_ = false;
  _has_bits_[0] &= ~0x00001000u;
}
inline bool Settings::_internal_embed_tilesets() const {
  return embed_tilesets_;
}
inline bool Settings::embed_tilesets() const {
  // @@protoc_insertion_point(field_get:Tactile.ProtoBuf.Settings.embed_tilesets)
  return _internal_embed_tilesets();
}
inline void Settings::_internal_set_embed_tilesets(bool value) {
  _has_bits_[0] |= 0x00001000u;
  embed_tilesets_ = value;
}
inline void Settings::set_embed_tilesets(bool value) {
  _internal_set_embed_tilesets(value);
  // @@protoc_insertion_point(field_set:Tactile.ProtoBuf.Settings.embed_tilesets)
}

// optional bool human_readable_output = 26;
inline bool Settings::_internal_has_human_readable_output() const {
  bool value = (_has_bits_[0] & 0x00002000u) != 0;
  return value;
}
inline bool Settings::has_human_readable_output() const {
  return _internal_has_human_readable_output();
}
inline void Settings::clear_human_readable_output() {
  human_readable_output_ = false;
  _has_bits_[0] &= ~0x00002000u;
}
inline bool Settings::_internal_human_readable_output() const {
  return human_readable_output_;
}
inline bool Settings::human_readable_output() const {
  // @@protoc_insertion_point(field_get:Tactile.ProtoBuf.Settings.human_readable_output)
  return _internal_human_readable_output();
}
inline void Settings::_internal_set_human_readable_output(bool value) {
  _has_bits_[0] |= 0x00002000u;
  human_readable_output_ = value;
}
inline void Settings::set_human_readable_output(bool value) {
  _internal_set_human_readable_output(value);
  // @@protoc_insertion_point(field_set:Tactile.ProtoBuf.Settings.human_readable_output)
}

// optional .Tactile.ProtoBuf.OverlayPos viewport_overlay_pos = 27;
inline bool Settings::_internal_has_viewport_overlay_pos() const {
  bool value = (_has_bits_[0] & 0x00004000u) != 0;
  return value;
}
inline bool Settings::has_viewport_overlay_pos() const {
  return _internal_has_viewport_overlay_pos();
}
inline void Settings::clear_viewport_overlay_pos() {
  viewport_overlay_pos_ = 0;
  _has_bits_[0] &= ~0x00004000u;
}
inline ::Tactile::ProtoBuf::OverlayPos Settings::_internal_viewport_overlay_pos() const {
  return static_cast< ::Tactile::ProtoBuf::OverlayPos >(viewport_overlay_pos_);
}
inline ::Tactile::ProtoBuf::OverlayPos Settings::viewport_overlay_pos() const {
  // @@protoc_insertion_point(field_get:Tactile.ProtoBuf.Settings.viewport_overlay_pos)
  return _internal_viewport_overlay_pos();
}
inline void Settings::_internal_set_viewport_overlay_pos(::Tactile::ProtoBuf::OverlayPos value) {
  _has_bits_[0] |= 0x00004000u;
  viewport_overlay_pos_ = value;
}
inline void Settings::set_viewport_overlay_pos(::Tactile::ProtoBuf::OverlayPos value) {
  _internal_set_viewport_overlay_pos(value);
  // @@protoc_insertion_point(field_set:Tactile.ProtoBuf.Settings.viewport_overlay_pos)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace ProtoBuf
}  // namespace Tactile

PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::Tactile::ProtoBuf::OverlayPos> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Tactile::ProtoBuf::OverlayPos>() {
  return ::Tactile::ProtoBuf::OverlayPos_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_settings_2eproto
