#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "eurovision.h"
#include "list.h"
#include "state.h"
#include "judge.h"

#define STATE_ID 0
#define AUDIENCE_SCORE 1
#define JUDGES_SCORE 2
#define NUM_OF_PARAMETERS 3


/** check if str contain only lower case letters and spaces return true
 * if it does and false other wise
 */
bool checkName(const char* str);

/** check each int in ids array and search for a matching state
 * with the same id in the states map container
 */
int checkIfStatesExists(Map states, int *ids);

/** check if their is two identical ints in ids arr */
bool ContainSameState(int *ids);

/** copy function for string for list element */
ListElement stringListCopy (ListElement str);

/** delete function for string for list element */
void stringListFree(ListElement str);

/** compare function for string for list element */
int stringListCompare(ListElement str1, ListElement str2);

/** copy function for state for list element */
ListElement stateListCopy(ListElement state);

/** delete function for state for list element */
void stateListFree(ListElement state);

/** compare function for state for list element
 * compare the states final score and if the score is the same
 * compare between the states ids*/
int stateListCompareFinalScore(ListElement state1, ListElement state2);

/** compare function for state for list element
 * compare the states name
 */
int stateListCompareName(ListElement state1, ListElement state2);

/** return the index of the max number in arr */
int findMaxIndex(double* arr, int size);

/** return the index of the toSearch number in arr */
int findIndex(double* arr, int size, double toSearch);

/** enter eurovision valid scores to arrToFeed in the feedIndex
 * from results arr.
 * results contain state ids, the state with the highest score is at
 * index 0 and so on.*/
void feedScoreTo(int feedIndex, double **arrToFeed, int arrSize,
                 int *results, int resultsSize);
/** calculate the audience scores and feed them into the scores arr */
EurovisionResult calculateAudienceScore(Map states, double **scores);

/** return the id of the most voted state of this spesific state */
int findFavoriteStateId(State state);

/** check if two sorted Lists of states is identical */
bool statesSortedListIdentical(List statesList1, List statesList2);

/** check if the states lists arr contain a spesific states list */
bool arrContainStatesList(List *friendlyStatesArr, int size,
                          List friendlyStates);
/** convert states list arr to string list containing the states names */
List convertStatesArrToStrList(List *friendlyStatesArr, int size);

/** return an arr that contain states id from the most voted (index 0)
 * state to the least voted */
int* transformAudienceScoreToArr(Map citizenVotes);

struct eurovision_t{
    Map judges;
    Map states;
};

Eurovision eurovisionCreate(){
    Eurovision newEurovision = malloc(sizeof(*newEurovision));
    if(!newEurovision) return NULL;
    newEurovision->judges = mapCreate(judgeMapDataElementCopy,
                                      judgeMapKeyElementCopy,
                                      judgeMapDataElementFree,
                                      judgeMapKeyElementFree,
                                      judgeMapKeyCompare);
    if(!newEurovision->judges){
        eurovisionDestroy(newEurovision);
        return NULL;
    }
    newEurovision->states = mapCreate(stateMapDataElementCopy,
                                      stateMapKeyElementCopy,
                                      stateMapDataElementFree,
                                      stateMapKeyElementFree,
                                      stateMapKeyCompare);
    if(!newEurovision->states) {
        eurovisionDestroy(newEurovision);
        return NULL;
    }
    return newEurovision;
}

void eurovisionDestroy(Eurovision eurovision){
    if(!eurovision) return;
    if(eurovision->judges){
        mapDestroy(eurovision->judges);
    }
    if(eurovision->states){
        mapDestroy(eurovision->states);
    }
    free(eurovision);
}

bool checkName(const char* str){
    if(!str) return false;
    while(*str){
        if(*str != ' '){
            if(*str>'z'||*str<'a'){
                return false;
            }
        }
        str++;
    }
    return true;
}

int checkIfStatesExists(Map states, int *ids){
    for(int i=0;i<NUM_OF_JUDGE_RESULTS;i++){
        if(ids[i]<0) return -1;
        if(!mapContains(states, &ids[i])){
            return 0;
        }
    }
    return 1;
}

bool ContainSameState(int *ids){
    if(!ids) return true;
    for(int i=0; i<NUM_OF_JUDGE_RESULTS; i++){
        int current = ids[i];
        for(int j=1;(j+i)<NUM_OF_JUDGE_RESULTS; j++){
            if(current==ids[i+j]) return true;
        }
    }
    return false;
}

EurovisionResult eurovisionAddState(Eurovision eurovision, int stateId,
                                    const char *stateName,
                                    const char *songName)
                                    {
    if (!eurovision || !stateName || !songName) {
                                            return EUROVISION_NULL_ARGUMENT;
                                        }
    if (stateId < 0) return EUROVISION_INVALID_ID;
    if (!checkName(stateName) || !checkName(songName)) {
        return EUROVISION_INVALID_NAME;
    }
    if (mapContains(eurovision->states, &stateId)) {
        return EUROVISION_STATE_ALREADY_EXIST;
    }
    State newState = stateCreate(stateId, stateName, songName);
    if(!newState){
       eurovisionDestroy(eurovision);
       return EUROVISION_OUT_OF_MEMORY;
    }
    if(mapPut(eurovision->states, &stateId, newState) != MAP_SUCCESS){
            eurovisionDestroy(eurovision);
            return EUROVISION_OUT_OF_MEMORY;
    }
    stateDestroy(newState);
    return EUROVISION_SUCCESS;
}

EurovisionResult eurovisionRemoveState(Eurovision eurovision, int stateId){
    if(!eurovision) return EUROVISION_NULL_ARGUMENT;
    if(stateId<0) return EUROVISION_INVALID_ID;
    if(!mapContains(eurovision->states, &stateId)) {
        return EUROVISION_STATE_NOT_EXIST;
    }
    int numOfJudges = mapGetSize(eurovision->judges);
    if(numOfJudges>0) {
        int* judgesToRemove = malloc(sizeof(int)*numOfJudges);
        if(!judgesToRemove){
            eurovisionDestroy(eurovision);
            return EUROVISION_OUT_OF_MEMORY;
        }
        int i=-1;
        MAP_FOREACH(int*, judgeIdIter, eurovision->judges){
            Judge tmpJudge = (Judge)mapGet(eurovision->judges, judgeIdIter);
            if (judgeVotedToState(tmpJudge, stateId)) {
                i++;
                judgesToRemove[i] = judgeGetId(tmpJudge);
            }
        }
        for(; i>-1; i--){
            eurovisionRemoveJudge(eurovision, judgesToRemove[i]);
        }
        free(judgesToRemove);
    }
    MAP_FOREACH(int*, stateIdIter, eurovision->states){
        State tmpState = mapGet(eurovision->states, stateIdIter);
        stateDeleteAllVotesOfSpecificState(tmpState, stateId);
    }
    mapRemove(eurovision->states, &stateId);
    return EUROVISION_SUCCESS;
}

EurovisionResult eurovisionAddJudge(Eurovision eurovision, int judgeId,
                                    const char *judgeName,
                                    int *judgeResults){
    if(!eurovision || !judgeName || !judgeResults) {
        return EUROVISION_NULL_ARGUMENT;
    }
    if(judgeId < 0) return EUROVISION_INVALID_ID;
    int resultsCheck = checkIfStatesExists(eurovision->states, judgeResults);
    if(resultsCheck == -1){
        return EUROVISION_INVALID_ID;
    }
    if(ContainSameState(judgeResults)){
        return EUROVISION_INVALID_ID;
    }
    if(!checkName(judgeName)) {
        return EUROVISION_INVALID_NAME;
    }
    if(resultsCheck == 0){
        return EUROVISION_STATE_NOT_EXIST;
    }
    if(mapContains(eurovision->judges, &judgeId)) {
        return EUROVISION_JUDGE_ALREADY_EXIST;
    }
    Judge newJudge = judgeCreate(judgeId, judgeName, judgeResults);
    if(mapPut(eurovision->judges, &judgeId, newJudge)== MAP_OUT_OF_MEMORY){
        eurovisionDestroy(eurovision);
        return EUROVISION_OUT_OF_MEMORY;
    }
    judgeDestroy(newJudge);
    return EUROVISION_SUCCESS;
}

EurovisionResult eurovisionRemoveJudge(Eurovision eurovision, int judgeId){
    if(!eurovision) return EUROVISION_NULL_ARGUMENT;
    if(judgeId < 0) return EUROVISION_INVALID_ID;
    if(!mapContains(eurovision->judges, &judgeId)){
        return EUROVISION_JUDGE_NOT_EXIST;
    }
    mapRemove(eurovision->judges, &judgeId);
    return EUROVISION_SUCCESS;
}

EurovisionResult eurovisionAddVote(Eurovision eurovision, int stateGiver,
                                   int stateTaker){
    if(!eurovision) return EUROVISION_NULL_ARGUMENT;
    if(stateGiver<0||stateTaker<0){
        return EUROVISION_INVALID_ID;
    }
    if(!mapContains(eurovision->states, &stateGiver)){
        return EUROVISION_STATE_NOT_EXIST;
    }
    if(!mapContains(eurovision->states, &stateTaker)){
        return EUROVISION_STATE_NOT_EXIST;
    }
    if(stateGiver==stateTaker) return EUROVISION_SAME_STATE;
    StateResult result;
    result = stateAddVote(mapGet(eurovision->states, &stateGiver), stateTaker);
    if(result==STATE_OUT_OF_MEMORY){
        eurovisionDestroy(eurovision);
        return EUROVISION_OUT_OF_MEMORY;
    }
    return EUROVISION_SUCCESS;
}

EurovisionResult eurovisionRemoveVote(Eurovision eurovision, int stateGiver,
                                      int stateTaker){
    if(!eurovision) return EUROVISION_NULL_ARGUMENT;
    if(stateGiver<0||stateTaker<0){
        return EUROVISION_INVALID_ID;
    }
    if(!mapContains(eurovision->states, &stateGiver)){
        return EUROVISION_STATE_NOT_EXIST;
    }
    if(!mapContains(eurovision->states, &stateTaker)){
        return EUROVISION_STATE_NOT_EXIST;
    }
    if(stateGiver==stateTaker) return EUROVISION_SAME_STATE;
    StateResult result;
    result = stateDeleteVote(mapGet(eurovision->states,
                             &stateGiver), stateTaker);
    if(result==STATE_OUT_OF_MEMORY){
        eurovisionDestroy(eurovision);
        return EUROVISION_OUT_OF_MEMORY;
    }
    return EUROVISION_SUCCESS;
}

ListElement stringListCopy (ListElement str){
    char* newStr = malloc(sizeof(char)*strlen(str)+1);
    if(!newStr) return NULL;
    strcpy(newStr, str);
    return newStr;
}

void stringListFree(ListElement str){
    free(str);
}

int stringListCompare(ListElement str1, ListElement str2){
    return strcmp(str1,str2);
}

ListElement stateListCopy(ListElement state){
    if(!state) return NULL;
    State newState = stateCopy((State)state);
    assert(newState);
    return newState;
}

void stateListFree(ListElement state){
    if(!state) return;
    stateDestroy((State)state);
}

int stateListCompareFinalScore(ListElement state1, ListElement state2){
    double score1 = stateGetFinalScore((State)state1);
    double score2 = stateGetFinalScore((State)state2);
    if(score2==score1) {
        int state1Id = stateGetId((State)state1);
        int state2Id = stateGetId((State)state2);
        return state1Id-state2Id;
    }
    if(score1>score2) return -1;
    return 1;
}

int stateListCompareName(ListElement state1, ListElement state2){
    char* stateName1 = stateGetName((State)state1);
    char* stateName2 = stateGetName((State)state2);
    return strcmp(stateName1, stateName2);
}

int findMaxIndex(double* arr, int size){
    if(!arr) return -1;
    double max = *arr;
    int maxIndex = 0;
    for(int i=1; i<size; i++){
        if(*(arr+i)>max){
            maxIndex = i;
            max = *(arr+i);
        }
    }
    if(max <=0) return -1;
    return maxIndex;
}

int findIndex(double* arr, int size, double toSearch){
    for(int i=0; i<size; i++){
        if(*(arr+i) == toSearch) return i;
    }
    return -1;
}

void feedScoreTo(int feedIndex, double **arrToFeed, int arrSize,
                 int *results, int resultsSize){
    if(!arrToFeed || !results || resultsSize <= 0) {
        return;
    }
    int score =12;
    for(int i=0; i<resultsSize; i++){
        int currentResultsId = *(results+i);
        if(currentResultsId<0) break;
        int currentStateId = *(results+i);
        int index = findIndex(arrToFeed[STATE_ID], arrSize ,currentStateId);
        *(arrToFeed[feedIndex]+index)+=score;
        if(score == 0) continue;
        if(score>8){
            score -=2;
        }
        else{
            score--;
        }
    }
}

int* transformAudienceScoreToArr(Map citizenVotes){
    if(!citizenVotes) return NULL;
    int numOfStates = mapGetSize(citizenVotes);
    if(numOfStates<=0) return NULL;
    double* audienceScore[2];
    for(int i=0; i<2; i++) {
        audienceScore[i] = malloc(sizeof(double)*numOfStates);
        if (!audienceScore[i]) {
            for (; i >= 0; i--) {
                free(audienceScore[i]);
            }
            return NULL;
        }
    }
    int *results = malloc(sizeof(int)*numOfStates);
    if(!results) {
        for (int i=0; i<2; i++) {
            free(audienceScore[i]);
        }
        return NULL;
    }
    int maxIndex, maxStateId;
    for(int i=0; i<numOfStates;i++){
        results[i] = 0;
    }
    int i=0;
    MAP_FOREACH(int* ,stateIdIter, citizenVotes) {
        double stateId = (double)*stateIdIter;
        int num = *((int*)mapGet(citizenVotes, stateIdIter));
        *(audienceScore[0] + i) = stateId;
        *(audienceScore[1] + i) = num;
        i++;
    }
    for(i=0; i<numOfStates; i++){
        maxIndex = findMaxIndex(audienceScore[1], numOfStates);
        if(maxIndex == -1) break;
        maxStateId = *(audienceScore[0]+maxIndex);
        results[i] = maxStateId;
        *(audienceScore[1]+ maxIndex) = 0;
    }
    for (i=0; i<2; i++) {
        free(audienceScore[i]);
    }
    return results;
}

EurovisionResult calculateAudienceScore(Map states, double **scores){
    if(!states||!scores) {
        return EUROVISION_NULL_ARGUMENT;
    }
    int numOfStates = mapGetSize(states);
    int i = 0;
    MAP_FOREACH(int*, stateIdIter, states){
        State tmpState = (State) mapGet(states, stateIdIter);
        stateSetScore(tmpState, 0);
        int stateId =  *stateIdIter;
        *(scores[STATE_ID]+i) = stateId;
        i++;
    }

    MAP_FOREACH(int*, stateIdIter, states) {
        State tmpState = (State) mapGet(states, stateIdIter);
        Map citizensVotes = stateGetCitizenVotes(tmpState);
        int *results = transformAudienceScoreToArr(citizensVotes);
        if (results) {
            int resultsSize = mapGetSize(citizensVotes);
            feedScoreTo(AUDIENCE_SCORE, scores,
                        numOfStates, results, resultsSize);
            free(results);
        }
    }
    return EUROVISION_SUCCESS;
}

EurovisionResult initiateScoreArr(double *scores[NUM_OF_PARAMETERS],
                                  int numOfStates){
    for(int i = 0; i<NUM_OF_PARAMETERS; i++){
        scores[i] = malloc(sizeof(double)*numOfStates);
        if(!scores[i]) {
            for(; i>=0; i--){
                free(scores[i]);
            }
            return EUROVISION_OUT_OF_MEMORY;
        }
    }
    for(int i=0; i<numOfStates; i++){
        *(scores[AUDIENCE_SCORE]+i) = 0;
        *(scores[JUDGES_SCORE]+i) = 0;
    }
    return EUROVISION_SUCCESS;
}

List eurovisionRunContest(Eurovision eurovision, int audiencePercent){
    if(audiencePercent<1||audiencePercent>100){
        return NULL;
    }
    List resultList = listCreate(stringListCopy, stringListFree);
    if(!resultList){
        eurovisionDestroy(eurovision);
        return NULL;
    }
    int numOfJudges = mapGetSize(eurovision->judges);
    int numOfStates = mapGetSize(eurovision->states);
    if(numOfStates<=0) return resultList;
    double *scores[NUM_OF_PARAMETERS] = {0};
    List statesList = listCreate(stateListCopy, stateListFree);
    if(!statesList){
        listDestroy(resultList);
        listDestroy(statesList);
        eurovisionDestroy(eurovision);
        return NULL;
    }
    if(initiateScoreArr(scores,numOfStates)!=EUROVISION_SUCCESS) {
        listDestroy(resultList);
        listDestroy(statesList);
        eurovisionDestroy(eurovision);
        return NULL;
    }
    calculateAudienceScore(eurovision->states, scores);
    MAP_FOREACH(int*, stateIdIter, eurovision->judges){
        Judge tmpJudge = (Judge)mapGet(eurovision->judges, stateIdIter);
        int* tmpJudgeResults = judgeGetResults(tmpJudge);
        feedScoreTo(JUDGES_SCORE,scores, numOfStates,
                    tmpJudgeResults, NUM_OF_JUDGE_RESULTS);
    }
    for(int i=0; i<numOfStates; i++){
        int stateId = *(scores[STATE_ID]+i);
        State tmpState = (State)mapGet(eurovision->states, &stateId);
        double audienceP = (double)audiencePercent/100;
        double audienceScore = *(scores[AUDIENCE_SCORE]+i);
        audienceScore = (numOfStates>0)?
                audienceScore/numOfStates : audienceScore;
        double judgesScore = *(scores[JUDGES_SCORE]+i);
        judgesScore = (numOfJudges>0)?
                        judgesScore/numOfJudges : judgesScore;
        double totalScore = (audienceScore*audienceP)+
                            (judgesScore*(1-audienceP));
        if(totalScore<10e-20) {
            totalScore = 0;
        }
        stateSetScore(tmpState, totalScore);
        listInsertLast(statesList, tmpState);
    }
    listSort(statesList, stateListCompareFinalScore);
    LIST_FOREACH(State, tmpState, statesList){
        listInsertLast(resultList, stateGetName(tmpState));
    }
    for(int i=0; i<NUM_OF_PARAMETERS; i++){
        free(scores[i]);
    }
    listDestroy(statesList);
    return resultList;
}

List eurovisionRunAudienceFavorite(Eurovision eurovision){
    if(!eurovision) return NULL;
    List resultList = listCreate(stringListCopy, stringListFree);
    if(!resultList){
        listDestroy(resultList);
        eurovisionDestroy(eurovision);
        return NULL;
    }
    int numOfStates = mapGetSize(eurovision->states);
    if(numOfStates<=0) return resultList;
    double* scores[2] ={0};
    int i;
    List statesList = listCreate(stateListCopy, stateListFree);
    if(!statesList){
        listDestroy(resultList);
        listDestroy(statesList);
        eurovisionDestroy(eurovision);
        return NULL;
    }
    for(i = 0; i<2; i++) {
        scores[i] = malloc(sizeof(double) * numOfStates);
        if (!scores[i]) {

            for(;i<=0 ;i--){
                free(scores[i]);
            }
            listDestroy(statesList);
            listDestroy(resultList);
            eurovisionDestroy(eurovision);
            return NULL;
        }
    }
    for(i=0; i<numOfStates; i++){
        *(scores[AUDIENCE_SCORE]+i) = 0;
    }
    calculateAudienceScore(eurovision->states, scores);
    for(i=0; i<numOfStates; i++){
        int stateId = *(scores[STATE_ID]+i);
        State tmpState = (State)mapGet(eurovision->states, &stateId);
        double audienceScore = *(scores[AUDIENCE_SCORE]+i)/numOfStates;
        if(audienceScore<10e-20) {
            audienceScore = 0;
        }
        stateSetScore(tmpState, audienceScore);
        listInsertFirst(statesList, tmpState);
    }
    listSort(statesList, stateListCompareFinalScore);
    LIST_FOREACH(State, tmpState, statesList){
        char *stateName = stateGetName(tmpState);
        listInsertLast(resultList, stateName);
    }
    for(i = 0; i<2; i++){
        free(scores[i]);
    }
    listDestroy(statesList);
    return resultList;
}

int findFavoriteStateId(State state){
    if(!state) return -1;
    Map citizensVotes = stateGetCitizenVotes(state);
    if(mapGetSize(citizensVotes)<=0) return -1;
    int *favoriteStates = transformAudienceScoreToArr(citizensVotes);
    int favoriteStateId = *favoriteStates;
    free(favoriteStates);
    return favoriteStateId;
}

bool statesSortedListIdentical(List statesList1, List statesList2){
    if(!statesList1) return false;
    if(!statesList2) return false;
    int stateId1 = stateGetId((State)listGetFirst(statesList1));
    int stateId2 = stateGetId((State)listGetFirst(statesList2));
    if(stateId1==stateId2) return true;
    return false;
}

bool arrContainStatesList(List *friendlyStatesArr, int size,
                          List friendlyStates){
    if(!friendlyStatesArr) return false;
    if(!friendlyStates) return false;
    for(int i=0; i<size; i++){
        List current = *(friendlyStatesArr+i);
        if(!current) return false;
        if(statesSortedListIdentical(current, friendlyStates)){
            return true;
        }
    }
    return false;
}

List convertStatesArrToStrList(List *friendlyStatesArr, int size){
    if(!friendlyStatesArr) {
        return NULL;
    }
    List friendlyStatesNames = listCreate(stringListCopy, stringListFree);
    if(!friendlyStatesNames) {

        return NULL;
    }
    if(size==0) return friendlyStatesNames;
    char *spaceStr = " - ";
    for(int i=0; i<size; i++){
        List currentPair = *(friendlyStatesArr+i);
        State state1 = listGetFirst(currentPair);
        if(!state1){
            listDestroy(friendlyStatesNames);
            return NULL;
        }
        State state2 = listGetNext(currentPair);
        if(!state2) {
            listDestroy(friendlyStatesNames);
            return NULL;
        }
        char *stateName1 = stateGetName(state1);
        char *stateName2 = stateGetName(state2);
        int strSize = (int)strlen(stateName1)+
                      (int)strlen(stateName2)+
                      (int)strlen(spaceStr)
                      +1;
        char *resultStr = malloc(sizeof(char)*strSize);
        if(!resultStr) {
            free(resultStr);
            listDestroy(friendlyStatesNames);
            return NULL;
        }
        *resultStr = '\0';
        strcat(resultStr, stateName1);
        strcat(resultStr, spaceStr);
        strcat(resultStr, stateName2);
        listInsertFirst(friendlyStatesNames, resultStr);
        free(resultStr);
    }
    return friendlyStatesNames;
}

List eurovisionRunGetFriendlyStates(Eurovision eurovision){
    if(!eurovision) return NULL;
    int numOfStates = mapGetSize(eurovision->states);
    int arrSize = 0;
    List resultList = listCreate(stringListCopy, stringListFree);
    if(!resultList){
        listDestroy(resultList);
        eurovisionDestroy(eurovision);
        return NULL;
    }
    if(numOfStates==0) return resultList;
    listDestroy(resultList);
    List *friendlyStatesArr = malloc(sizeof(List)*((numOfStates/2)+1));
    if(!friendlyStatesArr) {
        free(friendlyStatesArr);
        listDestroy(resultList);
        eurovisionDestroy(eurovision);
        return NULL;
    }
    int i=0;
    MAP_FOREACH(int*, stateIdIter, eurovision->states){
        List friendlyStates = listCreate(stateListCopy, stateListFree);
        if(!friendlyStates){
            listDestroy(friendlyStates);
            free(friendlyStatesArr);
            listDestroy(resultList);
            eurovisionDestroy(eurovision);
            return NULL;
        }
        State state1 = mapGet(eurovision->states, stateIdIter);
        int favoriteStateId = findFavoriteStateId(state1);
        State state2 = mapGet(eurovision->states, &favoriteStateId);
        if(findFavoriteStateId(state2)==*stateIdIter){
            listInsertFirst(friendlyStates, state1);
            listInsertFirst(friendlyStates, state2);
            listSort(friendlyStates, stateListCompareName);
            if(!arrContainStatesList(friendlyStatesArr, arrSize,
                                     friendlyStates)){
                *(friendlyStatesArr+i) = listCopy(friendlyStates);
                i++;
                arrSize++;
            }
        }
        listDestroy(friendlyStates);
    }
    resultList = convertStatesArrToStrList(friendlyStatesArr, arrSize);
    if(resultList == NULL){
        free(friendlyStatesArr);
        listDestroy(resultList);
        eurovisionDestroy(eurovision);
        return NULL;
    }
    listSort(resultList, stringListCompare);
    for(i=0; i<arrSize; i++){
        listDestroy(*(friendlyStatesArr+i));
    }
    free(friendlyStatesArr);
    return resultList;
}
