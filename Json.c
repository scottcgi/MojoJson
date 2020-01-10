/*
 * Copyright (c) scott.cgi All Rights Reserved.
 * 
 * This source code belongs to project MojoJson, which is hosted on GitHub, and licensed under the MIT License.
 *
 * License: https://github.com/scottcgi/MojoJson/blob/master/LICENSE
 * GitHub : https://github.com/scottcgi/MojoJson
 *
 * Since  : 2013-5-29
 * Author : scott.cgi
 * Version: 1.2.0
 * Update : 2020-1-8
 */


#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "Json.h"

#define ALog_A(e, ...) e ? (void) 0 : printf(__VA_ARGS__), printf("\n"),  assert(e);
#define ALog_D(...)    printf(__VA_ARGS__)


// ArrayList tool for JsonArray
//----------------------------------------------------------------------------------------------------------------------


/**
 * The list can dynamically increase memory capacity .
 */
typedef struct
{
    /**
     * Increase memory space when needed, default 10.
     */
    int   increase;

    /**
     * The sizeof element type.
     */
    int   elementTypeSize;

    /**
     * Elements count.
     */
    int   size;

    /**
     * Store memory data, the length is memory capacity.
     * if increase capacity, memory data will realloc.
     */
    struct 
    {
        /**
         * Elements memory space ptr.
         */
        void* data;

        /**
         * Elements count.
         */
        int   length;
    } 
    elementArr[1];
}
ArrayList;


static void ArrayListRelease(ArrayList* arrayList)
{
    free(arrayList->elementArr->data);
    arrayList->elementArr->data   = NULL;
    arrayList->elementArr->length = 0;
    arrayList->size               = 0;
}


static void ArrayListInit(int elementTypeSize, ArrayList* arrayList)
{
    arrayList->elementArr->data   = NULL;
    arrayList->elementArr->length = 0;
    arrayList->elementTypeSize    = elementTypeSize;
    arrayList->size               = 0;
    arrayList->increase           = 20;
}


static void ArrayListAddCapacity(ArrayList* arrayList, int increase)
{
    ALog_A(increase > 0, "Json ArrayListAddCapacity failed, increase = %d cannot <= 0", increase);

    void* data = realloc
                 (
                     arrayList->elementArr->data,
                     (size_t) (increase + arrayList->elementArr->length) * arrayList->elementTypeSize
                 );

    ALog_A
    (
        data != NULL,
        "Json ArrayListAddCapacity failed, unable to realloc memory, size = %d, length = %d, increase = %d",
        arrayList->size, arrayList->elementArr->length, increase
    );

    arrayList->elementArr->data    = data;
    arrayList->elementArr->length += increase;
}


static void* ArrayListAdd(ArrayList* arrayList, void* elementPtr)
{
    if (arrayList->size == arrayList->elementArr->length)
    {
        ArrayListAddCapacity(arrayList, arrayList->increase);
    }

    return memcpy
           (
               (char*) arrayList->elementArr->data + arrayList->elementTypeSize * (arrayList->size++),
               elementPtr,
               (size_t) arrayList->elementTypeSize
           );
}


static void* ArrayListInsert(ArrayList* arrayList, int index, void* elementPtr)
{
    if (arrayList->size == arrayList->elementArr->length)
    {
        ArrayListAddCapacity(arrayList, arrayList->increase);
    }

    void* from = (char*) arrayList->elementArr->data + arrayList->elementTypeSize * index;
    void* to   = (char*) from                        + arrayList->elementTypeSize;

    // from and to overlap so cannot use memcpy
    memmove(to, from, (size_t) arrayList->elementTypeSize * ((arrayList->size++) - index));

    return memcpy(from, elementPtr, (size_t) arrayList->elementTypeSize);
}


/**
 * Get the element with type.
 */
#define AArrayList_Get(arrayList, index, ElementType) \
    (((ElementType*) ((arrayList)->elementArr->data)))[index]


/**
 * Shortcut of ArrayListAdd.
 */
#define AArrayList_Add(arrayList, element) \
    ArrayListAdd(arrayList, &(element))

/**
 * Shortcut of AArrayList->Insert.
 */
#define AArrayList_Insert(arrayList, index, element) \
    ArrayListInsert(arrayList, index, &(element))


/**
 * Marked ArrayList element type.
 */
#define ArrayList(ElementType) ArrayList


// ArrayStrMap tool for JsonObject
//----------------------------------------------------------------------------------------------------------------------


/**
 * The actual element store in ArrayStrMap.
 */
typedef struct
{
    /**
     * ArrayStrMap value's key.
     * the key data will copy into ArrayStrMapElement malloc space.
     */
    const char* key;

    /**
     * The length of key, include '\0'.
     */
    int         keyLength;

    /**
     * ArrayStrMap value pointer.
     * the value data copy into ArrayStrMapElement malloc space.
     */
    void*       valuePtr;
}
ArrayStrMapElement;


/**
 * A list of elements each of which is a k-v pair.
 */
typedef struct
{
    /**
     * The sizeof ArrayStrMap value type.
     */
    int                            valueTypeSize;

    /**
     * Store all ArrayStrMapElements.
     */
    ArrayList(ArrayStrMapElement*) elementList[1];
}
ArrayStrMap;


static void ArrayStrMapRelease(ArrayStrMap* arrayStrMap)
{
    for (int i = 0; i < arrayStrMap->elementList->size; ++i)
    {
        free(AArrayList_Get(arrayStrMap->elementList, i, ArrayStrMapElement*));
    }

    ArrayListRelease(arrayStrMap->elementList);
}


static void ArrayStrMapInit(int valueTypeSize, ArrayStrMap* outArrayStrMap)
{
    ArrayListInit(sizeof(ArrayStrMapElement*), outArrayStrMap->elementList);
    outArrayStrMap->valueTypeSize = valueTypeSize;
}


static const char* ArrayStrMapGetKey(ArrayStrMap* arrayStrMap, int index)
{
    return AArrayList_Get(arrayStrMap->elementList, index, ArrayStrMapElement*)->key;
}


/**
 * Search index of key, if negative not found then return "-insertIndex - 1",
 * so insert index is "-BinarySearch() - 1".
 */
static int BinarySearch(ArrayList(ArrayStrMapElement)* elementList, const char* key, int keyLength)
{
    int high  = elementList->size;
    int low   = -1;
    int guess = -1;

    while (high - low > 1) // prevent infinite loops
    {
        // (high + low) always positive, so convert to unsigned
        // then the '>>' is unsigned move right
        // so the overflow will be handled correctly
        // because sign bit shift to right and 0 will be added
        guess                       = (unsigned int) (high + low) >> 1;
        ArrayStrMapElement* element = AArrayList_Get(elementList, guess, ArrayStrMapElement*);

        if (element->keyLength < keyLength)
        {
            low  = guess;
        }
        else if (element->keyLength > keyLength)
        {
            high = guess;
        }
        else if (element->keyLength == keyLength)
        {
            int cmp = memcmp(element->key, key, (size_t) keyLength);

            if (cmp < 0)
            {
                low  = guess;
            }
            else if (cmp > 0)
            {
                high = guess;
            }
            else if (cmp == 0)
            {
                // find the key, the guess is positive value
                return guess;
            }
        }
     }

    // if guess == high
    // the guess is bigger than key index and insert value at guess

    if (guess == low)
    {
        // the guess is smaller than key index and insert value behind,
        // or if list empty then the guess is -1, also do this make guess at 0
        ++guess;
    }

    // when list empty the guess is 0, so we -1 make sure return negative value
    return -guess - 1;
}


static void* ArrayStrMapGet(ArrayStrMap* arrayStrMap, const char* key, void* defaultValuePtr)
{
    int guess = BinarySearch(arrayStrMap->elementList, key, (int) strlen(key) + 1);

    return guess >= 0 ?
           AArrayList_Get(arrayStrMap->elementList, guess, ArrayStrMapElement*)->valuePtr : defaultValuePtr;
}


static void* ArrayStrMapTryPut(ArrayStrMap* arrayStrMap, const char* key, void* valuePtr)
{
    int keyLength = (int) strlen(key) + 1;
    int guess     = BinarySearch(arrayStrMap->elementList, key, keyLength);

    if (guess < 0)
    {
        int                 valueTypeSize = arrayStrMap->valueTypeSize;
        ArrayStrMapElement* element       = malloc(sizeof(ArrayStrMapElement) + valueTypeSize + keyLength);
        element->keyLength                = keyLength;
        element->valuePtr                 = (char*) element + sizeof(ArrayStrMapElement);
        element->key                      = (char*) element->valuePtr + valueTypeSize;

        memcpy((void*) element->key, key, (size_t) keyLength);
        AArrayList_Insert(arrayStrMap->elementList, -guess - 1, element);

        return memcpy(element->valuePtr, valuePtr, (size_t) valueTypeSize);
    }
    else
    {
        return NULL;
    }
}


/**
 * Marked ArrayStrMap key and value.
 */
#define ArrayStrMap(keyName, ValueType) ArrayStrMap


/**
 * Shortcut of ArrayStrMapGetAt.
 * return value.
 */
#define AArrayStrMap_GetAt(arrayStrMap, index, ValueType) \
    (*(ValueType*) AArrayList_Get(arrayStrMap->elementList, index, ArrayStrMapElement*)->valuePtr)


/**
 * Shortcut of ArrayStrMapGet.
 * return value.
 */
#define AArrayStrMap_Get(arrayStrMap, key, ValueType) \
    (*(ValueType*) ArrayStrMapGet(arrayStrMap, key, (void*[1]) {NULL}))


/**
 * Shortcut of ArrayStrMapTryPut.
 */
#define AArrayStrMap_TryPut(arrayStrMap, key, value) \
    ArrayStrMapTryPut(arrayStrMap, key, &(value))


// Define struct of JsonObject and JsonArray
//----------------------------------------------------------------------------------------------------------------------


/**
 * For json object that contains a set of k-v pairs.
 */
struct JsonObject
{
   ArrayStrMap(objectKey, JsonValue*) valueMap[1];
};


/**
 * For json array that contains a list of json value.
 */
struct JsonArray
{
   ArrayList(JsonValue*) valueList[1];
};


// Json value create and destory
//----------------------------------------------------------------------------------------------------------------------


/**
 * If the JsonValue is JsonType_Array,  then free each items and do recursively.
 * if the JsonValue is JsonType_Object, then free each K-V   and do recursively.
 */
static void Destroy(JsonValue* value)
{
    // JsonValue hold the whole memory
    // so free JsonValue will be release JsonValue's memory

    switch (value->type)
    {
        case JsonType_Array:
        {
            ArrayList* list = value->jsonArray->valueList;
            for (int i = 0; i < list->size; ++i)
            {
                Destroy(AArrayList_Get(list, i, JsonValue*));
            }

            ArrayListRelease(list);
            break;
        }

        case JsonType_Object:
        {
            ArrayStrMap* map = value->jsonObject->valueMap;
            for (int i = 0; i < map->elementList->size; ++i)
            {
                Destroy(AArrayStrMap_GetAt(map, i, JsonValue*));
            }

            ArrayStrMapRelease(map);
            break;
        }
            
        case JsonType_Float:
            break;
            
        case JsonType_String:
            break;
            
        case JsonType_Null:
            break;
    }

    free(value);
}


static JsonValue* CreateJsonValue(void* data, size_t valueSize, JsonType type)
{
    JsonValue* value = malloc(sizeof(JsonValue) + valueSize);

    switch (type)
    {
        case JsonType_Float:
            break;

        case JsonType_String:
            value->jsonString = memcpy((char*) value + sizeof(JsonValue), data, valueSize);
            break;

        case JsonType_Array:
            value->jsonArray = (JsonArray*) ((char*) value + sizeof(JsonValue));
            ArrayListInit(sizeof(JsonValue*), value->jsonArray->valueList);
            break;

        case JsonType_Object:
            value->jsonObject = (JsonObject*) ((char*) value + sizeof(JsonValue));
            ArrayStrMapInit(sizeof(JsonValue*), value->jsonObject->valueMap);
            break;

        default:
            ALog_A(false, "Json CreateJsonValue unknown JsonType = %d", type);
    }

    value->type = type;

    return value;
}

// JsonObject API
//----------------------------------------------------------------------------------------------------------------------


static bool ObjectGetBool(JsonObject* object, const char* key, bool defaultValue)
{
    JsonValue* jsonValue = AArrayStrMap_Get(object->valueMap, key, JsonValue*);
    return jsonValue != NULL ? strcmp(jsonValue->jsonString, "true") == 0 : defaultValue;
}


static int ObjectGetInt(JsonObject* object, const char* key, int defaultValue)
{
    JsonValue* jsonValue = AArrayStrMap_Get(object->valueMap, key, JsonValue*);
    
    if (jsonValue != NULL)
    {
        return (int) jsonValue->jsonFloat;
    }
    
    return defaultValue;
}


static float ObjectGetFloat(JsonObject* object, const char* key, float defaultValue)
{
    JsonValue* jsonValue = AArrayStrMap_Get(object->valueMap, key, JsonValue*);
    
    if (jsonValue != NULL)
    {
        return jsonValue->jsonFloat;
    }
    
    return defaultValue;
}


static char* ObjectGetString(JsonObject* object, const char* key, const char* defaultValue)
{
    JsonValue* jsonValue = AArrayStrMap_Get(object->valueMap, key, JsonValue*);
    return jsonValue != NULL ? jsonValue->jsonString : (char*) defaultValue;
}


static JsonObject* ObjectGetObject(JsonObject* object, const char* key)
{
    JsonValue* jsonValue = AArrayStrMap_Get(object->valueMap, key, JsonValue*);
    return jsonValue != NULL ? jsonValue->jsonObject : NULL;
}


static JsonArray* ObjectGetArray(JsonObject* object, const char* key)
{
    JsonValue* jsonValue = AArrayStrMap_Get(object->valueMap, key, JsonValue*);
    return jsonValue != NULL ? jsonValue->jsonArray : NULL;
}


static JsonType ObjectGetType(JsonObject* object, const char* key)
{
    JsonValue* jsonValue = AArrayStrMap_Get(object->valueMap, key, JsonValue*);

    if (jsonValue == NULL)
    {
        return JsonType_Null;
    }

    return jsonValue->type;
}


static const char* ObjectGetKey(JsonObject* object, int index)
{
    return ArrayStrMapGetKey(object->valueMap, index);
}


static JsonObject* ObjectGetObjectByIndex(JsonObject* object, int index)
{
    return AArrayStrMap_GetAt(object->valueMap, index, JsonValue*)->jsonObject;
}


static JsonArray* ObjectGetArrayByIndex(JsonObject* object, int index)
{
    return AArrayStrMap_GetAt(object->valueMap, index, JsonValue*)->jsonArray;
}


struct AJsonObject AJsonObject[1] =
{{
    ObjectGetBool,
    ObjectGetInt,
    ObjectGetFloat,
    ObjectGetType,
    ObjectGetString,
    ObjectGetObject,
    ObjectGetArray,
    ObjectGetKey,
    ObjectGetObjectByIndex,
    ObjectGetArrayByIndex,
}};


// JsonArray API
//----------------------------------------------------------------------------------------------------------------------


static bool ArrayGetBool(JsonArray* array, int index)
{
    return strcmp(AArrayList_Get(array->valueList, index, JsonValue*)->jsonString, "true") == 0;
}


static int ArrayGetInt(JsonArray* array, int index)
{
    return (int) AArrayList_Get(array->valueList, index, JsonValue*)->jsonFloat;
}

static float ArrayGetFloat(JsonArray* array, int index)
{
    return AArrayList_Get(array->valueList, index, JsonValue*)->jsonFloat;
}


static char* ArrayGetString(JsonArray* array, int index)
{
    return AArrayList_Get(array->valueList, index, JsonValue*)->jsonString;
}


static JsonObject* ArrayGetObject(JsonArray* array, int index)
{
    return AArrayList_Get(array->valueList, index, JsonValue*)->jsonObject;
}


static JsonArray* ArrayGetArray(JsonArray* array, int index)
{
    return AArrayList_Get(array->valueList, index, JsonValue*)->jsonArray;
}


static JsonType ArrayGetType(JsonArray* array, int index)
{
    if (index < 0 || index >= array->valueList->size)
    {
        return JsonType_Null;
    }
    
    return AArrayList_Get(array->valueList, index, JsonValue*)->type;
}


struct AJsonArray AJsonArray[1] =
{{
    ArrayGetBool,
    ArrayGetInt,
    ArrayGetFloat,
    ArrayGetType,
    ArrayGetString,
    ArrayGetObject,
    ArrayGetArray,
}};


// Json parser
//----------------------------------------------------------------------------------------------------------------------


static void SkipWhiteSpace(const char** jsonPtr)
{
    const char* json = *jsonPtr;

    while (true)
    {
        switch (*json)
        {
            case ' ' :
            case '\t':
            case '\n':
            case '\r':
                ++json;
                continue;
            default:
                break;
        }
        break;
    }

    ALog_A(json != NULL, "The Json parse error on NULL, json is incomplete.");
    
    *jsonPtr = json;
}


static void* ParseNumber(const char** jsonPtr)
{
    char* endPtr;

    JsonValue* value = CreateJsonValue(NULL, 0, JsonType_Float);
    value->jsonFloat = strtof(*jsonPtr, &endPtr);

    ALog_D("Json number = %.*s", (int) (endPtr - *jsonPtr), *jsonPtr);
    
    *jsonPtr = endPtr;

    return value;
}


static int SkipString(const char** jsonPtr, const char** outStrStart)
{
    // skip '"'
    const char* json  = ++(*jsonPtr);
    int         count = 0;
    char        c;

    // check end '"'
    while ((c = json[count++]) != '"')
    {
        if (c == '\\')
        {
            // skip escaped quotes
            // the escape char may be '\"'，which will break while
            ++count;
        }
    }

    *outStrStart = json;
    
    // already skipped the string end '"'
    *jsonPtr    += count;

    // how many char skipped
    // count contains the string end '"', so -1
    return count - 1;
}


static JsonValue* ParseString(const char** jsonPtr)
{
    const char* strStart;
    int         length        = SkipString(jsonPtr, &strStart);
    JsonValue*  value         = CreateJsonValue((void*) strStart, (length + 1) * sizeof(char), JsonType_String);
    value->jsonString[length] = '\0';

    ALog_D("Json string = %s", value->jsonString);

    return value;
}


// predefine
static JsonValue* ParseValue(const char** jsonPtr);


static JsonValue* ParseArray(const char** jsonPtr)
{
    JsonValue* jsonValue = CreateJsonValue(NULL, sizeof(JsonArray), JsonType_Array);
    ArrayList* list      = jsonValue->jsonArray->valueList;

    ALog_D("Json Array: [");
    
    // skip '['
    ++(*jsonPtr);

    do
    {
        SkipWhiteSpace(jsonPtr);

        if (**jsonPtr == ']')
        {
            break;
        }

        JsonValue* value = ParseValue(jsonPtr);
        // add Array element
        AArrayList_Add(list, value);
         
        SkipWhiteSpace(jsonPtr);

        if (**jsonPtr == ',')
        {
            ++(*jsonPtr);
        }
        else
        {
            ALog_A(**jsonPtr == ']', "Json Array not has ']', error char = %c ", **jsonPtr);
            break;
        }
    }
    while (true);

    // skip ']'
    ++(*jsonPtr);
    ALog_D("] JsonArray element count = %d", list->size);
    
    return jsonValue;
}


static JsonValue* ParseObject(const char** jsonPtr)
{
    JsonValue*   jsonValue = CreateJsonValue(NULL, sizeof(JsonObject), JsonType_Object);
    ArrayStrMap* map       = jsonValue->jsonObject->valueMap;

    ALog_D("Json Object: {");
    
    // skip '{'
    ++(*jsonPtr);

    do
    {
        SkipWhiteSpace(jsonPtr);

        if (**jsonPtr == '}')
        {
            break;
        }
        
        ALog_A(**jsonPtr == '"', "Json object parse error, char = %c, should be '\"' ", **jsonPtr);

        const char* strStart;
        int         keyLen = SkipString(jsonPtr, &strStart);
        char        key[keyLen];
        // make string end
        key[keyLen] = '\0';

        memcpy(key, strStart, (size_t) keyLen);
        ALog_D("Json key = %s", key);

        SkipWhiteSpace(jsonPtr);
        ALog_A((**jsonPtr) == ':', "Json object parse error, char = %c, should be ':' ", **jsonPtr);

        // skip ':'
        ++(*jsonPtr);
        JsonValue* value = ParseValue(jsonPtr);
        
        // set object element
        AArrayStrMap_TryPut(map, key, value);

        SkipWhiteSpace(jsonPtr);

        if (**jsonPtr == ',')
        {
            ++(*jsonPtr);
        }
        else
        {
            ALog_A(**jsonPtr == '}', "Json Object not has '}', error char = %c ", **jsonPtr);
            break;
        }
    }
    while (true);

    // skip '}'
    ++(*jsonPtr);
    ALog_D("} JsonObject elements count = %d", map->elementList->size);

    return jsonValue;
}


/**
 * ParseValue changed the *jsonPtr, so if *jsonPtr is direct malloc will cause error
 */
static JsonValue* ParseValue(const char** jsonPtr)
{
    SkipWhiteSpace(jsonPtr);

    char c = **jsonPtr;

    switch (c)
    {
        case '{':
            return ParseObject(jsonPtr);

        case '[':
            return ParseArray(jsonPtr);

        case '"':
            return ParseString(jsonPtr);

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
            return ParseNumber(jsonPtr);

        case 'f':
        {
            const char* json = *jsonPtr;

            if
            (
                json[1] == 'a' &&
                json[2] == 'l' &&
                json[3] == 's' &&
                json[4] == 'e'
            )
            {
                ALog_D("Json false");

                (*jsonPtr) += 5;

                // copy with '\0'
                return CreateJsonValue("false", 6, JsonType_String);
            }
            break;
        }

        case 't':
        {
            const char* json = *jsonPtr;

            if
            (
                json[1] == 'r' &&
                json[2] == 'u' &&
                json[3] == 'e'
            )
            {
                ALog_D("Json true");

                (*jsonPtr) += 4;

                // copy with '\0'
                return CreateJsonValue("true", 5, JsonType_String);
            }
            break;
        }

        case 'n':
        {
            const char* json = *jsonPtr;

            if
            (
                json[1] == 'u' &&
                json[2] == 'l' &&
                json[3] == 'l'
            )
            {
                ALog_D("Json null");

                (*jsonPtr) += 4;

                // copy with '\0'
                return CreateJsonValue("null", 5, JsonType_String);
            }
            break;
        }

        default:
            break;
    }

    ALog_A(false, "Invalid json value type, error char = %c", c);

    return NULL;
}


static JsonValue* Parse(const char* jsonString)
{
    return ParseValue(&jsonString);
}


struct AJson AJson[1] =
{{
    Parse,
    Destroy,
}};


#undef ALog_A
#undef ALog_D
