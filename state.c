#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "set.h"
#include "state.h"

/** Function to copy int for map container */
MapDataElement stateCopyInt(MapDataElement n);

/** Function to deallocate int for map container */
void stateFreeInt(MapKeyElement n);

/** Function to compare two ints for map container,
 *  return a positive number if the first int is greater,
 *  0 if they equal, and a negative number if the second one is greater
 */
int stateCompareInts(MapKeyElement n1, MapKeyElement n2);

struct State_t{
    int id;
    char* name;
    char* song;
    double finalScore;
    Map citizenVotes;
};

MapDataElement stateCopyInt(MapDataElement n) {
    if (!n) {
        return NULL;
    }
    int *copy = malloc(sizeof(*copy));
    if (!copy) {
        return NULL;
    }
    *copy = *(int *) n;
    return copy;
}

void stateFreeInt(MapKeyElement n){
    free(n);
}

int stateCompareInts(MapKeyElement n1, MapKeyElement n2) {
    return (*(int *) n1 - *(int *) n2);
}

State stateCreate(int stateId, const char* stateName, const char* stateSong){
    if(stateId<0||!stateName||!stateSong){
        return NULL;
    }
    State newState = malloc(sizeof(*newState));
    if(!newState) {
        return NULL;
    }
    newState->id = stateId;
    newState->name = malloc((strlen(stateName)*sizeof(char))+1);
    if(!newState->name) {
        stateDestroy(newState);
        return NULL;
    }
    strcpy(newState->name,stateName);
    newState->song = malloc((strlen(stateSong)*sizeof(char))+1);
    if(!newState->song) {
        stateDestroy(newState);
        return NULL;
    }
    strcpy(newState->song,stateSong);
    newState->finalScore = 0;
    newState->citizenVotes = mapCreate(stateCopyInt, stateCopyInt, stateFreeInt, stateFreeInt, stateCompareInts);
    if(!newState->citizenVotes)  {
        stateDestroy(newState);
        return NULL;
    }
    return newState;
}

void stateDestroy(State state){
    if(!state) return;
    free(state->name);
    free(state->song);
    mapDestroy(state->citizenVotes);
    free(state);
}

State stateCopy(State state){
    if(!state) return NULL;
    State newState = stateCreate(state->id, state->name, state->song);
    if(!newState) return NULL;
    newState->finalScore = state->finalScore;
    mapDestroy(newState->citizenVotes);
    newState->citizenVotes = mapCopy(state->citizenVotes);
    return newState;
}



int stateGetId(State state){
    if(!state) return -1;
    return state->id;
}

char* stateGetName(State state){
    if(!state) return NULL;
    return state->name;
}

char* stateGetSong(State state){
    if(!state) return NULL;
    return state->song;
}

double stateGetFinalScore(State state){
    if(!state) return -1;
    return state->finalScore;
}

Map stateGetCitizenVotes(State state){
    if(!state) return NULL;
    return state->citizenVotes;
}

int stateGetNumOfVotes(State state, int stateToSearch){
    if(!state) return -1;
    if(stateToSearch<0) return -1;
    int numOfVotes = 0;
    if(mapContains(state->citizenVotes,&stateToSearch)) {
        numOfVotes = *(int *)mapGet(state->citizenVotes, &stateToSearch);
    }
    return numOfVotes;
}


StateResult stateSetScore(State state, double newScore){
    if(!state) return STATE_NULL_ARGUMENT;
    state->finalScore = newScore;
    return STATE_SUCCESS;
}

StateResult stateAddVote(State state, int stateToVoteId){
    if(!state) return STATE_NULL_ARGUMENT;
    if(stateToVoteId<0) return STATE_INVALID_ID;
    int tmp;
    MapResult result;
    tmp = stateGetNumOfVotes(state, stateToVoteId);
    tmp++;
    result = mapPut(state->citizenVotes,&stateToVoteId,&tmp);
    return stateErrorTranslate(result);
}

StateResult stateDeleteVote(State state, int stateToDeleteVote){
    if(!state) return STATE_NULL_ARGUMENT;
    if(stateToDeleteVote<0) return STATE_INVALID_ID;
    int tmp = 0;
    MapResult result;
    tmp = stateGetNumOfVotes(state, stateToDeleteVote);
    if(tmp == 0) {
        return STATE_SUCCESS;
    }
    if(tmp == 1){
        return stateDeleteAllVotesOfSpecificState(state,stateToDeleteVote);
    }
    tmp--;
    result = mapPut(state->citizenVotes,&stateToDeleteVote,&tmp);
    return stateErrorTranslate(result);
}

StateResult stateDeleteAllVotesOfSpecificState(State state, int stateToDeleteVotes){
    if(!state) return STATE_NULL_ARGUMENT;
    if(stateToDeleteVotes<0) return STATE_INVALID_ID;
    MapResult result = mapRemove(state->citizenVotes,&stateToDeleteVotes);
    return stateErrorTranslate(result);
}

int stateMapKeyCompare(MapKeyElement stateId1, MapKeyElement stateId2){
    return stateCompareInts(stateId1, stateId2);
}

MapDataElement stateMapDataElementCopy(MapDataElement state){
    return stateCopy((State)state);
}

MapKeyElement stateMapKeyElementCopy(MapKeyElement stateId){
    return stateCopyInt(stateId);
}

void stateMapDataElementFree(MapDataElement state){
    stateDestroy((State)state);
}

void stateMapKeyElementFree(MapKeyElement stateId){
    stateFreeInt(stateId);
}

StateResult stateErrorTranslate(MapResult result){
    switch(result){
        case MAP_SUCCESS:
            return STATE_SUCCESS;

        case MAP_NULL_ARGUMENT:
            return STATE_NULL_ARGUMENT;

        case MAP_OUT_OF_MEMORY:
            return STATE_OUT_OF_MEMORY;

        case MAP_ITEM_DOES_NOT_EXIST:
        case MAP_ITEM_ALREADY_EXISTS:
            return STATE_INVALID_ID;
    }
    return STATE_OUT_OF_MEMORY;
}
