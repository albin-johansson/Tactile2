// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: history.proto

#include "history.pb.h"

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
constexpr History::History(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : files_()
  , last_opened_file_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string){}
struct HistoryDefaultTypeInternal {
  constexpr HistoryDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~HistoryDefaultTypeInternal() {}
  union {
    History _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT HistoryDefaultTypeInternal _History_default_instance_;
}  // namespace Proto
}  // namespace Tactile
static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_history_2eproto[1];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_history_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_history_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_history_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  PROTOBUF_FIELD_OFFSET(::Tactile::Proto::History, _has_bits_),
  PROTOBUF_FIELD_OFFSET(::Tactile::Proto::History, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::Tactile::Proto::History, last_opened_file_),
  PROTOBUF_FIELD_OFFSET(::Tactile::Proto::History, files_),
  0,
  ~0u,
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, 8, -1, sizeof(::Tactile::Proto::History)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::Tactile::Proto::_History_default_instance_),
};

const char descriptor_table_protodef_history_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\rhistory.proto\022\rTactile.Proto\"L\n\007Histor"
  "y\022\035\n\020last_opened_file\030\001 \001(\tH\000\210\001\001\022\r\n\005file"
  "s\030\002 \003(\tB\023\n\021_last_opened_fileb\006proto3"
  ;
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_history_2eproto_once;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_history_2eproto = {
  false, false, 116, descriptor_table_protodef_history_2eproto, "history.proto", 
  &descriptor_table_history_2eproto_once, nullptr, 0, 1,
  schemas, file_default_instances, TableStruct_history_2eproto::offsets,
  file_level_metadata_history_2eproto, file_level_enum_descriptors_history_2eproto, file_level_service_descriptors_history_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable* descriptor_table_history_2eproto_getter() {
  return &descriptor_table_history_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY static ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptorsRunner dynamic_init_dummy_history_2eproto(&descriptor_table_history_2eproto);
namespace Tactile {
namespace Proto {

// ===================================================================

class History::_Internal {
 public:
  using HasBits = decltype(std::declval<History>()._has_bits_);
  static void set_has_last_opened_file(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
};

History::History(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned),
  files_(arena) {
  SharedCtor();
  if (!is_message_owned) {
    RegisterArenaDtor(arena);
  }
  // @@protoc_insertion_point(arena_constructor:Tactile.Proto.History)
}
History::History(const History& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _has_bits_(from._has_bits_),
      files_(from.files_) {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  last_opened_file_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_last_opened_file()) {
    last_opened_file_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_last_opened_file(), 
      GetArenaForAllocation());
  }
  // @@protoc_insertion_point(copy_constructor:Tactile.Proto.History)
}

void History::SharedCtor() {
last_opened_file_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

History::~History() {
  // @@protoc_insertion_point(destructor:Tactile.Proto.History)
  if (GetArenaForAllocation() != nullptr) return;
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

inline void History::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  last_opened_file_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void History::ArenaDtor(void* object) {
  History* _this = reinterpret_cast< History* >(object);
  (void)_this;
}
void History::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void History::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void History::Clear() {
// @@protoc_insertion_point(message_clear_start:Tactile.Proto.History)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  files_.Clear();
  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    last_opened_file_.ClearNonDefaultToEmpty();
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* History::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // optional string last_opened_file = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          auto str = _internal_mutable_last_opened_file();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "Tactile.Proto.History.last_opened_file"));
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // repeated string files = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          ptr -= 1;
          do {
            ptr += 1;
            auto str = _internal_add_files();
            ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
            CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "Tactile.Proto.History.files"));
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<18>(ptr));
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

::PROTOBUF_NAMESPACE_ID::uint8* History::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:Tactile.Proto.History)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // optional string last_opened_file = 1;
  if (_internal_has_last_opened_file()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_last_opened_file().data(), static_cast<int>(this->_internal_last_opened_file().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "Tactile.Proto.History.last_opened_file");
    target = stream->WriteStringMaybeAliased(
        1, this->_internal_last_opened_file(), target);
  }

  // repeated string files = 2;
  for (int i = 0, n = this->_internal_files_size(); i < n; i++) {
    const auto& s = this->_internal_files(i);
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      s.data(), static_cast<int>(s.length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "Tactile.Proto.History.files");
    target = stream->WriteString(2, s, target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Tactile.Proto.History)
  return target;
}

size_t History::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:Tactile.Proto.History)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // repeated string files = 2;
  total_size += 1 *
      ::PROTOBUF_NAMESPACE_ID::internal::FromIntSize(files_.size());
  for (int i = 0, n = files_.size(); i < n; i++) {
    total_size += ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
      files_.Get(i));
  }

  // optional string last_opened_file = 1;
  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_last_opened_file());
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData History::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSizeCheck,
    History::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*History::GetClassData() const { return &_class_data_; }

void History::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message* to,
                      const ::PROTOBUF_NAMESPACE_ID::Message& from) {
  static_cast<History *>(to)->MergeFrom(
      static_cast<const History &>(from));
}


void History::MergeFrom(const History& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:Tactile.Proto.History)
  GOOGLE_DCHECK_NE(&from, this);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  files_.MergeFrom(from.files_);
  if (from._internal_has_last_opened_file()) {
    _internal_set_last_opened_file(from._internal_last_opened_file());
  }
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void History::CopyFrom(const History& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Tactile.Proto.History)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool History::IsInitialized() const {
  return true;
}

void History::InternalSwap(History* other) {
  using std::swap;
  auto* lhs_arena = GetArenaForAllocation();
  auto* rhs_arena = other->GetArenaForAllocation();
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  files_.InternalSwap(&other->files_);
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      &last_opened_file_, lhs_arena,
      &other->last_opened_file_, rhs_arena
  );
}

::PROTOBUF_NAMESPACE_ID::Metadata History::GetMetadata() const {
  return ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(
      &descriptor_table_history_2eproto_getter, &descriptor_table_history_2eproto_once,
      file_level_metadata_history_2eproto[0]);
}

// @@protoc_insertion_point(namespace_scope)
}  // namespace Proto
}  // namespace Tactile
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::Tactile::Proto::History* Arena::CreateMaybeMessage< ::Tactile::Proto::History >(Arena* arena) {
  return Arena::CreateMessageInternal< ::Tactile::Proto::History >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
