#include "Arduino.h"
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
    uint8_t body[sizeof(Registration)];
    RadioHeader header = (RadioHeader) { id, REGISTRATION };
    RadioNode::sendData(&header, (const void *)body, sizeof(body));
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

void SwitchedToggleControl::switchAction(int state)
{
    uint8_t id = getId();
    RadioHeader header = (RadioHeader) { id, SWITCHEDTOGGLE_STATUS };

    enableControl = !enableControl;
    SwitchedToggle body = (SwitchedToggle) { enableControl };
    RadioNode::sendData(&header, (const void *)&body, RF69_MAX_DATA_LEN);
}

void SwitchedToggleControl::setToggleControl(bool state)
{
    enableControl = state ? RELAY_ON : RELAY_OFF;
}

void SwitchedToggleControl::loop()
{
    toggleControl.readButton();
}
