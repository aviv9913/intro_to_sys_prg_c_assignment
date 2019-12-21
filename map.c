#include "map.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/** Type for defining a linked list map elements */
typedef struct Element_t {
    MapDataElement data;
    MapKeyElement key;
    struct Element_t* next;
}*Element;

/** Allocate a new map element according to the parameters */
Element ElementCreate(Map map,MapKeyElement key, MapDataElement data);

/** Allocate a copy of the map element send as parameter */
Element ElementCopy(Map map, Element e);

/** Deallocate the map element send as parameter */
void ElementDestroy(Map map,Element e);

/** Deallocate all the map element in the map */
void DestroyAllElements(Map map);

/** Recursion function to delete all the map element in the map */
void DestroyAllElementsReq(Map map,Element e);

/** Put the elementToAppend next to the map element send as parameter */
bool AppendElement(Element element, Element elementToAppend);

/** Find the map element with the specific key and return it  */
Element FindElementInMap(Map map, MapKeyElement keyElement);


struct Map_t{
    Element head;
    Element current;
    int num_of_elements;
    copyMapDataElements CopyData;
    copyMapKeyElements CopyKey;
    freeMapDataElements FreeData;
    freeMapKeyElements FreeKey;
    compareMapKeyElements CompareKeys;
};

Element ElementCreate(Map map,MapKeyElement key, MapDataElement data){
    if(!data||!key||!map){
        return NULL;
    }
    Element e = malloc(sizeof(*e));
    if(!e) return NULL;
    e->data = map->CopyData(data);
    e->key = map->CopyKey(key);
    e->next = NULL;
    return e;
}

Element ElementCopy(Map map, Element srcElement){
    if(!map||!srcElement){
        return NULL;
    }
    Element newElement = ElementCreate(map, srcElement->key, srcElement->data);
    if(!newElement){
        return NULL;
    }
    return newElement;
}

void ElementDestroy(Map map,Element e){
    if(!e||!map){
        return;
    }
    map->FreeData(e->data);
    map->FreeKey(e->key);
    free(e);
}


void DestroyAllElements(Map map){
    if(!map){
        return;
    }
    DestroyAllElementsReq(map,map->head);
    map->current = NULL;
    map->head = NULL;
}

void DestroyAllElementsReq(Map map,Element e){
    if(!map||!e){
    return;
    }
    DestroyAllElementsReq(map,e->next);
    ElementDestroy(map,e);
}

bool AppendElement(Element element, Element elementToAppend){
    if(!element||!elementToAppend) {
        return false;
    }
    if(!element->next){
        element->next = elementToAppend;
        return true;
    }
    else {
        Element tmp = element->next;
        element->next = elementToAppend;
        elementToAppend ->next = tmp;
        return true;
    }
}

Map mapCreate(copyMapDataElements copyDataElement,
              copyMapKeyElements copyKeyElement,
              freeMapDataElements freeDataElement,
              freeMapKeyElements freeKeyElement,
              compareMapKeyElements compareKeyElements){
    if(!copyDataElement||!copyKeyElement||!freeDataElement||!freeKeyElement
        ||!compareKeyElements){
        return NULL;
    }
    Map map = malloc(sizeof(*map));
    if(!map)return NULL;
    map->CopyData = copyDataElement;
    map->CopyKey = copyKeyElement;
    map->FreeData = freeDataElement;
    map->FreeKey = freeKeyElement;
    map->CompareKeys = compareKeyElements;
    map->num_of_elements = 0;
    map->head = NULL;
    map->current = NULL;
    return map;
}

void mapDestroy(Map map){
    if(!map) return;
    DestroyAllElements(map);
    free(map);
}

int mapGetSize(Map map){
    if(!map) return -1;
    int result = map->num_of_elements;
    return result;
}

MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement){
    if(!map||!keyElement||!dataElement){
        return MAP_NULL_ARGUMENT;
    }
    Element newElement = ElementCreate(map, keyElement , dataElement);
    if(!newElement) return MAP_OUT_OF_MEMORY;
    if(!map->head){
        map->head = newElement;
        map->num_of_elements++;
        return MAP_SUCCESS;
    }
    Element tmpElement = map->head;
    int compareResult = map->CompareKeys(newElement->key,tmpElement->key);
    if(compareResult<=0){
        if(!AppendElement(newElement,tmpElement)) return MAP_OUT_OF_MEMORY;
            map->head = newElement;
            map->num_of_elements++;
        if(compareResult==0){
            newElement->next = tmpElement->next;
            ElementDestroy(map,tmpElement);
            map->num_of_elements--;
        }
        map->current = NULL;
        return MAP_SUCCESS;
    }
    else {
        while (tmpElement->next) {
            compareResult = map->CompareKeys(newElement->key,
                                             tmpElement->next->key);
            if (compareResult > 0) {
                tmpElement = tmpElement->next;
                continue;
            }
            else if (compareResult < 0) {
                break;
            }
            else if(compareResult == 0){
                bool result = AppendElement(tmpElement,newElement);
                if(!result) return MAP_OUT_OF_MEMORY;
                Element elementToDestroy = newElement->next;
                newElement->next = newElement->next->next;
                ElementDestroy(map,elementToDestroy);
                map->current = NULL;
                return MAP_SUCCESS;
            }
        }
        AppendElement(tmpElement, newElement);
        map->num_of_elements++;
        map->current = NULL;
        return MAP_SUCCESS;
    }
}

Element FindElementInMap(Map map, MapKeyElement keyElement){
    if(!map||!keyElement){
        return NULL;
    }
    Element tmpElement = map->head;
    int compareResult;
    while(tmpElement){
        compareResult = map->CompareKeys(keyElement,tmpElement->key);
        if(compareResult==0){
            return tmpElement;
        }
        else if(compareResult<0){
            break;
        }
        else if(compareResult>0){
            tmpElement = tmpElement->next;
        }
    }
    return NULL;
}

bool mapContains(Map map, MapKeyElement element){
    if(!map||!element){
        return false;
    }
    if(FindElementInMap(map,element)){
        return true;
    }
    else {
        return false;
    }
}

Map mapCopy(Map map){
    if(!map) return NULL;
    Map newMap = mapCreate(map->CopyData, map->CopyKey,
                           map->FreeData, map->FreeKey, map->CompareKeys);
    if(!newMap) return NULL;
    if(!map->head) return newMap;
    Element srcElement = map->head;
    Element tmpElement;
    Element destElement = ElementCopy(map, srcElement);
    if(!destElement) return NULL;
    newMap->head = destElement;
    newMap->num_of_elements++;
    srcElement = srcElement->next;
    while (srcElement) {
        tmpElement = ElementCopy(map, srcElement);
        if(!tmpElement) return NULL;
        destElement->next = tmpElement;
        newMap->num_of_elements++;
        destElement = destElement->next;
        srcElement = srcElement->next;
    }
    if(map->current){
        newMap->current = FindElementInMap(newMap,map->current->key);
    }
    return newMap;
}

MapDataElement mapGet(Map map, MapKeyElement keyElement){
    if(!map||!keyElement) {
        return NULL;
    }
    Element tmpElement = FindElementInMap(map,keyElement);
    if(!tmpElement){
        return NULL;
    }
    else{
        return tmpElement->data;
    }
}

MapResult mapRemove(Map map, MapKeyElement keyElement){
    if(!map||!keyElement) {
        return MAP_NULL_ARGUMENT;
    }
    if(!map->head) return MAP_ITEM_DOES_NOT_EXIST;
    Element tmpElement = map->head;
    int compareResult = map->CompareKeys(tmpElement->key,keyElement);
    if(compareResult==0){
        Element elementToDestroy = tmpElement;
        map->head = map->head->next;
        map->num_of_elements--;
        ElementDestroy(map,elementToDestroy);
        map->current = NULL;
        return MAP_SUCCESS;
    }
    while(tmpElement->next){
        compareResult = map->CompareKeys(tmpElement->next->key,keyElement);
        if(compareResult>0){
            return MAP_ITEM_DOES_NOT_EXIST;
        }
        else if(compareResult==0){
            Element elementToDestroy = tmpElement->next;
            tmpElement->next = elementToDestroy->next;
            map->num_of_elements--;
            ElementDestroy(map,elementToDestroy);
            map->current = NULL;
            return MAP_SUCCESS;
        }
        else{
            tmpElement = tmpElement->next;
        }
    }
    return MAP_ITEM_DOES_NOT_EXIST;
}

MapKeyElement mapGetFirst(Map map){
    if(!map) return NULL;
    if(!map->head) return NULL;
    map->current = map->head;
    return map->head->key;
}

MapKeyElement mapGetNext(Map map){
    if(!map) return NULL;
    if(!map->current) return NULL;
    if(!map->current->next) return NULL;
    map->current = map->current->next;
    return map->current->key;
}

MapResult mapClear(Map map){
    if(!map) return MAP_NULL_ARGUMENT;
    DestroyAllElements(map);
    map->num_of_elements = 0;
    map->current = NULL;
    return MAP_SUCCESS;
}
