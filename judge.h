#ifndef MTM_HW1_EUROVISION_JUDGE_H
#define MTM_HW1_EUROVISION_JUDGE_H

#include <stdbool.h>
#include "map.h"

/**
 * Eurovision Judge ADT
 *
 * The following functions are available:
 *
 * judgeGetName            - Return the Judge name
 * judgeGetId              - Return the Judge Id.
 * judgeGetResult          - Return the judge votes array.
 * judgeCreate             - Allocate a new Judge according to the arguments.
 * judgeCopy               - Return a copy of the Judge send as an argument.
 * judgeDestroy            - Deallocate the Judge send as an argument.
 * judgeVotedToState       - Return true if the Judge as an argument has voted to
 *                           the State that has the id send as an argument, false
 *                           otherwise.
 * judgeMapKeyCompare      - compare two Judges according to their ids as a
 *                           MapKeyElement.
 * judgeMapDataElementCopy - Return a copy of the Judge send as an argument
 *                           as a MapDataElement
 * judgeMapKeyElementCopy  - Return a copy of the JudgeId send as an argument
 *                           as a MapKeyElement
 * judgeMapDataElementFree - Deallocate the Judge send as an argument
 * judgeMapKeyElementFree  - Deallocate the JudgeId send as an argument
*/

#define NUM_OF_JUDGE_RESULTS 10
/** Type for defining a Eurovision Judge */
typedef struct Judge_t *Judge;

/**
* judgeGetName - Function to get the judge name as a char*
*
* @param judge - Eurovision Judge to get his name.
* @return
* 	NULL - if the parameter send is NULL
* 	pointer to the judge name in type of char*
*/
char* judgeGetName(Judge judge);

/**
* judgeGetId - Function to get the judge id
*
* @param judge - Eurovision Judge to get his id.
* @return
* 	NULL - if the parameter send is NULL
* 	the judge id value as a int
*/
int judgeGetId(Judge judge);

/**
* judgeGetResults - Function to get the judge results array
*
* @param judge - Eurovision Judge to get his results.
* @return
* 	NULL - if the parameter send is NULL
* 	a pointer to the judge results array
*/
int* judgeGetResults(Judge judge);

/**
* judgeCreate: Allocates a new Eurovision judge
*
* @param judgeId - the judge id value, most be positive number or 0
* @param judgeName - the new judge name as a char*
* @param judgeResults - the new judge results, an array of 10 integers
* @return
* 	NULL - if one of the parameters is NULL or allocations failed.
 * 	NULL - if judgeId<0
* 	A new Judge in case of success.
*/
Judge judgeCreate(int judgeId, const char* judgeName, int judgeResults[NUM_OF_JUDGE_RESULTS]);

/**
* judgeCopy: Allocates a copy of Eurovision judge
*
* @param judge - the judge to copy
* @return
* 	NULL - if the parameter is NULL or allocations failed.
* 	A copy of the Judge send as parameter in case of success.
*/
Judge judgeCopy(Judge judge);

/**
* judgeDestroy: Deallocate a Eurovision judge
*
* @param judge - the judge to Deallocate
*/
void judgeDestroy(Judge judge);

/**
* judgeVotedToState: Check if a Eurovision judge has voted to a specific state
*
* @param judge - the judge to search in his results
* @param stateId - a state id to search
* @return
* 	false - if the parameter is NULL or stateId<0
* 	true - if the stateId is found in the judge results
* 	false other wise
*/
bool judgeVotedToState(Judge judge, int stateId);

/**
* judgeMapKeyCompare: compare two judges ids
*
* @param judgeId1 - the first judge id to compare
* @param judgeId2 - the second judge id to compare
* @return
* 		A positive integer if the first element is greater;
* 		0 if they're equal;
*		A negative integer if the second element is greater.
*/
int judgeMapKeyCompare(MapKeyElement judgeId1, MapKeyElement judgeId2);

/**
* judgeMapDataElementCopy: Allocate a copy of judge for a map container
*
* @param judge - the judge to copy
* @return
* 		MapDataElement that is a copy of the judge send as a parameter
*/
MapDataElement judgeMapDataElementCopy(MapDataElement judge);

/**
* judgeMapKeyElementCopy: Allocate a copy of judge id for a map container
*
* @param judgeId - the judge id to copy
* @return
* 		MapKeyElement that is a copy of the judge id send as a parameter
*/
MapKeyElement judgeMapKeyElementCopy(MapKeyElement judgeId);

/**
* judgeMapDataElementFree: Function to deallocate a judge for a map container
*
* @param judge - the judge to deallocate
*/
void judgeMapDataElementFree(MapDataElement judge);

/**
* judgeMapKeyElementFree: Function to deallocate a judge id for a map container
*
* @param judgeId - the judge to deallocate
*/
void judgeMapKeyElementFree(MapKeyElement judgeId);

#endif
