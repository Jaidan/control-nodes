#ifndef controls_h
#define controls_h

#define TOPIC_LENGTH 57 

#define RELAY_ON 0
#define RELAY_OFF 1

#define REGISTRATION 1
#define SWITCHEDTOGGLE_STATUS 2

#include <../debounced/debounced.h>
#include <../radioNode/radioNode.h>

typedef struct {
    uint8_t id; // Should be 2 nibbles containing both node and control ids
    uint8_t controlType; // waste of space =( we're unlikely to ever have 255 control types
    char topic[TOPIC_LENGTH] = {0};
} Registration;

typedef struct {
    bool status;
} SwitchedToggle;

class Control
{
    public:
        Control(const uint8_t nodeId, const uint8_t controlId);
        uint8_t getId();
        void setToggleControl(bool status);
        void registerControl();
        void loop();

    private:
        uint8_t id;
};
class SwitchedToggleControl : public Control, public DebouncedCallback
{
    public:
        SwitchedToggleControl(const uint8_t switchPin, const uint8_t relayPin, const uint8_t nodeId, const uint8_t controlId);
        void setToggleControl(bool status);
        void loop();
        virtual void callback(int state);

    private:
        Debounced toggleControl;
        uint8_t switchPin;
        uint8_t relayPin;
        bool enableControl = RELAY_OFF;
};
#endif
