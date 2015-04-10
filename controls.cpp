#include "Arduino.h"
#include "../debounced/debounced.h"
#include "controls.h"

Control::Control(const uint8_t nodeId, const uint8_t controlId)
{
    id = nodeId << 4;
    id &= controlId;
}

uint8_t Control::getId()
{
    return id;
}

void Control::registerControl()
{
    uint8_t data[RF69_MAX_DATA_LEN];
    uint8_t body[sizeof(Registration)];
    RadioHeader header = (RadioHeader) { id, REGISTRATION };

    memcpy(data, &header, LHEADER);
    memcpy(&data[LHEADER], body, sizeof(Registration));

    RadioNode::sendData((const void *)data, RF69_MAX_DATA_LEN);
}

void Control::loop()
{
}

SwitchedToggleControl::SwitchedToggleControl(const uint8_t switchPin, const uint8_t relayPin, const uint8_t nodeId, const uint8_t controlId) : 
    relayPin(relayPin),
    switchPin(switchPin),
    toggleControl(switchPin, (debfuncptr)&SwitchedToggleControl::switchAction),
    Control(nodeId, controlId)
{
}

void SwitchedToggleControl::getPacket(uint8_t *buff)
{
    uint8_t id = getId();
    RadioHeader header = (RadioHeader) { id, SWITCHEDTOGGLE_STATUS };
    memcpy(buff, &header, LHEADER);

    SwitchedToggle body = (SwitchedToggle) { enableControl };
    memcpy(&buff[LHEADER], &body, sizeof(SwitchedToggle));
}

void SwitchedToggleControl::switchAction(int state)
{
    enableControl = !enableControl;
    uint8_t data[RF69_MAX_DATA_LEN];
    getPacket(data);
    RadioNode::sendData((const void *)data, RF69_MAX_DATA_LEN);
}

void SwitchedToggleControl::setToggleControl(bool state)
{
    enableControl = state ? RELAY_ON : RELAY_OFF;
}

void SwitchedToggleControl::loop()
{
    toggleControl.readButton();
}
