#ifndef MTM_HW1_EUROVISION_STATE_H
#define MTM_HW1_EUROVISION_STATE_H

#include <stdbool.h>
#include "map.h"

/**
 * Eurovision State ADT
 *
 * The following functions are available:
 *
 * stateCreate                        - Allocate a new State according to the arguments.
 * stateDestroy                       - Deallocate the State send as an argument.
 * stateCopy                          - Return a copy of the State send as an argument.
 * stateGetName                       - Return the State name.
 * stateGetSong                       - Return the State song name.
 * stateGetId                         - Return the State Id.
 * stateGetId                         - Return the State final score.
 * stateGetCitizenVotes               - Return the State votes map container.
 * stateSetScore                      - Change the state final score.
 * stateGetNumOfVotes                 - Return the number of votes to a specific state.
 * stateAddVote                       - Add one vote to a specific state.
 * stateDeleteVote                    - Delete one vote from a specific state.
 * stateDeleteAllVotesOfSpecificState - Delete all votes for specific state.
 * stateDeleteAllVotes                - Deallocate all the citizenVotes map container.
 * stateVotesContain                  - Check if citizen votes contain a vote to a
 *                                      specific state.
 * stateMapKeyCompare                 - compare two States according to their ids as a
 *                                      MapKeyElement.
 * stateMapDataElementCopy            - Return a copy of the State send as an argument
 *                                      as a MapDataElement.
 * stateMapKeyElementCopy             - Return a copy of the State id send as an argument
 *                                      as a MapKeyElement.
 * stateMapDataElementFree            - Deallocate the State send as an argument.
 * stateMapKeyElementFree             - Deallocate the State id send as an argument.
 * stateErrorTranslate                - Changing a MapResult type to StateResult type.
*/

/** Type for defining a State */
typedef struct State_t *State;

/** Type used for returning error codes from map functions */
typedef enum StateResult_t{
    STATE_NULL_ARGUMENT,
    STATE_OUT_OF_MEMORY,
    STATE_INVALID_ID,
   /* STATE_INVALID_NAME,
    STATE_VOTE_STATE_DOESNT_EXIST,*/
    STATE_SUCCESS
}StateResult;

/**
* stateCreate: Allocates a new State
*
* @param stateId - the state id value, most be positive number or 0.
* @param stateName - the new state name as a char*.
* @param stateSong - the new state song name as a char*.
* @return
* 	NULL - if one of the parameters is NULL or allocations failed.
* 	NULL - if stateId<0
* 	A new State in case of success with no votes.
*/
State stateCreate(int stateId, const char* stateName, const char* stateSong);

/**
* stateDestroy: Deallocate a state
*
* @param state - the state to Deallocate
*/
void stateDestroy(State state);

/**
* stateCopy: Allocates copy of a state
*
* @param state - the state to copy
* @return
* 	NULL - if the parameter is NULL or allocations failed.
* 	A copy of the State send as parameter in case of success.
*/
State stateCopy(State state);

/**
* stateGetId - Function to get the state id
*
* @param state - state to get its id
* @return
* 	NULL - if the parameter send is NULL
* 	the state id value as a int
*/
int stateGetId(State state);

/**
* stateGetName - Function to get the state name as a char*
*
* @param state - state to get its name.
* @return
* 	NULL - if the parameter send is NULL
* 	pointer to the state name in type of char*
*/
char* stateGetName(State state);

/**
* stateGetSong - Function to get the state songs name as a char*
*
* @param state - state to get its songs name.
* @return
* 	NULL - if the parameter send is NULL
* 	pointer to the state song name in type of char*
*/
char* stateGetSong(State state);

/**
* stateGetFinalScore - Function to get the state final score
*
* @param state - state to get its current final score
* @return
* 	-1 - if the parameter send is NULL
* 	the state final score value as a double
*/
double stateGetFinalScore(State state);

/**
* stateGetCitizenVotes - Function to get the state citizen votes map container
*
* @param state - state to get its citizens votes
* @return
* 	NULL - if the parameter send is NULL
* 	a pointer to the states citizen votes map container
*/
Map stateGetCitizenVotes(State state);

/**
* stateSetScore - Function to change the state final score
*
* @param state - state to get its citizens votes
* @param newScore - the new score to set
* @return
* 	STATE_NULL_ARGUMENT - if one of the parameters send is NULL
* 	STATE_SUCCESS - if the score changed successfully
*/
StateResult stateSetScore(State state, double newScore);

/**
* stateGetNumOfVotes - Function to get the num of votes for specific state
*
* @param state - the state that voted
* @param stateToSearch - state id to search in the state votes
* @return
* 	-1 - if one of the parameters send is NULL
 * 	-1 - if the stateToSearch is a negative number
* 	num of votes to the specific state in case of success
*/
int stateGetNumOfVotes(State state, int stateToSearch);

/**
* stateAddVote - add one vote to a specific state
*
* @param state - the state that votes
* @param stateToVoteId - state id to add one vote to
* @return
* 	STATE_NULL_ARGUMENT - if one of the parameters send is NULL
* 	STATE_INVALID_ID - if stateToVoteId is a negative number
* 	STATE_OUT_OF_MEMORY - in case of a allocation error
* 	STATE_SUCCESS - if one vote added
*/
StateResult stateAddVote(State state, int stateToVoteId);

/**
* stateDeleteVote - Delete one vote from a specific state
*
* @param state - the state that votes
* @param stateToDeleteVote - state id to delete one vote from
* @return
* 	STATE_NULL_ARGUMENT - if one of the parameters send is NULL
* 	STATE_INVALID_ID - if stateToDeleteVote is a negative number
* 	STATE_SUCCESS - if one vote deleted or if the stateToDeleteVote has 0 votes
*/
StateResult stateDeleteVote(State state, int stateToDeleteVote);

/**
* stateDeleteAllVotesOfSpecificState - Delete all votes from a specific state
*
* @param state - the state that votes
* @param stateToDeleteVotes - state id to delete all votes from
* @return
* 	STATE_NULL_ARGUMENT - if one of the parameters send is NULL
* 	STATE_INVALID_ID - if stateToDeleteVotes is a negative number
* 	STATE_SUCCESS - if all votes of a spesific state deleted
*/
StateResult stateDeleteAllVotesOfSpecificState(State state, int stateToDeleteVotes);

/**
* stateMapKeyCompare: compare two judges ids
*
* @param stateId1 - the first state id to compare
* @param stateId2 - the second state id to compare
* @return
* 		A positive integer if the first element is greater;
* 		0 if they're equal;
*		A negative integer if the second element is greater.
*/
int stateMapKeyCompare(MapKeyElement stateId1, MapKeyElement stateId2);

/**
* stateMapDataElementCopy: Allocate a copy of state for a map container
*
* @param state - the state to copy
* @return
* 		MapDataElement that is a copy of the state send as a parameter
*/
MapDataElement stateMapDataElementCopy(MapDataElement state);

/**
* stateMapKeyElementCopy: Allocate a copy of state id for a map container
*
* @param stateId - the state id to copy
* @return
* 		MapKeyElement that is a copy of the state id send as a parameter
*/
MapKeyElement stateMapKeyElementCopy(MapKeyElement stateId);

/**
* stateMapDataElementFree: Function to deallocate a state for a map container
*
* @param state - the state to deallocate
*/
void stateMapDataElementFree(MapDataElement state);

/**
* stateMapKeyElementFree: Function to deallocate a state id for a map container
*
* @param stateId - the state to deallocate
*/
void stateMapKeyElementFree(MapKeyElement stateId);

/**
* stateErrorTranslate: transform a Map_result type to State_result type
*
* @param result - the Map_result to transform
* @return
* 		STATE_SUCCESS - if result is MAP_SUCCESS
* 		STATE_NULL_ARGUMENT - if result is MAP_NULL_ARGUMENT
* 		STATE_OUT_OF_MEMORY - if result is MAP_OUT_OF_MEMORY
* 		STATE_INVALID_ID - if result is MAP_ITEM_DOES_NOT_EXIST or
* 		                   MAP_ITEM_ALREADY_EXISTS
* 		STATE_OUT_OF_MEMORY - other wise
*/
StateResult stateErrorTranslate(MapResult result);

#endif
