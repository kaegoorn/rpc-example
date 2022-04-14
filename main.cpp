#include "abstracttransport.h"

#include "protocol.h"

#include "commandprocessor.h"
#include <iostream>

using namespace std;

class HelloCommand : public AbstractCommand {
public:
  const std::string &hello() const {
    return hello_;
  }
  void setHello(std::string_view hello) {
    hello_ = hello;
  }

  Error serialize(Protocol *protocol, ByteArray &data) override {
    return protocol->serializeString(data, hello_);
  }

protected:
  std::string hello_;
};

class HelloReply : public AbstractReply {
public:
  const std::string &bye() const {
    return bye_;
  }

  void setBye(const std::string_view &bye) {
    bye_ = bye;
  }

  Error serialize(Protocol *protocol, ByteArray &data) override {
    return protocol->serializeString(data, bye_);
  }

private:
  std::string bye_;
};

class HelloRequestHandler : public RequestHandler<HelloCommand> {
public:
  HelloRequestHandler(SerialNumber serialNumber) : RequestHandler<HelloCommand>(static_cast<uint32_t>(AbstractRequestHandler::Flag::Long), serialNumber) {}

  Error deserialize(Protocol *protocol, ByteArrayView data) override {
    return protocol->deserializeString(data, hello_);
  }
  void process() override {
    std::shared_ptr<HelloReply> reply;
    setReply(reply);
  }
};

int main() {

  enum Commands : CommandId { Hello = 1 };

  std::shared_ptr<CommandProcessor> processor(new CommandProcessor);
  std::shared_ptr<CommandFactory> factory(new CommandFactory);
  factory->registerRequestHandler<HelloRequestHandler>(Commands::Hello);

  std::shared_ptr<Transport> transport(new Transport);
  transport->initialize(factory, processor);

  ByteArrayView data(reinterpret_cast<const unsigned char *>("\00\00\00\01\00\00\00\01"), 8);
  transport->onRead(data);

  while(processor->isPending()) {
    std::this_thread::sleep_for(1ms);
  }

  transport->destroy();

  return 0;
}
