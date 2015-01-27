/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */

// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: network.proto

#ifndef PROTOBUF_network_2eproto__INCLUDED
#define PROTOBUF_network_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2004000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2004001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>
// @@protoc_insertion_point(includes)

// Internal implementation detail -- do not call these.
void protobuf_AddDesc_network_2eproto();
void protobuf_AssignDesc_network_2eproto();
void protobuf_ShutdownFile_network_2eproto();

class Marker;
class DataSet;

enum Marker_MODE
{
    Marker_MODE_ENABLE = 0,
    Marker_MODE_DISABLE = 1
};
bool Marker_MODE_IsValid(int value);
const Marker_MODE Marker_MODE_MODE_MIN = Marker_MODE_ENABLE;
const Marker_MODE Marker_MODE_MODE_MAX = Marker_MODE_DISABLE;
const int Marker_MODE_MODE_ARRAYSIZE = Marker_MODE_MODE_MAX + 1;

const ::google::protobuf::EnumDescriptor *Marker_MODE_descriptor();
inline const ::std::string &Marker_MODE_Name(Marker_MODE value)
{
    return ::google::protobuf::internal::NameOfEnum(
        Marker_MODE_descriptor(), value);
}
inline bool Marker_MODE_Parse(
    const ::std::string &name, Marker_MODE *value)
{
    return ::google::protobuf::internal::ParseNamedEnum<Marker_MODE>(
        Marker_MODE_descriptor(), name, value);
}
enum DataSet_DATASET
{
    DataSet_DATASET_PRESSURE = 0,
    DataSet_DATASET_VELOCITY = 1
};
bool DataSet_DATASET_IsValid(int value);
const DataSet_DATASET DataSet_DATASET_DATASET_MIN = DataSet_DATASET_PRESSURE;
const DataSet_DATASET DataSet_DATASET_DATASET_MAX = DataSet_DATASET_VELOCITY;
const int DataSet_DATASET_DATASET_ARRAYSIZE = DataSet_DATASET_DATASET_MAX + 1;

const ::google::protobuf::EnumDescriptor *DataSet_DATASET_descriptor();
inline const ::std::string &DataSet_DATASET_Name(DataSet_DATASET value)
{
    return ::google::protobuf::internal::NameOfEnum(
        DataSet_DATASET_descriptor(), value);
}
inline bool DataSet_DATASET_Parse(
    const ::std::string &name, DataSet_DATASET *value)
{
    return ::google::protobuf::internal::ParseNamedEnum<DataSet_DATASET>(
        DataSet_DATASET_descriptor(), name, value);
}
// ===================================================================

class Marker : public ::google::protobuf::Message
{
public:
    Marker();
    virtual ~Marker();

    Marker(const Marker &from);

    inline Marker &operator=(const Marker &from)
    {
        CopyFrom(from);
        return *this;
    }

    inline const ::google::protobuf::UnknownFieldSet &unknown_fields() const
    {
        return _unknown_fields_;
    }

    inline ::google::protobuf::UnknownFieldSet *mutable_unknown_fields()
    {
        return &_unknown_fields_;
    }

    static const ::google::protobuf::Descriptor *descriptor();
    static const Marker &default_instance();

    void Swap(Marker *other);

    // implements Message ----------------------------------------------

    Marker *New() const;
    void CopyFrom(const ::google::protobuf::Message &from);
    void MergeFrom(const ::google::protobuf::Message &from);
    void CopyFrom(const Marker &from);
    void MergeFrom(const Marker &from);
    void Clear();
    bool IsInitialized() const;

    int ByteSize() const;
    bool MergePartialFromCodedStream(
        ::google::protobuf::io::CodedInputStream *input);
    void SerializeWithCachedSizes(
        ::google::protobuf::io::CodedOutputStream *output) const;
    ::google::protobuf::uint8 *SerializeWithCachedSizesToArray(::google::protobuf::uint8 *output) const;
    int GetCachedSize() const
    {
        return _cached_size_;
    }

private:
    void SharedCtor();
    void SharedDtor();
    void SetCachedSize(int size) const;

public:
    ::google::protobuf::Metadata GetMetadata() const;

    // nested types ----------------------------------------------------

    typedef Marker_MODE MODE;
    static const MODE ENABLE = Marker_MODE_ENABLE;
    static const MODE DISABLE = Marker_MODE_DISABLE;
    static inline bool MODE_IsValid(int value)
    {
        return Marker_MODE_IsValid(value);
    }
    static const MODE MODE_MIN = Marker_MODE_MODE_MIN;
    static const MODE MODE_MAX = Marker_MODE_MODE_MAX;
    static const int MODE_ARRAYSIZE = Marker_MODE_MODE_ARRAYSIZE;
    static inline const ::google::protobuf::EnumDescriptor *
    MODE_descriptor()
    {
        return Marker_MODE_descriptor();
    }
    static inline const ::std::string &MODE_Name(MODE value)
    {
        return Marker_MODE_Name(value);
    }
    static inline bool MODE_Parse(const ::std::string &name,
                                  MODE *value)
    {
        return Marker_MODE_Parse(name, value);
    }

    // accessors -------------------------------------------------------

    // required int32 id = 1;
    inline bool has_id() const;
    inline void clear_id();
    static const int kIdFieldNumber = 1;
    inline ::google::protobuf::int32 id() const;
    inline void set_id(::google::protobuf::int32 value);

    // required .Marker.MODE mode = 2;
    inline bool has_mode() const;
    inline void clear_mode();
    static const int kModeFieldNumber = 2;
    inline ::Marker_MODE mode() const;
    inline void set_mode(::Marker_MODE value);

    // repeated float matrix = 3;
    inline int matrix_size() const;
    inline void clear_matrix();
    static const int kMatrixFieldNumber = 3;
    inline float matrix(int index) const;
    inline void set_matrix(int index, float value);
    inline void add_matrix(float value);
    inline const ::google::protobuf::RepeatedField<float> &
    matrix() const;
    inline ::google::protobuf::RepeatedField<float> *
    mutable_matrix();

    // @@protoc_insertion_point(class_scope:Marker)
private:
    inline void set_has_id();
    inline void clear_has_id();
    inline void set_has_mode();
    inline void clear_has_mode();

    ::google::protobuf::UnknownFieldSet _unknown_fields_;

    ::google::protobuf::int32 id_;
    int mode_;
    ::google::protobuf::RepeatedField<float> matrix_;

    mutable int _cached_size_;
    ::google::protobuf::uint32 _has_bits_[(3 + 31) / 32];

    friend void protobuf_AddDesc_network_2eproto();
    friend void protobuf_AssignDesc_network_2eproto();
    friend void protobuf_ShutdownFile_network_2eproto();

    void InitAsDefaultInstance();
    static Marker *default_instance_;
};
// -------------------------------------------------------------------

class DataSet : public ::google::protobuf::Message
{
public:
    DataSet();
    virtual ~DataSet();

    DataSet(const DataSet &from);

    inline DataSet &operator=(const DataSet &from)
    {
        CopyFrom(from);
        return *this;
    }

    inline const ::google::protobuf::UnknownFieldSet &unknown_fields() const
    {
        return _unknown_fields_;
    }

    inline ::google::protobuf::UnknownFieldSet *mutable_unknown_fields()
    {
        return &_unknown_fields_;
    }

    static const ::google::protobuf::Descriptor *descriptor();
    static const DataSet &default_instance();

    void Swap(DataSet *other);

    // implements Message ----------------------------------------------

    DataSet *New() const;
    void CopyFrom(const ::google::protobuf::Message &from);
    void MergeFrom(const ::google::protobuf::Message &from);
    void CopyFrom(const DataSet &from);
    void MergeFrom(const DataSet &from);
    void Clear();
    bool IsInitialized() const;

    int ByteSize() const;
    bool MergePartialFromCodedStream(
        ::google::protobuf::io::CodedInputStream *input);
    void SerializeWithCachedSizes(
        ::google::protobuf::io::CodedOutputStream *output) const;
    ::google::protobuf::uint8 *SerializeWithCachedSizesToArray(::google::protobuf::uint8 *output) const;
    int GetCachedSize() const
    {
        return _cached_size_;
    }

private:
    void SharedCtor();
    void SharedDtor();
    void SetCachedSize(int size) const;

public:
    ::google::protobuf::Metadata GetMetadata() const;

    // nested types ----------------------------------------------------

    typedef DataSet_DATASET DATASET;
    static const DATASET PRESSURE = DataSet_DATASET_PRESSURE;
    static const DATASET VELOCITY = DataSet_DATASET_VELOCITY;
    static inline bool DATASET_IsValid(int value)
    {
        return DataSet_DATASET_IsValid(value);
    }
    static const DATASET DATASET_MIN = DataSet_DATASET_DATASET_MIN;
    static const DATASET DATASET_MAX = DataSet_DATASET_DATASET_MAX;
    static const int DATASET_ARRAYSIZE = DataSet_DATASET_DATASET_ARRAYSIZE;
    static inline const ::google::protobuf::EnumDescriptor *
    DATASET_descriptor()
    {
        return DataSet_DATASET_descriptor();
    }
    static inline const ::std::string &DATASET_Name(DATASET value)
    {
        return DataSet_DATASET_Name(value);
    }
    static inline bool DATASET_Parse(const ::std::string &name,
                                     DATASET *value)
    {
        return DataSet_DATASET_Parse(name, value);
    }

    // accessors -------------------------------------------------------

    // required int32 id = 1;
    inline bool has_id() const;
    inline void clear_id();
    static const int kIdFieldNumber = 1;
    inline ::google::protobuf::int32 id() const;
    inline void set_id(::google::protobuf::int32 value);

    // required .DataSet.DATASET dataSet = 2;
    inline bool has_dataset() const;
    inline void clear_dataset();
    static const int kDataSetFieldNumber = 2;
    inline ::DataSet_DATASET dataset() const;
    inline void set_dataset(::DataSet_DATASET value);

    // @@protoc_insertion_point(class_scope:DataSet)
private:
    inline void set_has_id();
    inline void clear_has_id();
    inline void set_has_dataset();
    inline void clear_has_dataset();

    ::google::protobuf::UnknownFieldSet _unknown_fields_;

    ::google::protobuf::int32 id_;
    int dataset_;

    mutable int _cached_size_;
    ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

    friend void protobuf_AddDesc_network_2eproto();
    friend void protobuf_AssignDesc_network_2eproto();
    friend void protobuf_ShutdownFile_network_2eproto();

    void InitAsDefaultInstance();
    static DataSet *default_instance_;
};
// ===================================================================

// ===================================================================

// Marker

// required int32 id = 1;
inline bool Marker::has_id() const
{
    return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Marker::set_has_id()
{
    _has_bits_[0] |= 0x00000001u;
}
inline void Marker::clear_has_id()
{
    _has_bits_[0] &= ~0x00000001u;
}
inline void Marker::clear_id()
{
    id_ = 0;
    clear_has_id();
}
inline ::google::protobuf::int32 Marker::id() const
{
    return id_;
}
inline void Marker::set_id(::google::protobuf::int32 value)
{
    set_has_id();
    id_ = value;
}

// required .Marker.MODE mode = 2;
inline bool Marker::has_mode() const
{
    return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Marker::set_has_mode()
{
    _has_bits_[0] |= 0x00000002u;
}
inline void Marker::clear_has_mode()
{
    _has_bits_[0] &= ~0x00000002u;
}
inline void Marker::clear_mode()
{
    mode_ = 0;
    clear_has_mode();
}
inline ::Marker_MODE Marker::mode() const
{
    return static_cast< ::Marker_MODE>(mode_);
}
inline void Marker::set_mode(::Marker_MODE value)
{
    GOOGLE_DCHECK(::Marker_MODE_IsValid(value));
    set_has_mode();
    mode_ = value;
}

// repeated float matrix = 3;
inline int Marker::matrix_size() const
{
    return matrix_.size();
}
inline void Marker::clear_matrix()
{
    matrix_.Clear();
}
inline float Marker::matrix(int index) const
{
    return matrix_.Get(index);
}
inline void Marker::set_matrix(int index, float value)
{
    matrix_.Set(index, value);
}
inline void Marker::add_matrix(float value)
{
    matrix_.Add(value);
}
inline const ::google::protobuf::RepeatedField<float> &
Marker::matrix() const
{
    return matrix_;
}
inline ::google::protobuf::RepeatedField<float> *
Marker::mutable_matrix()
{
    return &matrix_;
}

// -------------------------------------------------------------------

// DataSet

// required int32 id = 1;
inline bool DataSet::has_id() const
{
    return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void DataSet::set_has_id()
{
    _has_bits_[0] |= 0x00000001u;
}
inline void DataSet::clear_has_id()
{
    _has_bits_[0] &= ~0x00000001u;
}
inline void DataSet::clear_id()
{
    id_ = 0;
    clear_has_id();
}
inline ::google::protobuf::int32 DataSet::id() const
{
    return id_;
}
inline void DataSet::set_id(::google::protobuf::int32 value)
{
    set_has_id();
    id_ = value;
}

// required .DataSet.DATASET dataSet = 2;
inline bool DataSet::has_dataset() const
{
    return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void DataSet::set_has_dataset()
{
    _has_bits_[0] |= 0x00000002u;
}
inline void DataSet::clear_has_dataset()
{
    _has_bits_[0] &= ~0x00000002u;
}
inline void DataSet::clear_dataset()
{
    dataset_ = 0;
    clear_has_dataset();
}
inline ::DataSet_DATASET DataSet::dataset() const
{
    return static_cast< ::DataSet_DATASET>(dataset_);
}
inline void DataSet::set_dataset(::DataSet_DATASET value)
{
    GOOGLE_DCHECK(::DataSet_DATASET_IsValid(value));
    set_has_dataset();
    dataset_ = value;
}

// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google
{
namespace protobuf
{

    template <>
    inline const EnumDescriptor *GetEnumDescriptor< ::Marker_MODE>()
    {
        return ::Marker_MODE_descriptor();
    }
    template <>
    inline const EnumDescriptor *GetEnumDescriptor< ::DataSet_DATASET>()
    {
        return ::DataSet_DATASET_descriptor();
    }

} // namespace google
} // namespace protobuf
#endif // SWIG

// @@protoc_insertion_point(global_scope)

#endif // PROTOBUF_network_2eproto__INCLUDED
