#pragma once

#include "protocol.h"
#include "types.h"

// for each client connection
class AbstractTransport {
public:
  virtual ~AbstractTransport() = default;
  virtual Error onRead(ByteArrayView data) = 0;
  virtual Error write(ByteArrayView data) = 0;
};

class Transport : public AbstractTransport, public std::enable_shared_from_this<Transport> {
public:
  ~Transport() override {}
  Error initialize(std::shared_ptr<CommandFactory> factory, std::shared_ptr<CommandProcessor> processor) {
    protocol_.reset(new Protocol(factory));
    protocol_->setProcessor(processor);
    protocol_->setTransport(shared_from_this());
    return Success;
  }

  void destroy() {
    protocol_->setTransport({});
  }

  Error onRead(ByteArrayView data) override {
    return protocol_->deserializeRequest(data);
  }

  Error write(ByteArrayView data) override {
    // ...
    return Success;
  }

private:
  std::shared_ptr<Protocol> protocol_;
};
