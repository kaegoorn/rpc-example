#pragma once

#include "types.h"
#include <functional>
#include <memory>

/*
 * AbstractCommand and RequestHandler, which is inherited from AbstractCommand, in order to be able to implement the server part and the client library.
 */
class Protocol;
class AbstractCommand {
public:
  virtual ~AbstractCommand() = default;

  virtual Error serialize(Protocol *, ByteArray &) = 0;
};

// simplified, there should be a normal classes here
using AbstractRequest = AbstractCommand;
using AbstractReply = AbstractCommand;

class AbstractRequestHandler {
public:
  enum Flag { Long = 1 };

  AbstractRequestHandler(uint32_t flags, SerialNumber serialNumber) : serialNumber_(serialNumber) {
    flags_ = flags;
  }

  virtual ~AbstractRequestHandler() = default;

  virtual Error deserialize(Protocol *, ByteArrayView) = 0;
  virtual void process() = 0;

  uint32_t flags() const {
    return flags_;
  }

  // to identify a response by a client over an asynchronous protocol, not needed in a synchronous protocol. Can be replaced with any data
  SerialNumber serialNumber() const {
    return serialNumber_;
  }

  std::shared_ptr<AbstractReply> reply() const {
    return reply_;
  }

protected:
  void setReply(std::shared_ptr<AbstractReply> reply) {
    reply_ = reply;
  }

protected:
  uint32_t flags_ = 0;

  SerialNumber serialNumber_;
  std::shared_ptr<AbstractReply> reply_;
};

// Handler
template <class AbstractCommand> class RequestHandler : public AbstractRequestHandler, public AbstractCommand {
public:
  using AbstractRequestHandler::AbstractRequestHandler;
};

// simplified, there should be a normal classes here
template <class AbstractCommand> class AbstractReplyHandler : public RequestHandler<AbstractCommand> {};
