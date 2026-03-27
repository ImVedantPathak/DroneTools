#pragma once

#include <Arduino.h>

struct MultiButtonConfig {
    int debounceDecay;
    int singleClickDelay;
    int longClickDelay;
};

const static MultiButtonConfig DEFAULT_MULTIBUTTON_CONFIG = { 20, 250, 300 };

class MultiButton {
public:
    MultiButton()
        : MultiButton(&DEFAULT_MULTIBUTTON_CONFIG) {
    };

    MultiButton(const MultiButtonConfig* configuration)
        : _lastTransition(millis()), _state(StateIdle), _new(false) {
        _configPtr = configuration;
    };

    void update(bool pressed) {
        _new = false;

        if (!pressed && _state == StateIdle) {
            return;
        }

        unsigned int now = millis();
        int diff = now - _lastTransition;

        State next = StateIdle;
        switch (_state) {
            case StateIdle:                next = _checkIdle(pressed, diff);                break;
            case StateDebounce:            next = _checkDebounce(pressed, diff);            break;
            case StatePressed:             next = _checkPressed(pressed, diff);             break;
            case StateClickUp:             next = _checkClickUp(pressed, diff);             break;
            case StateClickIdle:           next = _checkClickIdle(pressed, diff);           break;
            case StateSingleClick:         next = _checkSingleClick(pressed, diff);         break;
            case StateDoubleClickDebounce: next = _checkDoubleClickDebounce(pressed, diff); break;
            case StateDoubleClick:         next = _checkDoubleClick(pressed, diff);         break;
            case StateLongClick:           next = _checkLongClick(pressed, diff);           break;
            case StateOtherUp:             next = _checkOtherUp(pressed, diff);             break;
        }

        if (next != _state) {
            _lastTransition = now;
            _state = next;
            _new = true;
        }
    }

    bool isClick() const {
        return _new && (_state == StatePressed || _state == StateDoubleClick);
    }

    bool isSingleClick() {
        return _new && _state == StateSingleClick;
    }

    bool isDoubleClick() {
        return _new && _state == StateDoubleClick;
    }

    bool isLongClick() {
        return _new && _state == StateLongClick;
    }

    bool isReleased() {
        return _new && (_state == StateClickUp || _state == StateOtherUp);
    }

private:
    const MultiButtonConfig* _configPtr;

    enum State {
        StateIdle,
        StateDebounce,
        StatePressed,
        StateClickUp,
        StateClickIdle,
        StateSingleClick,
        StateDoubleClickDebounce,
        StateDoubleClick,
        StateLongClick,
        StateOtherUp,
    };

    unsigned int _lastTransition;
    State _state;
    bool _new;

    State _checkIdle(bool pressed, int diff) {
        (void)diff;
        return pressed ? StateDebounce : StateIdle;
    }

    State _checkDebounce(bool pressed, int diff) {
        if (!pressed) {
            return StateIdle;
        }
        if (diff >= _configPtr->debounceDecay) {
            return StatePressed;
        }
        return StateDebounce;
    }

    State _checkPressed(bool pressed, int diff) {
        if (!pressed) {
            return StateClickUp;
        }
        if (diff >= _configPtr->longClickDelay) {
            return StateLongClick;
        }
        return StatePressed;
    }

    State _checkClickUp(bool pressed, int diff) {
        (void)pressed;
        (void)diff;
        return StateClickIdle;
    }

    State _checkClickIdle(bool pressed, int diff) {
        if (pressed) {
            return StateDoubleClickDebounce;
        }
        if (diff >= _configPtr->singleClickDelay) {
            return StateSingleClick;
        }
        return StateClickIdle;
    }

    State _checkSingleClick(bool pressed, int diff) {
        (void)pressed;
        (void)diff;
        return StateIdle;
    }

    State _checkDoubleClickDebounce(bool pressed, int diff) {
        if (!pressed) {
            return StateClickIdle;
        }
        if (diff >= _configPtr->debounceDecay) {
            return StateDoubleClick;
        }
        return StateDoubleClickDebounce;
    }

    State _checkDoubleClick(bool pressed, int diff) {
        (void)diff;
        if (!pressed) {
            return StateOtherUp;
        }
        return StateDoubleClick;
    }

    State _checkLongClick(bool pressed, int diff) {
        (void)diff;
        if (!pressed) {
            return StateOtherUp;
        }
        return StateLongClick;
    }

    State _checkOtherUp(bool pressed, int diff) {
        (void)pressed;
        (void)diff;
        return StateIdle;
    }
};