# xml_parser_c
One .h file for xml parsing for C
## It's very easy to use it

```c
const char* xml_string = "<Error><Code>AccessDenied</Code><Message>Access Denied</Message></Error>";

const char* result = xml_parse(xml_string, -1, "Error.Code", 1, 1);

printf("$s", result);
```
Output:
```c
AccessDenied
```

## Function and parameters:
#### Signature
```c
const char* xml_parse(const char* xml, int length, const char* pattern, int access, char is_copy)
```
#### Params 
1.  `xml` - input string in xml format
2.  `length` - Total characters for parsing (no feof checks). Or `-1` - until the end (for zero terminated string)
3.  `pattern` - string with `.`(dot) separator for tags. You can use array `[]` notation to access equal tags on one level. Also there is `@` to access attributes. Example of complicated valid pattern: "root.element[13].item[21].name@lang" to access lang attribute on third level of deepness name tag.
4.  `access` - There is 4 values: `0`,`1`,`2`,`3`  to access different part of target tag:
```c
0.....1...........2......3
<code>AccessDenied</code>
```
 5. `is_copy` - Flag. Values are: `0` - return pointer from input string, `1` - create a new string.
 
## Understanding `access` and `is_copy` parameters
For this kind of input string:
```xml
<Error><Code>AccessDenied</Code><Message>Access Denied</Message></Error>
```
And this kind of pattern:
```c
Error.Code
```
This table shows how these two parameters `access` and `is_copy` change the target result
| access | is_copy | result |
| ------ | ------ | ------ |
| `0` | `1` |`<code>AccessDenied</code>` |
| `1` | `1` |`AccessDenied` |
| `2` | `1` |`</code>` |
| `3` | `1` | empty string |
| `0` | `0` | `<code>AccessDenied</code><Mess...` |
| `1` | `0` | `AccessDenied</code><Mess...` |
| `2` | `0` | `</code><Mess...` |
| `3` | `0` | `<Mess...` |

## What about single tags?
For example input string is as follows:
```xml
<Error><Code>AccessDenied</Code><single name="any"/></Error>
```
Pattern like so
```c
Error.single
```
| access | is_copy | result |
| ------ | ------ | ------ |
| `0` | `1` |`<single name="any"/>` |
| `1` | `1` | empty string |
| `2` | `1` | empty string |
| `3` | `1` | empty string |
| `0` | `0` | `<single name="any"/></Erro...` |
| `1` | `0` | `</Erro...` |
| `2` | `0` | `</Erro...` |
| `3` | `0` | `</Erro...` |
## Comments?
 For example we have this gipotethic xml:
```xml
<root>
    <element1 /> 
    <!--This is the comment--> 
    <code>38389338</code> 
    <!--This is the comment # 2-->
    <element2 color="black"/>
</root>
```
Pattern to get a comment 1:
```c
root.!--
```
|pattern| access | is_copy | result |
|------| ------ | ------ | ------ |
|root.!--| `0` | `1` |`<!--This is the comment-->` |
|root.!--| `1` | `1` | `This is the comment` |
|root.!--| `2` | `1` | `-->` |
|root.!--| `3` | `1` | empty string |
|root.!--| `0` | `0` | `<!--This is the comment--> <code>3...` |
|root.!--| `1` | `0` | `This is the comment--> <code>3...` |
|root.!--| `2` | `0` | `--> <code>3...` |
|root.!--| `3` | `0` | ` <code>3...` |
|||||
|root.!--[1]| `0` | `1` |`<!--This is the comment # 2-->` |
|root.!--[1]| `1` | `1` | `This is the comment # 2` |
|root.!--[1]| `2` | `1` | `-->` |
|root.!--[1]| `3` | `1` | empty string |
|root.!--[1]| `0` | `0` | `<!--This is the comment # 2--> <eleme...` |
|root.!--[1]| `1` | `0` | `This is the comment # 2--> <eleme...` |
|root.!--[1]| `2` | `0` | `--> <eleme...` |
|root.!--[1]| `3` | `0` | ` <eleme...` |
> Note: For comment N use: `root.!--[N-1]` pattern
> `root.!--[0]` same as `root.!--`
For undefined comment use: `root.!--[%d]` pattern and vararg param
## CDATA sections
 For example we have xml:
```xml
<root>
    <element1 /> 
    <!--This is the comment--> 
    <code><![CDATA[9879879879]]></code> 
    <!--This is the comment # 2-->
    <element2 color="black"/>
</root>
```
Pattern to get a CDATA section:
```c
root.code.![CDATA[
```
| access | is_copy | result |
| ------ | ------ | ------ |
| `0` | `1` |`<![CDATA[9879879879]]>` |
| `1` | `1` | `9879879879` |
| `2` | `1` | `]]>` |
| `3` | `1` | empty string |
| `0` | `0` | `<![CDATA[9879879879]]></code...` |
| `1` | `0` | `9879879879]]></code...` |
| `2` | `0` | `]]></code...` |
| `3` | `0` | `</code...` |

## License

MIT
