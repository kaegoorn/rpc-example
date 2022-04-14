#pragma once
#include "abstractcommand.h"
#include <map>
#include <string_view>

class CommandFactory {
public:
  template <class RequestHandler> Error registerRequestHandler(const CommandId &id) {
    commands_[id] = [](SerialNumber serialNumber) -> AbstractRequestHandler * {
      return new RequestHandler(serialNumber);
    };
    return Success;
  }

  Error createRequestHandler(const CommandId &id, SerialNumber serialNumber, std::shared_ptr<AbstractRequestHandler> &command) const {
    CommandMap::const_iterator i = commands_.find(id);
    if(i == commands_.end()) {
      command.reset();
      return Fail;
    }
    command.reset(i->second(serialNumber));
    return Success;
  }

private:
  using CommandMap = std::map<CommandId, std::function<AbstractRequestHandler *(SerialNumber serialNumber)>>;
  CommandMap commands_;
};
