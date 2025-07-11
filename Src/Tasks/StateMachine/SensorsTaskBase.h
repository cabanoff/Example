 // SensorsTaskBase.h
#pragma once
#include "ScheduledTaskBase.h"
#include "IMediatorObserver.h"
//#include "DataHandler.h"
#include "RBLabSensParser.h" // to do: transfer it to the app layer
#include "Channels.h"
#include "debug_utils.h"


struct sensorsPollScript {
  bool singleCycleExecution ;
  uint32_t period_ms;
  uint32_t duration_ms;
  RBLabSensCommand::Type toRBLabSensParser;

  sensorsPollScript() :
    singleCycleExecution(true), 
    period_ms(1000),
    toRBLabSensParser(RBLabSensCommand::NONE)
    {}
};


class SensorsTaskBase : public ScheduledTaskBase, public IMediatorObserver {
public:
  SensorsTaskBase(sensorsPollScript& script);
  virtual ~SensorsTaskBase() {ccrAbort(m_ctx);}
    
  virtual void connectSender(ISender* sender);

  virtual void onConnect();
  virtual void onMediatorEvent(const volatile uint8_t* data, 
                               uint16_t size, MediatorEvent::Type event);

  virtual void execute(); // Common execute implementation
  
  bool isComplete();

protected:
  static const uint8_t MAX_CHANNELS = MAX_SENSORS;

  uint16_t length;
  uint32_t counter;
  uint8_t ch_index;
  bool answerReceived;
  MediatorEvent::Type rxEvent;
  sensorsPollScript& script;
  bool coroutineOnce;
  ccrContext m_ctx;

  virtual void handleResponse(bool answerReceived) = 0;
  virtual bool isChannelExist(uint8_t ch_index) = 0;
  virtual void onEndCycle();
  virtual void handleDurationReached();

private:
  virtual bool processMessage(const volatile uint8_t* data, uint16_t size);
  void setChannel(uint8_t channel);
  void clearChannel();
  void send(const char* data, size_t length);
  void sendRBLabSensCommand(RBLabSensCommand::Type cmd);
  void run(ccrContParam); // Common coroutine implementation
  
  bool complete;

  // Prevent copying
  SensorsTaskBase(const SensorsTaskBase&);
  SensorsTaskBase& operator=(const SensorsTaskBase&);
};





