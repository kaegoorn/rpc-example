#pragma once

#include "commandfactory.h"
#include "commandprocessor.h"
#include <endian.h>

class AbstractTransport;

class AbstractProtocol {
public:
  virtual ~AbstractProtocol() = default;
  // ...
};

class Protocol : public AbstractProtocol, public std::enable_shared_from_this<Protocol> {
public:
  Protocol(std::shared_ptr<CommandFactory> factory) : factory_(factory) {}

  // nothing interesting )
  Error deserializeRequest(ByteArrayView data) {
    static constexpr int HeaderSize = sizeof(CommandId) + sizeof(SerialNumber);

    if(data.size() < HeaderSize) {
      return Fail;
    }

    // simple deserialization
    const unsigned char *p = data.data();
    CommandId commandId = be32toh(*reinterpret_cast<const uint32_t *>(p));
    p += sizeof(CommandId);
    SerialNumber serialNumber = be32toh(*reinterpret_cast<const uint32_t *>(p));
    p += sizeof(SerialNumber);

    std::shared_ptr<AbstractRequestHandler> commandHandler;
    Error error = factory_->createRequestHandler(commandId, serialNumber, commandHandler);
    if(error != Success) {
      return error;
    }

    data = ByteArrayView(p, data.size() - HeaderSize);
    error = commandHandler->deserialize(this, data);
    if(error != Success) {
      return error;
    }

    processor_->process(std::move(commandHandler), shared_from_this());
    return Success;
  }

  //  example serialize method
  Error serializeString(ByteArray &data, std::string_view s) {
    // ...
    return Success;
  }
  //  example deserialize method
  Error deserializeString(ByteArrayView &data, std::string &s) {
    // ...
    return Success;
  }

  // send methods
  void sendError(Error error) {
    // serialize and call transport
    // ...
  }
  void sendRequest(AbstractCommand &) {
    // ...
  }
  void sendReply(const AbstractCommand &) {
    // ...
  }

  //
  std::shared_ptr<AbstractTransport> transport() const {
    return transport_;
  }
  void setTransport(const std::shared_ptr<AbstractTransport> &transport) {
    transport_ = transport;
  }
  std::shared_ptr<CommandProcessor> processor() const {
    return processor_;
  }
  void setProcessor(const std::shared_ptr<CommandProcessor> &processor) {
    processor_ = processor;
  }

private:
  std::shared_ptr<CommandFactory> factory_;
  std::shared_ptr<AbstractTransport> transport_;
  std::shared_ptr<CommandProcessor> processor_;
};
