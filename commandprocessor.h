#pragma once
#include "commandfactory.h"
#include "protocol.h"
#include <future>
#include <mutex>
#include <queue>
#include <unordered_set>

class CommandProcessor {
public:
  CommandProcessor(size_t maxParallelCommands = 16, size_t maxWaitingCommands = 16) : maxParallelCommands_(maxParallelCommands), maxWaitingCommands_(maxWaitingCommands) {}

  void process(std::shared_ptr<AbstractRequestHandler> &&command, const std::shared_ptr<Protocol> &protocol) {
    // direct processing
    if(!(command->flags() & AbstractRequestHandler::Flag::Long)) {
      command->process();
      return;
    }
    std::lock_guard<std::mutex> lock(lock_);
    if(runningCommands_ == maxParallelCommands_) {
      if(waitingCommands_.size() == maxWaitingCommands_) {
        //        protocol_->sendError(Busy);
        return;
      }
      waitingCommands_.push(command);
      return;
    }
    async(std::move(command), protocol);
  }

  // Before completing work in asynchronous mode, you need to make sure that all tasks are completed, otherwise there will be a deadlock when sending a response
  bool isPending() const {
    return !pendings_.empty();
  }

protected:
  struct ThreadData {
    CommandProcessor *_this;
    pthread_t thread;
    std::shared_ptr<AbstractRequestHandler> command;
    std::shared_ptr<Protocol> protocol;
  };

  void processCommand(ThreadData *data) {

    data->command->process();

    // in asynchronous mode, the call must be made via invoke or the protocol must be thread-safe
    //          protocol_->sendReply(command->reply());

    std::lock_guard<std::mutex> lock(lock_);
    --runningCommands_;
    pendings_.erase(data);
    if(!waitingCommands_.empty()) {
      std::shared_ptr<AbstractRequestHandler> command = waitingCommands_.front();
      waitingCommands_.pop();
      async(std::move(command), data->protocol);
    }
  }

  static void *threadWrapper(void *context) {
    ThreadData *data = static_cast<ThreadData *>(context);
    data->_this->processCommand(data);
    delete data;
    return nullptr;
  }

  void async(std::shared_ptr<AbstractRequestHandler> &&command, const std::shared_ptr<Protocol> &protocol) {

    // !!!!!! should be replaced by ThreadPool !!!!!!!

    ThreadData *threadData = new ThreadData;
    threadData->protocol = protocol;
    threadData->command = std::move(command);
    threadData->_this = this;
    pendings_.emplace(threadData);
    if(pthread_create(&threadData->thread, NULL, threadWrapper, (void *)threadData) != 0) {
      // direct
      command->process();
      pendings_.erase(threadData);
      return;
    }
  }

private:
  std::mutex lock_;
  size_t maxParallelCommands_;
  size_t maxWaitingCommands_;
  size_t runningCommands_ = 0;
  // should be replaced by ThreadPool
  std::unordered_set<ThreadData *> pendings_;
  std::queue<std::shared_ptr<AbstractRequestHandler>> waitingCommands_;
};
