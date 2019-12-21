#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "set.h"
#include "judge.h"



struct Judge_t{
    int id;
    char* name;
    int results[NUM_OF_JUDGE_RESULTS];
};

char* judgeGetName(Judge judge){
    if(judge) return judge->name;
    return NULL;
}

int judgeGetId(Judge judge){
    if(judge) return judge->id;
    return -1;
}

int* judgeGetResults(Judge judge){
    if(!judge) return NULL;
    return judge->results;
}

Judge judgeCreate(int judgeId, const char* judgeName, int judgeResults[NUM_OF_JUDGE_RESULTS]){
    if(judgeId<0) return NULL;
    if(!judgeName||!judgeResults) return NULL;
    Judge newJudge = malloc(sizeof(*newJudge));
    if(!newJudge) return NULL;
    newJudge ->id = judgeId;
    newJudge->name = malloc((sizeof(char)*strlen(judgeName))+1);
    if(!newJudge->name) {
        judgeDestroy(newJudge);
        return NULL;
    }
    strcpy(newJudge->name,judgeName);
    memcpy(newJudge->results,judgeResults,sizeof(int)*NUM_OF_JUDGE_RESULTS);
    return newJudge;
}

Judge judgeCopy(Judge judge){
    if(!judge) return NULL;
    Judge newJudge = judgeCreate(judge->id, judge->name, judge->results);
    if(!newJudge) return NULL;
    return newJudge;
}

void judgeDestroy(Judge judge){
    if(!judge) return;
    free(judge->name);
    free(judge);
}

bool judgeVotedToState(Judge judge, int stateId){
    if(!judge||stateId<0){
        return false;
    }
    for(int i=0;i<NUM_OF_JUDGE_RESULTS;i++){
        if(judge->results[i]==stateId){
            return true;
        }
    }
    return false;
}

int judgeMapKeyCompare(MapKeyElement judgeId1, MapKeyElement judgeId2){
    if(!judgeId1||!judgeId2){
        fprintf(stderr,"Null argument in judgeMapKeyCompare, return 0");
        return 0;
    }
    return (*(int *) judgeId1 - *(int *) judgeId2);
}

MapDataElement judgeMapDataElementCopy(MapDataElement judge){
    return judgeCopy((Judge)judge);
}
MapKeyElement judgeMapKeyElementCopy(MapKeyElement judgeId) {
    if (!judgeId) {
        return NULL;
    }
    int *copy = malloc(sizeof(*copy));
    if (!copy) {
        return NULL;
    }
    *copy = *(int *) judgeId;
    return copy;
}
void judgeMapDataElementFree(MapDataElement judge){
    judgeDestroy((Judge)judge);
}

void judgeMapKeyElementFree(MapKeyElement judgeId){
    free(judgeId);
}
