////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 ArangoDB GmbH, Cologne, Germany
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
/// @author Dr. Frank Celler
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_REST_HANDLER_REST_DOCUMENT_HANDLER_H
#define ARANGOD_REST_HANDLER_REST_DOCUMENT_HANDLER_H 1

#include "Basics/Common.h"
#include "Transaction/TransactionRegistry.h"
#include "RestHandler/RestVocbaseBaseHandler.h"

namespace arangodb {

struct TransactionProperties {

  transaction::TransactionId transactionId;
  bool isSingle;
  bool isStart;

  /// @brief default ctor 
  TransactionProperties(
    transaction::TransactionId const& id = transaction::TransactionId::zero(),
    bool single = false, bool start = false)
    : transactionId(id), isSingle(single), isStart(start) {}

  /// @brief construct with http headers
  TransactionProperties(GeneralRequest const* request)
    : isSingle(false), isStart(false){
    
    auto const headers = request->headers();
    auto const coordinator = headers.find("trxcoordinator");
    if (coordinator!=headers.end()) { // We have a transaction header
      auto const identifier = headers.find("trxidentifier");
      auto const issingle = headers.find("issingle");
      auto const isstart = headers.find("issstart");
      if (identifier!=headers.end()) {
        try {
          transactionId = transaction::TransactionId(
            std::stoull(coordinator->second),std::stoull(identifier->second));
        } catch (std::exception const& e) {
          LOG_TOPIC(ERR, arangodb::Logger::TRANSACTIONS) <<
            "Failed to extract transaction headers ";
        }
        if (issingle!=headers.end()) {
          try {
            isSingle = (std::stoi(issingle->second) == 1);
          } catch (...) {}
        }
        if (isstart!=headers.end()) {
          try {
            isStart = (std::stoi(isstart->second) == 1);
          } catch (...) {}
        }
      } else {
        LOG_TOPIC(ERR, Logger::TRANSACTIONS) <<
          "Incomplete transaction headers: " << *this;
      }
    }
  }
  
};

inline std::ostream& operator<<(
  std::ostream& o, TransactionProperties const& p) {
  o << "[" << p.transactionId;
  if (p.isStart)  { o << ", start";  }
  if (p.isSingle) { o << ", single"; }
  o << "]";
  return o;
}

class RestDocumentHandler : public RestVocbaseBaseHandler {
 public:
  RestDocumentHandler(GeneralRequest*, GeneralResponse*);

 public:
  RestStatus execute() override final;
  char const* name() const override final { return "RestDocumentHandler"; }

#ifdef USE_ENTERPRISE
  void finalizeExecute() override;
#endif

 protected:
  virtual TRI_col_type_e getCollectionType() const {
    return TRI_COL_TYPE_DOCUMENT;
  }

  // creates a document
  virtual bool createDocument();

  // reads a single or all documents
  bool readDocument();

  // reads a single document
  bool readSingleDocument(bool generateBody);

  // reads multiple documents
  bool readManyDocuments();

  // reads a single document head
  bool checkDocument();

  // replaces a document
  bool replaceDocument();

  // updates a document
  bool updateDocument();

  // helper function for replace and update
  bool modifyDocument(bool);

  // deletes a document
  bool deleteDocument();

  // transaction stuff
  TransactionProperties _transProps;
  
};
}

#endif
