## v1.2.3
_`2021.2.5 UTC+8 10:13`_
* **The C Code**
  * Fix the parse key bug of ParseObject.

* **The C# Code**
  * Code formatting of #region.  

## v1.2.2
_`2020.2.28 UTC+8 10:50`_
* **The C# Code**
  * Change the `Data` type from `class` to `struct`, and use `ref` to pass it.
  * Use keyword `out` inline to follow the compiler hits.

_`2020.1.10 UTC+8 11:35`_
* **The C Code**
  * Add the C code implementation.


## v1.2.0
_`2020.1.1 UTC+8 11:16`_
* Optimize unicode string parsing.
* Add `SkipString` for `ParseObject`.
* Add `SetEscapeString` for whether to convert escaped strings.

## v1.1.8
_`2018.2.5 UTC+8 10:51`_
* Optimize ParseArray and ParseObject that remove redundant variable assignment. (+0.0.2)
* Optimize DebugTool Assert call. (+0.0.1)



## v1.1.5
_`2018.1.25 UTC+8 16:57`_
* Code format.(+0.0.1)
* Improve function UnicodeCharToInt and ParseString. (+0.0.2)
* Refactor JsonValue fields. (+0.1.0)
* Refactor struct Data to class Data. (+0.0.1)
* Optimize JSON function SkipWhiteSpace call in ParseArray and ParseObject. (+0.0.1)

## v1.0
_`2018.1.15 UTC+8 22:07`_

* Initial Release.
