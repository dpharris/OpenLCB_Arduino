#include "Locomotive.h"

void Locomotive::init(void)
{
  available = true;
  state = LOCOMOTIVE_INITIAL;
}

void Locomotive::update(void)
{
  //handle periodic sending of DCC packet for keep-alive TOD
  uint16_t cur_time = millis();
  if((cur_time - timer) > 5000) //5 seconds have passed, time to update!
  {
    timer = cur_time;
    packetScheduler.setSpeed128(getDCCAddress(), speed*direction);
  }
}

bool Locomotive::processDatagram(void)
{
  if(_rxDatagramBuffer->data[0] == DATAGRAM_MOTIVE) //is this a datagram for loco control?
  {
    switch(_rxDatagramBuffer->data[1])
    {
    case DATAGRAM_MOTIVE_ATTACH:
      Serial.println("Request attach!");
      return attachDatagram();
    case DATAGRAM_MOTIVE_RELEASE:
      Serial.println("Request release!");
      return releaseDatagram();
    case DATAGRAM_MOTIVE_SETSPEED:
      Serial.println("Request set speed!");
      return setSpeedDatagram();
    case DATAGRAM_MOTIVE_SETFUNCTION:
      Serial.println("Request set function!");
      return setFunctionDatagram();
    }
  }
  return false;
}

void Locomotive::datagramResult(bool accepted, uint16_t errorcode)
{
  // cases to handle:
  //  *attached NAKd (make self available) TODO
  if((state == LOCOMOTIVE_ATTACHING) && accepted)
  {
    state = LOCOMOTIVE_ATTACHED;
  }
  else
  {
    state = LOCOMOTIVE_INITIAL;
    available = true;
  }
}

bool Locomotive::attachDatagram(void)
{
  OLCB_Datagram d;
  d.destination.copy(&(_rxDatagramBuffer->source));
  d.data[0] = DATAGRAM_MOTIVE;
  d.length = 2;
  if(available) //if free!
  {
    throttle.copy(&(_rxDatagramBuffer->source));
    available = false;
    d.data[1] = DATAGRAM_MOTIVE_ATTACHED;
    state = LOCOMOTIVE_ATTACHING;
    Serial.println("  attaching!");
  }
  else //not free!
  {
    d.data[1] = DATAGRAM_MOTIVE_ATTACH_DENIED;
    Serial.println("  attach denied!");
  }
  sendDatagram(&d);
  return true;
}

bool Locomotive::releaseDatagram(void)
{
  if(!available && _rxDatagramBuffer->source == throttle)
  {
    Serial.println("  releasing!");
    OLCB_Datagram d;
    d.destination.copy(&(_rxDatagramBuffer->source));
    d.length = 2;
    d.data[0] = DATAGRAM_MOTIVE;
    d.data[1] = DATAGRAM_MOTIVE_RELEASED;
    sendDatagram(&d); //THIS IS BAD FORM releasing before making sure release is ACK'd. TODO
    available = false;
    state = LOCOMOTIVE_INITIAL;
    return true;
  }
  Serial.println("  not attached!");
  return false; //what you talkin' 'bout, Willis?
}

bool Locomotive::setSpeedDatagram(void)
{
  //TODO CHECK DATAGRAM LENGTH!!
  if(!available && _rxDatagramBuffer->source == throttle)
  {
    Serial.println("  setting speed!");
    //Speed in data[3], and direction in data[2].
    speed = (_rxDatagramBuffer->data[3] / 100.0) * 127;
    if(_rxDatagramBuffer->data[2] == 1)
      direction = 1;
    else
      direction = -1;
    packetScheduler.setSpeed128(getDCCAddress(), speed*direction);
    return true;
  }
  Serial.println("  not attached!");
  return false;
}

bool Locomotive::setFunctionDatagram(void)
{
  //TODO CHECK DATAGRAM LENGTH!!
  if(!available && _rxDatagramBuffer->source == throttle)
  {
    Serial.println("  setting function!");
    //function no. in data[2] and on/off in data[3]
    if(_rxDatagramBuffer->data[3]) //function on
      functions |= (1<<_rxDatagramBuffer->data[2]);
    else //function off
    functions &= ~(1<<_rxDatagramBuffer->data[2]);
    packetScheduler.setFunctions(getDCCAddress(), (functions>>16), (functions&0xFF), 0);
    return true;
  }
  Serial.println("  not attached!");
  return false;
}

uint16_t Locomotive::getDCCAddress(void)
{
  return ((uint16_t)(NID->val[4])<<16) | (NID->val[5]);
}
