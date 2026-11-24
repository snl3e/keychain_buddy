#include "../include/BuddyStateMachine.h"

BuddyStateMachine::BuddyStateMachine() : StateMachine(ST_MAX_STATES, ST_IDLE){
}

void BuddyStateMachine::enterIdle(){
    BEGIN_TRANSITION_MAP
        TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)  //Idle
        TRANSITION_MAP_ENTRY(ST_IDLE)        //Interact
        TRANSITION_MAP_ENTRY(ST_IDLE)        //Menu
        TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)  //Game
    END_TRANSITION_MAP(NULL)
}

void BuddyStateMachine::enterGame(){
    BEGIN_TRANSITION_MAP
        TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)  //Idle
        TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)  //Interact
        TRANSITION_MAP_ENTRY(ST_GAME)        //Menu
        TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)  //Game
    END_TRANSITION_MAP(NULL)
}

void BuddyStateMachine::enterMenu(){
    BEGIN_TRANSITION_MAP
        TRANSITION_MAP_ENTRY(ST_MENU)        //Idle
        TRANSITION_MAP_ENTRY(ST_MENU)        //Interact
        TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)  //Menu
        TRANSITION_MAP_ENTRY(ST_MENU)        //Game
    END_TRANSITION_MAP(NULL)
}

STATE_DEFINE(BuddyStateMachine, Idle, NoEventData){
    
}

STATE_DEFINE(BuddyStateMachine, Interact, NoEventData){
    
}

STATE_DEFINE(BuddyStateMachine, Menu, NoEventData){
    
}

STATE_DEFINE(BuddyStateMachine, Game, NoEventData){
    
}