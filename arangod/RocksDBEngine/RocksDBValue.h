////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2017 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Jan Steemann
/// @author Daniel H. Larkin
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGO_ROCKSDB_ROCKSDB_VALUE_H
#define ARANGO_ROCKSDB_ROCKSDB_VALUE_H 1

#include "Basics/Common.h"
#include "RocksDBEngine/RocksDBTypes.h"
#include "VocBase/vocbase.h"

#include <rocksdb/slice.h>

#include <velocypack/Slice.h>
#include <velocypack/velocypack-aliases.h>

namespace arangodb {

class RocksDBValue {
 public:
  RocksDBValue() = delete;

  static RocksDBValue Database(VPackSlice const& data);
  static RocksDBValue Collection(VPackSlice const& data);
  static RocksDBValue Index(VPackSlice const& data);
  static RocksDBValue Document(VPackSlice const& data);
  static RocksDBValue PrimaryIndexValue(TRI_voc_rid_t revisionId);
  static RocksDBValue EdgeIndexValue();
  static RocksDBValue IndexValue();
  static RocksDBValue UniqueIndexValue(std::string const& primaryKey);
  static RocksDBValue View(VPackSlice const& data);

 public:
  static TRI_voc_rid_t revisionId(RocksDBValue const&);
  static TRI_voc_rid_t revisionId(rocksdb::Slice const&);
  static TRI_voc_rid_t revisionId(std::string const&);

  static std::string primaryKey(RocksDBValue const&);
  static std::string primaryKey(rocksdb::Slice const&);
  static std::string primaryKey(std::string const&);

  static VPackSlice data(RocksDBValue const&);
  static VPackSlice data(rocksdb::Slice const&);
  static VPackSlice data(std::string const&);

 public:
  std::string const& value() const;

 private:
  RocksDBValue(RocksDBEntryType type);
  RocksDBValue(RocksDBEntryType type, uint64_t data);
  RocksDBValue(RocksDBEntryType type, std::string const& data);
  RocksDBValue(RocksDBEntryType type, VPackSlice const& data);

 private:
  static RocksDBEntryType type(char const* data, size_t size);
  static TRI_voc_rid_t revisionId(char const* data, size_t size);
  static std::string primaryKey(char const* data, size_t size);
  static VPackSlice data(char const* data, size_t size);

 private:
  RocksDBEntryType const _type;
  std::string _buffer;
};

}  // namespace arangodb

#endif
