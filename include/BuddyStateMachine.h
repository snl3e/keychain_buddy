#pragma once
#include <Cpp_StateMachine.h>

class BuddyStateMachine : public StateMachine {
    public:
        BuddyStateMachine();

        enum States{
            ST_IDLE,
            ST_INTERACT,
            ST_MENU,
            ST_GAME,
            ST_MAX_STATES
        };

        //Events
        void enterIdle();
        void enterGame();
        void enterMenu();
        

    private:
        STATE_DECLARE(BuddyStateMachine, Idle, NoEventData);
        STATE_DECLARE(BuddyStateMachine, Interact, NoEventData);
        STATE_DECLARE(BuddyStateMachine, Menu, NoEventData);
        STATE_DECLARE(BuddyStateMachine, Game, NoEventData);

        BEGIN_STATE_MAP
            STATE_MAP_ENTRY(&Idle)
            STATE_MAP_ENTRY(&Interact)
            STATE_MAP_ENTRY(&Menu)
            STATE_MAP_ENTRY(&Game)
        END_STATE_MAP
};