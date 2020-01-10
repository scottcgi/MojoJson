/*
 * Copyright (c) scott.cgi All Rights Reserved.
 * 
 * This source code belongs to project MojoJson, which is hosted on GitHub, and licensed under the MIT License.
 *
 * License: https://github.com/scottcgi/MojoJson/blob/master/LICENSE
 * GitHub : https://github.com/scottcgi/MojoJson
 *
 * Since  : 2013-1-26
 * Author : scott.cgi
 * Version: 1.2.0
 * Update : 2020-1-8
 */

 
#ifndef JSON_H
#define JSON_H

#include <stdbool.h>


/**
 * Json value type.
 */
typedef enum
{
    JsonType_Object,
    JsonType_Array,
    JsonType_String,
    JsonType_Float,
    JsonType_Null,
}
JsonType;


/**
 * For json object that contains a set of k-v pairs.
 */
typedef struct JsonObject JsonObject;


/**
 * For json array that contains a list of json value.
 */
typedef struct JsonArray JsonArray;


/**
 * One json value.
 */
typedef struct
{
    JsonType type;

    union
    {
        /**
         * For JsonType_String.
         */
        char*       jsonString;

        /**
         * For JsonType_Object.
         */
        JsonObject* jsonObject;

        /**
         * For JsonType_Array.
         */
        JsonArray*  jsonArray;

        /**
         * For JsonType_Float and int value.
         */
        float       jsonFloat;
    };
}
JsonValue;


/**
 * Get different types of values JsonObject.
 */
struct AJsonObject
{
    bool        (*GetBool)         (JsonObject* object, const char* key, bool  defaultValue);
    int         (*GetInt)          (JsonObject* object, const char* key, int   defaultValue);
    float       (*GetFloat)        (JsonObject* object, const char* key, float defaultValue);
    JsonType    (*GetType)         (JsonObject* object, const char* key);

    /**
     * When JsonValue released the string value will free.
     */
    char*       (*GetString)       (JsonObject* object, const char* key, const char* defaultValue);

    /**
     * If not found return NULL.
     */
    JsonObject* (*GetObject)       (JsonObject* object, const char* key);

    /**
     * If not found return NULL.
     */
    JsonArray*  (*GetArray)        (JsonObject* object, const char* key);

    /**
     * Get JsonObject's key.
     */
    const char* (*GetKey)          (JsonObject* object, int index);

    /**
     * Get index of JsonObject in JsonObject map.
     */
    JsonObject* (*GetObjectByIndex)(JsonObject* object, int index);

    /**
     * Get index of JsonObject in JsonArray map.
     */
    JsonArray*  (*GetArrayByIndex) (JsonObject* object, int index);
};


extern struct AJsonObject AJsonObject[1];


/**
 * Get different types of values from JsonArray.
 */
struct AJsonArray
{
    bool        (*GetBool)  (JsonArray* array, int index);
    int         (*GetInt)   (JsonArray* array, int index);
    float       (*GetFloat) (JsonArray* array, int index);
    JsonType    (*GetType)  (JsonArray* array, int index);

    /**
     * When JsonValue released the string value will free.
     */
    char*       (*GetString)(JsonArray* array, int index);
    JsonObject* (*GetObject)(JsonArray* array, int index);
    JsonArray*  (*GetArray) (JsonArray* array, int index);

};


extern struct AJsonArray AJsonArray[1];


/**
 * Control Json data.
 */
struct AJson
{
    /**
     * Parse the Json string, return root JsonValue.
     */
    JsonValue* (*Parse)    (const char* jsonString);


    /**
     * Destroy JsonValue member memory space and free itself,
     * if Destroy root JsonValue will free all memory space.
     *
     * important: after Destroy the jsonValue will be invalidated.
     */
    void       (*Destroy)  (JsonValue* jsonValue);
};


extern struct AJson AJson[1];


#endif
