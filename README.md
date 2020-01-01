## MojoJson v1.2.0

MojoJson is an **extremely simple** and **super fast** JSON parser. The parser supports all **standard** Json formats and provides **simple** APIs for visit different types of the Json values. Also the **core algorithm** can be easily implemented by various programming languages.

* Released version in [releases](https://github.com/scottcgi/MojoJson/releases).
* Release changes in [ChangeLog](https://github.com/scottcgi/MojoJson/blob/master/ChangeLog.md).


The current implementations as follow:

* For C#.   
  
  The core parsing code only 400 lines, and the implementation only use the core .NET lib, and just has one file that can be easily integrated into any C# project.


## License

MojoJson is licensed under the [MIT License](https://github.com/scottcgi/MojoJson/blob/master/LICENSE).

## How to use

* For C#

  * Parse Json string.

  ```csharp
  var jsonValue = MojoJson.Json.Parse(string jsonString);
  ```

  * Whether to convert escaped strings when `ParseString`.

  ```csharp
  /// default false
  MojoJson.Json.SetEscapeString(bool isEscapeString)
  ```

  * JsonValue is `primitive` type.

  ```csharp
  public string AsString();
  public float  AsFloat();
  public int    AsInt();
  public bool   AsBool();
  public bool   IsNull();
  ```
  
  * JsonValue is `JsonObject` 

  ```csharp
  /// Get the JsonObject, that is a set of k-v pairs, and each value is JsonValue.
  public Dictionary<string, JsonValue> AsObject();
  
  /// Get the JsonValue from JsonObject by key.
  public JsonValue AsObjectGet(string key);
  
  /// Get the JsonObject from JsonObject by key.
  public Dictionary<string, JsonValue> AsObjectGetObject(string key);
  
  /// Get the JsonArray from JsonObject by key.
  public List<JsonValue> AsObjectGetArray(string key);
  
  /// Get the string from JsonObject by key.
  public string AsObjectGetString(string key);
  
  /// Get the float from JsonObject by key.
  public float AsObjectGetFloat(string key);
  
  /// Get the int from JsonObject by key.
  public int AsObjectGetInt(string key);
  
  /// Get the bool from JsonObject by key.
  public bool AsObjectGetBool(string key);
  
  /// Get the null from JsonObject by key.  
  public bool AsObjectGetIsNull(string key);
  ```

  * JsonValue is `JsonArray`.

  ```csharp
  /// Get the JsonArray, that is JsonValue list.
  public List<JsonValue> AsArray();
  
  /// Get the JsonValue from JsonArray at index.
  public JsonValue AsArrayGet(int index);
  
  /// Get the JsonObject from JsonArray at index.
  public Dictionary<string, JsonValue> AsArrayGetObject(int index);
  
  /// Get the JsonArray from JsonArray at index.
  public List<JsonValue> AsArrayGetArray(int index);
  
  /// Get the string from JsonArray at index. 
  public string AsArrayGetString(int index);
  
  /// Get the float from JsonArray at index.
  public float AsArrayGetFloat(int index);
  
  /// Get the int from JsonArray at index.
  public int AsArrayGetInt(int index);
  
  /// Get the bool from JsonArray at index.
  public bool AsArrayGetBool(int index);
  
  /// Get the null from JsonArray at index.
  public bool AsArrayGetIsNull(int index);  
  ```

    
## How was born

The original implementation and core algorithm came from the [Json.h](https://github.com/scottcgi/Mojoc/blob/master/Engine/Toolkit/Utils/Json.h) of the pure C game engine [Mojoc](https://github.com/scottcgi/Mojoc).


## Support

If the source code is **useful** for you, maybe you could buy me a coffee via [Paypal-0.99](https://www.paypal.me/PayScottcgi/0.99) :coffee:
