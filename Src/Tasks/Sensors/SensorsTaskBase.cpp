 // SensorsTaskBase.cpp

#include  "SensorsTaskBase.h"


SensorsTaskBase::SensorsTaskBase(sensorsPollScript& script) :
  script(script),
  ch_index(MAX_CHANNELS - 1),
  counter(0),
  rxEvent(MediatorEvent::NONE),
  answerReceived(false),
  coroutineOnce(false),
  complete(false),
  m_ctx(NULL)
{
  sender = NULL; //since sender is a member of the parent class
                  //it can't be in the initializer list
  P5DIR |= BIT0 | BIT1 | BIT2; //debug
  P5OUT_bit.P0 = 1; P5OUT_bit.P1 = 1; P5OUT_bit.P2 = 1; //debug
}


void SensorsTaskBase::execute() {
  if (!coroutineOnce) {
    run(&m_ctx);
  }

  counter++;
  if ((counter % script.period_ms) == 0) {
    onEndCycle();
  }

  if (counter >= script.duration_ms) {
    handleDurationReached();
  }
}


void SensorsTaskBase::run(ccrContParam) {
  ccrBeginContext;
  uint8_t ch;
  uint8_t i;
  uint8_t delayCount; // Ticks remaining in delay
  ccrEndContext(ctx);

  ccrBegin(ctx);
  
  for (ctx->ch = 0; ctx->ch < MAX_CHANNELS; ++ctx->ch) {
    // Set up current channel
    ch_index = ctx->ch;
    if (!isChannelExist(ch_index)) continue;
    setChannel(ch_index);

    ccrDelay(ctx, 2);
    answerReceived = false;
    sendRBLabSensCommand(script.toRBLabSensParser);
    
    // Wait for response with retries
    ctx->i = 0;
    while (ctx->i < 14) {
      ccrDelay(ctx, 10);
      if (answerReceived) break;
      if (ctx->i++ == 7)sendRBLabSensCommand(script.toRBLabSensParser);
    }

    handleResponse(answerReceived);
    clearChannel();
    ccrDelay(ctx, 2);
  }

  coroutineOnce = true;
  ccrFinishV;
}



void SensorsTaskBase::onEndCycle() {
  coroutineOnce = script.singleCycleExecution;
}

void SensorsTaskBase::handleDurationReached() {
  complete = true;
}

bool SensorsTaskBase::isComplete(){
  return complete;
}

void  SensorsTaskBase::onConnect(){
  counter = 0;
}


void SensorsTaskBase::sendRBLabSensCommand(RBLabSensCommand::Type cmd){
    uint8_t command = static_cast<uint8_t>(cmd);
    uint8_t* buffer = &command;
    uint16_t length = 0;
    /**
    * buffer[0] - comntains request
    * return answer in buffer,  
    */
    RBLabSensParser::instance().answer(buffer, length);
    send(reinterpret_cast<const char*>(buffer), length);
}


void SensorsTaskBase::send(const char* data, size_t length){
  if(sender) sender->send(data, length);
}


void SensorsTaskBase::onMediatorEvent(const volatile uint8_t* data, 
                                      uint16_t size, MediatorEvent::Type event){

    answerReceived = processMessage(data, size);

}


bool SensorsTaskBase::processMessage(const volatile uint8_t* data, 
                                           uint16_t size){
  
  parse_error_code err = RBLabSensParser::instance().parseFrame(data, size);
  
  return (err == PARSE_OK)? true : false;
}


void  SensorsTaskBase::setChannel(uint8_t ch_index){
  RBLabSensParser::instance().setChannel(ch_index);
  Channels::instance().set(ch_index); //to do: move to RBLabSensParser
}


void SensorsTaskBase::clearChannel() {
  Channels::instance().clear();
}


void SensorsTaskBase::connectSender(ISender* sender){
    this->sender = sender;
}