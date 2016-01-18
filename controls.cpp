#include "Arduino.h"
#include "controls.h"

const PROGMEM char pinWriteDebug[] = "Writing pin %d to %d";

Control::Control(const uint8_t nodeId, const uint8_t controlId)
{
    id = nodeId << 4;
    id |= controlId;
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
    switchPin(switchPin),
    relayPin(relayPin),
    toggleControl(switchPin, this),
    Control(nodeId, controlId)
{
   pinMode(relayPin, OUTPUT);
}

void SwitchedToggleControl::setToggleControl(bool state)
{
    Serial.println(F("Recieved state command"));
    enableControl = state ? RELAY_ON : RELAY_OFF;
    digitalWrite(relayPin, enableControl);
}

void SwitchedToggleControl::loop()
{
    toggleControl.readButton();
}

void SwitchedToggleControl::callback(int state)
{
    Serial.println(F("Switched"));
    uint8_t id = getId();
    RadioHeader header = (RadioHeader) { id, SWITCHEDTOGGLE_STATUS };

    enableControl = !enableControl;

    char buff[50];
    sprintf_P(buff, pinWriteDebug, relayPin, enableControl);
    Serial.println(buff);

    digitalWrite(relayPin, enableControl);
    SwitchedToggle body = (SwitchedToggle) { enableControl == RELAY_ON ? 1 : 0 };
    RadioNode::sendData(&header, (const void *)(&body), sizeof(SwitchedToggle));
}

EndLimitSwitchControl::EndLimitSwitchControl(const uint8_t switchPin, const uint8_t nodeId, const uint8_t controlId) :
    switchPin(switchPin),
    limitControl(switchPin, this),
    Control(nodeId, controlId)
{
}

void EndLimitSwitchControl::loop()
{
    limitControl.readButton();
}

void EndLimitSwitchControl::callback(int status)
{
    Serial.println(F("End limit state change"));
    uint8_t id = getId();
    RadioHeader header = (RadioHeader) { id, ENDLIMIT };

    char buff[50];
    EndLimit body = (EndLimit) { status };
    RadioNode::sendData(&header, (const void *)(&body), sizeof(EndLimit));
}

GarageLimitControlGroup::GarageLimitControlGroup(const uint8_t upLimitSwitchPin, const uint8_t downLimitSwitchPin, const uint8_t nodeId, const uint8_t upLimitControlId, const uint8_t downLimitControlId) :
    upLimitControl(upLimitSwitchPin, nodeId, upLimitControlId),
    downLimitControl(downLimitSwitchPin, nodeId, downLimitControlId)
{
}

void GarageLimitControlGroup::loop()
{
    upLimitControl.loop();
    downLimitControl.loop();
}
