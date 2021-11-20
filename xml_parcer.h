// Short xml parser for C  
// Copyright (C) 2021 by Alexander Fedoseev (https://t.me.AlexanderFedoseev1984)
// This code is licensed under MIT license (see LICENSE.txt for details)
#define TESTS
#if !defined(XML_PARCER_H)
#define XML_PARCER_H
#if defined(TESTS)
#include <stdarg.h>
#include <stdio.h>
#endif
#include <stdlib.h>
#define TAG_START '<'
#define TAG_END '>'
#define TAG_END2 '/'
#define PATTERN_TAG_SEPARATOR '.'
#define PATTERN_ATTRIBUTE_SEPARATOR '@'
#define ATTRIBUTE_TOKEN_1 '='
#define ATTRIBUTE_TOKEN_2 '"'
#define DYN_BRS '['
#define DYN_BRE ']'
#define DYN_PST '%'
#define DYN_D 'd'
#define DYN_L 'l'
#define DYN_Z 'z'
#define DYN_U 'u'
#define QUESTION '?'
#define ZERO '\0'
#define IS_TRIM(x) ((x) == ' ' || (x) == '\t' || (x) == '\n' || (x) == '\r')
#define IS_S(x) ((x) == ' ' || (x) == '\t' || (x) == '\n' || (x) == '\r')
#define IS_DIGIT(x) ((x) >= '0' && (x) <= '9')
#define IS_SPACE(x) ((x) == ' ')
#define IS_TAG_END(x) (x == TAG_END)
#define IS_TAG_END2(x) (x == TAG_END2)
#define IS_TAG_PRE_END(x) ((x) == TAG_END2 || (x) == QUESTION)
#define STRLEN(x,y) {y=0;const char* z = (x);while((*z)!='\0'){(y)++;(z)++;}}
#define MEMCPY(x,y,z) {size_t a=0;while(a<z){x[a]=y[a];a++;}}
#define STRCMP(x,y,z) {size_t a=0;z=0;while(x[a]==y[a]){if(x[a] == '\0'){(z)=1;break;}a++;}}

const char* attr_parse( const char* data, 
                        size_t dataEnd, 
                        const char* pattern, 
                        char flag, 
                        char copy
                        )
{
    
    //printf("attr_parse(\"%s\", %d, %d)\n", pattern, flag, copy);
    int pattern_progress = 0;

    int pattern_length = 0;

    char is_reading_attribute_value = 0;
    
    char is_reading_target_attribute_value = 0;

    const char* d = data;

    const char* start_attr = data;

    const char* start_attr_value = data;

    const char* end_attr_value = data;

    const char* end_attr = data;
    
    STRLEN(pattern,pattern_length)
    
    while((size_t)d < dataEnd)
    {
        char pc = pattern[pattern_progress];
        
        if (*d == TAG_END && 
            !is_reading_attribute_value && 
            pattern_progress != pattern_length
        )
        {
            goto fail;
        }
        else if(is_reading_target_attribute_value)
        {
            if(start_attr_value == data)
            {
                start_attr_value = d;
            }

            if( *d == ATTRIBUTE_TOKEN_2 &&
                *(d - 1) != '\\')
            {
                end_attr_value = d;

                end_attr = d + 1;
                
                //printf("attribute exit \"%s\"\n", pattern);

                goto success;
            }
        }
        else if(pattern_progress == pattern_length)
        {
            // printf("attribute end %c%c%c->%c<-%c%c\"\n", *(d - 3), *(d-2), *(d-1), *d, *(d+1), *(d+2));
            if (*d == ATTRIBUTE_TOKEN_2 &&
                *(d - 1) == ATTRIBUTE_TOKEN_1)
            {
                
                is_reading_target_attribute_value = 1;
                
                is_reading_attribute_value = 1;
            }
            else if ( !is_reading_attribute_value && (IS_S(*d) || IS_TAG_END(*d) || (IS_TAG_PRE_END(*d) && IS_TAG_END(*(d + 1)))))
            {
                start_attr_value = d;
                
                end_attr_value = d;
                
                end_attr = d;
                
                goto success;
            }
            else if(*d != ATTRIBUTE_TOKEN_1)
            {
                goto fail_char;
            }
        }
        else if (*d == '"' && *(d - 1) != '\\')
        {
            is_reading_attribute_value = is_reading_attribute_value ? 0 : 1;
        }
        else if (*d == pc && !is_reading_attribute_value)
        {
            
            if (pattern_progress == 0 && IS_S(*(d - 1)))
            {
                // printf("attribute start %c%c%c->%c<-%c%c\"\n", *(d - 3), *(d-2), *(d-1), *d, *(d+1), *(d+2));
                start_attr = d;
            }
            else if (IS_S(*(d - 1)))
            {
                goto fail_char;
            }
            
            pattern_progress++;
            
            // printf("does attribute '%c' == '%c'? Yes,  progress:  %d/%d\n", *d, pc, pattern_progress, pattern_length);
        }
        else
        {
            fail_char:;
            
            if (pattern_progress != 0)
            {
                //printf("read attribute '%c' == '%c'? No\n", *d, pc);
                start_attr = data;
            }
            
            pattern_progress = 0;
        }

        d++;
    }
    
    fail:;

    return (const char*)0;

    success:;

    switch(flag)
    {
        case 0:;
            if (copy)
            {
                size_t length = end_attr - start_attr + 1;
                
                char* result = (char*)malloc(length);
                
                MEMCPY(result, start_attr, length)
                    
                result[length - 1] = 0;
                
                return (const char*)result;
            }
            
            return start_attr;
        break;
        case 1:;
            if (copy)
            {
                
                size_t length = end_attr_value - start_attr_value + 1;
                
                char* result = (char*)malloc(length);
                
                MEMCPY(result, start_attr_value, length)
                    
                result[length - 1] = 0;
                
                
                return (const char*)result;
            }
            
            return start_attr_value;
        
        break;
        case 2:;
            if (copy)
            {
                char* result = (char*)calloc(2,1);
                
                result[0] = *end_attr_value;
                
                return (const char*)result;
            }
            
            return end_attr_value;
        
        break;
        
        default:
            if (copy)
            {
                char* result = (char*)calloc(1,1);
                
                return (const char*)result;
            }
            return end_attr;
        break;
    }
}
const char* tag_parse( const char* data, 
                        size_t dataEnd, 
                        const char* pattern, 
                        char flag, 
                        char copy
                        )
{
        
    // printf("tag_parse(\"%s\", %d, %d)\n", pattern, flag, copy);
    int pattern_progress = 0;
    
    int pattern_length = 0;

    int pattern_depth = 0;
    
    STRLEN(pattern,pattern_length)
    
    const char* tagNameStart = data;
    const char* tagStart = data;
    const char* valueStart = data;
    const char* endTagStart = data;
    const char* endTagEnd = data;

    char is_tag_entered = 0;
    char is_inside_attribute = 0;

    int resettedPatternProgress = 0;

    size_t i = 0;

    int depth = 0;

    const char* d = (char*)data;


    
    while((size_t)d < dataEnd)
    {
        start_tag_loop:;
        
        switch(*d)
        {
            case '"':
                //printf("tag_parse -> \'\"\', is_tag_entered: %d, *(d - 1): \'%c\', is_inside_attribute: %d\n", is_tag_entered, *(d - 1), is_inside_attribute);
                if (!is_tag_entered)break;
                
                if(*(d - 1) == '\\')break;

                is_inside_attribute = is_inside_attribute ? 0 : 1;
                
            break;
            case TAG_END:;//end of started tag
                
                if (is_inside_attribute)break;
                
                is_tag_entered = 0;

                // printf("END_STARTING_TAG (>) - Go to deeper level (depth: %d <- %d, pattern_depth: %d, pattern_progress: %d)\n", depth + 1, depth, pattern_depth, pattern_progress);
                
                char isSelfendedTag =   *(d - 1) == '/' ||      //Selfended tag: <br/> or <AnyOtherTag />
                                        *(d - 1) == '?' ||      //<?xml ... ?>
                                        *(d - 1) == '-' ||      //<!-- ... -->
                                        *(d - 1) == ']' ||      //<![CDATA[ ... ]]>
                                        *(tagNameStart + 1) == '!'; //<!DOCTYPE ... >
                //if(isSelfendedTag)
                //printf("selfended tag: %c%c%c%c%c pattern_progress: %d/%d, pattern_depth: %d/%d\n", *(d - 4), *(d-3),*(d-2),*(d-1),*(d), pattern_progress, pattern_length, pattern_depth, depth);
                
                if (pattern_progress == pattern_length && pattern_depth == depth)
                {
                    valueStart = d + 1;  

                    if( isSelfendedTag)
                    {
                        //printf("selfended tag: %c%c%c%c\n", *(d),*(d+1),*(d+2),*(d+3));
                        endTagStart = valueStart;

                        endTagEnd = endTagStart;

                        //<![CDATA[ ... ]]>
                        char isCData =  *(d - 1) == ']' && 
                                        *(d - 2) == ']' && 
                                        *(tagStart + 1) == '!' && 
                                        *(tagStart + 2) == '[' &&
                                        *(tagStart + 3) == 'C' &&
                                        *(tagStart + 4) == 'D' &&
                                        *(tagStart + 5) == 'A' &&
                                        *(tagStart + 6) == 'T' &&
                                        *(tagStart + 7) == 'A' &&
                                        *(tagStart + 8) == '[';

                        char isComment =    *(d - 1) == '-' && 
                                            *(d - 2) == '-' && 
                                            *(tagStart + 1) == '!' && 
                                            *(tagStart + 2) == '-' &&
                                            *(tagStart + 3) == '-';
                        if (isCData)
                        {
                            valueStart = tagStart + 9;

                            endTagStart = endTagEnd - 3;                            
                        }

                        if (isComment)
                        {
                            valueStart = tagStart + 4;

                            endTagStart = endTagEnd - 3;                            
                        }
                    
                        goto success;                        
                    }  
                }
                
                if (!isSelfendedTag)
                { 
                    depth++;
                    // printf("regular tag: %c%c%c%c%c\n", *(tagNameStart),*(tagNameStart+1),*(tagNameStart+2),*(tagNameStart+3),*(tagNameStart+4));
                }
                // else
                // {
                //     printf("selfended tag: %c%c%c%c%c skip depth\n", *(tagNameStart),*(tagNameStart+1),*(tagNameStart+2),*(tagNameStart+3),*(tagNameStart+4));
                // }
                                
            break;
            case TAG_START:;
                
                if (is_inside_attribute)break;
                
                is_tag_entered = 1;

                tagNameStart = d;

                d++;

                if (IS_TAG_END2(*d))
                {
                    
                    //printf("START_ENDING_TAG (</) - Go to upper level (depth: %d <- %d, pattern_depth: %d, pattern_progress: %d)\n", depth - 1, depth, pattern_depth, pattern_progress);
                    
                    depth--;
                    
                    if (pattern_progress == pattern_length && pattern_depth == depth)
                    {
                        endTagStart = d - 1;    
                    }
                    
                    d++;

                    while(!IS_TAG_END(*d))
                    {
                        d++;
                    }
                    
                    
                    
                    if (pattern_progress == pattern_length && pattern_depth == depth)
                    {
                        d++;//end of ended tag
                        
                        endTagEnd = d;
                        
                        
                        goto success;
                    }

                    break;                    
                } 

                // printf("Read Tag Name\n");
                //Read Tag Name                                                
                int tag_start_pattern_progress = pattern_progress;

                resettedPatternProgress = pattern_progress;

                if (pattern_depth == depth)
                {
                    while( !IS_SPACE(*d) && !IS_TAG_END(*d))
                    {
                        const char c = *d;
                        
                        // printf("does \"%c\" == \"%c\"? ", c, pattern[pattern_progress]);
                        if(pattern[pattern_progress] == c)
                        {
                            // printf("yes progress: %d/%d\n", pattern_progress + 1, pattern_length);
                            pattern_progress++;
                            d++;
                        }
                        else
                        {
                            // printf("no progress: %d/%d reseted_pattern_progress: %d\n", pattern_progress, pattern_length, tag_start_pattern_progress);
                            pattern_progress = tag_start_pattern_progress;
                            break;
                        }
                        
                        if (pattern_progress == pattern_length)
                        {
                            char isComment = *(d - 1) == '-' && *(d - 2) == '-' && *(d - 3) == '!';
                            char isCdata = *(d - 1) == '[' && *(d - 2) == 'A' && *(d - 3) == 'T';
                            // char isTagEnd = IS_TAG_END(*d);
                            // char isTagEnd2 = IS_TAG_END2(*d);
                            // char isS = IS_S(*d);
                            // char isDataEnd = (size_t)(d + 1) > dataEnd;
                            
                            //printf("pattern_progress==pattern_length  next isDataEnd: %d, isTagEnd: %d, isTagEnd2: %d, isS: %d\n", isDataEnd, isTagEnd, isTagEnd2, isS);
                            if ((size_t)(d + 1) > dataEnd || (!isComment && !isCdata && !IS_TAG_END(*d) && !(IS_TAG_PRE_END(*d) && IS_TAG_END(*(d + 1))) && !IS_S(*d)))
                            {
                                pattern_progress = tag_start_pattern_progress;
                                // printf("reset pattern progress to: %d  d: \"%c\"\n", tag_start_pattern_progress, d[0]);
                            }
                            break;
                        }
                    }

                    if (pattern_progress == pattern_length)
                    {
                        // printf("success progress: %d/%d\n", pattern_progress, pattern_length);
                        
                        tagStart = tagNameStart;
                        
                        goto start_tag_loop;
                    }
                    else if (pattern_progress != tag_start_pattern_progress)
                    {     
                        char currentPatternChar = pattern[pattern_progress];

                        switch (currentPatternChar)        
                        {
                            case PATTERN_TAG_SEPARATOR:;

                                pattern_progress++;
                            
                                pattern_depth++;
                                
                                goto start_tag_loop;
                            break;
                        }                        
                        
                        // printf("partial success progress: %d, depth: %d\n", pattern_progress, pattern_depth);
                    }
                    
                }
                
            break;

        }
        d++;
    }

    //Fail
    return 0;
    
    success:;

    switch(flag)
    {
        case 0:;
            if (copy)
            {
                size_t length = endTagEnd - tagStart + 1;
                
                char* result = (char*)malloc(length);
                
                MEMCPY(result, tagStart, length)
                    
                result[length - 1] = 0;
                
                return (const char*)result;
            }
            
            return tagStart;
        break;
        case 1:;
            if (copy)
            {
                
                size_t length = endTagStart - valueStart + 1;
                
                
                char* result = (char*)malloc(length);
                
                MEMCPY(result, valueStart, length)
                    
                result[length - 1] = 0;
                
                
                return (const char*)result;
            }
            
            return valueStart;
        
        break;
        case 2:;
        
            if (copy)
            {
                size_t length = endTagEnd - endTagStart + 1;
                
                char* result = (char*)malloc(length);
                
                MEMCPY(result, endTagStart, length)
                    
                result[length - 1] = 0;
                
                return (const char*)result;
            }
            
            return endTagStart;
        break;
        default:
            if (copy)
            {
                
                char* result = (char*)calloc(1, 1);
                
                return (const char*)result;
            }
            return endTagEnd;
        break;
    }

}



const char* xml_parse(  const char* data,
                        size_t dataEnd,
                        const char* pattern,
                        char flag, 
                        char copy,
                        ...
                        )
{
    va_list argp;

    va_start(argp, copy);
    
    int pl = 0;
    
    STRLEN(pattern, pl)
    
    char* pat = (char*)calloc(pl + 1, 1);
    
    MEMCPY(pat, pattern, pl)
    
    
    // printf("pat length: \"%s\": %d\n", pat, pl);
    pl = pl == 0 ? 0 : pl - 1;
    
    char pattern_char = pat[pl];
    
    // printf("pattern_char: %d\n", (int)pattern_char);
    
    while(IS_TRIM(pattern_char) && pl > 0)
    {
        
        pat[pl] = ZERO;
        
        pl--;
        
        pattern_char = pat[pl];
        
    }
    
    const char* p = pat;

    const char* croppedPattern = pat;        

    const char* croppedData = data;

    char isInSquareBrackets = 0;
    
    int squareBracketNumber = -1;

    const char* squareBracketStart = 0;

    if (*pattern == ZERO)
    {
        goto fail;
    }

    

    while(*p != ZERO)
    {
        if(*p == DYN_BRS)
        {
            isInSquareBrackets = 1;

            squareBracketStart = p;
        }
        else if(*p == DYN_BRE && 
                isInSquareBrackets && 
                squareBracketNumber >= 0)
        {
            isInSquareBrackets = 0;

            goto dynamic_param;
        }   
        else if (isInSquareBrackets &&
                 *p != DYN_BRE)
        {
            if(IS_DIGIT(*p))
            {
                if (squareBracketNumber < 0) squareBracketNumber = 0;

                char currentDigit = *p - '0';
                
                squareBracketNumber *= 10;
                
                squareBracketNumber += currentDigit;
            }
            else
            {
                squareBracketNumber = -1;
            }
        }             
        else if (
            *(p - 3)    == DYN_BRS && 
            *(p - 2)    == DYN_PST && 
            *(p - 1)    == DYN_D && 
            *p          == DYN_BRE
            )
        {                        
            squareBracketStart = p - 3;

            squareBracketNumber = va_arg(argp, int);

            // printf("squareBracketNumber: va_arg = %d, croppedPattern: \"%s\", squareBracketStart: \"%s\", isInSquareBrackets: %d\n", squareBracketNumber, croppedPattern, squareBracketStart, isInSquareBrackets);

            dynamic_param:;

            isInSquareBrackets = 0;

            size_t pat_length = (size_t)(squareBracketStart - croppedPattern);

            char* croppedLeftPattern = (char*)calloc(pat_length + 1, 1);

            MEMCPY(croppedLeftPattern, croppedPattern, pat_length)  

            croppedPattern = p + 1;
            
            if (croppedPattern[0] == PATTERN_TAG_SEPARATOR)
            {
                croppedPattern++;
            }
            
            char* secondLeftPattern = croppedLeftPattern + pat_length;
            
            while(secondLeftPattern > croppedLeftPattern)
            {
                if (secondLeftPattern[0] == PATTERN_TAG_SEPARATOR)
                {
                    secondLeftPattern++;
                    break;
                }
                secondLeftPattern--;
            }
            
            int iterations = 0;
            //Parce multiple times until param will become 1
            while(squareBracketNumber > 0)
            {
                // printf("tag_parse squareBracketNumber: %d\n", squareBracketNumber);
                croppedData = tag_parse(croppedData, dataEnd, iterations == 0 ? croppedLeftPattern : secondLeftPattern, 3, 0);
                
                iterations++;
                
                squareBracketNumber--;
                
                if (croppedData == 0)
                {
                    // printf("goto fail: croppedData == 0\n");
                    free(croppedLeftPattern);

                    goto fail;
                }
                else
                {
                    // printf("croppedData[%d] = \"%c%c%c%c\"\n", squareBracketNumber, croppedData[0], croppedData[1], croppedData[2], croppedData[3]);
                }
            }
            
            
            squareBracketNumber = -1;

            //If ']' is the last character of pattern
            if (*(p + 1) == ZERO)
            {
                
                croppedData = tag_parse(croppedData, dataEnd, iterations == 0 ? croppedLeftPattern : secondLeftPattern, flag, copy);
                

                free(croppedLeftPattern);

                goto success;
            }
            else
            {
                
                croppedData = tag_parse(croppedData, dataEnd, iterations == 0 ? croppedLeftPattern : secondLeftPattern, croppedPattern[0] != PATTERN_ATTRIBUTE_SEPARATOR ? 1 : 0, 0);
                
                // printf("croppedData = \"%c%c%c%c\"\n", croppedData[0], croppedData[1], croppedData[2], croppedData[3]);
            }
                        
            free(croppedLeftPattern);

            if (croppedData == 0)
            {                
                goto fail;
            } 
              
            // printf("tag_parse complete tag_parse: %s, *p: \"%s\"\n", croppedPattern, p);
        }
        else if(*p == PATTERN_ATTRIBUTE_SEPARATOR && 
                *(p + 1) != ZERO)
        {
            size_t pat_length = (size_t)(p - croppedPattern);
            
            char* croppedLeftPattern = (char*)calloc(pat_length + 1, 1);

            MEMCPY(croppedLeftPattern, croppedPattern, pat_length) 

            //printf("tag_parse: \"%s\", text: %s\n", croppedLeftPattern, croppedData);
            if (*(p - 1) != DYN_BRE)
            {
                croppedData = tag_parse(croppedData, dataEnd, croppedLeftPattern, 0, 0);
            }
            
            free(croppedLeftPattern);
            
            if (croppedData == 0)
            {
                // printf("croppedData == 0\n");
                goto fail;
            }
            
            const char* attribute_pattern = p + 1;
            
            
            // printf("attr_parse \"%s\"\n", attribute_pattern);
            
            croppedData = attr_parse(croppedData, dataEnd, attribute_pattern, flag, copy);

            goto success;
        }
        
        p++;
        
    }

    

    croppedData = tag_parse(croppedData, dataEnd, croppedPattern, flag, copy);

    goto success;

    fail:;

    va_end(argp);
    
    free(pat);

    return 0; 

    success:;
    
    va_end(argp);
    
    free(pat);

    return croppedData;   
    
}
#endif


//You can delete all lines below in production
#if defined(TESTS)
//Tests
int main()
{
    // pattern: "code"
    //   flags:  0.....1...............2.....3     
    //    text:  <code>some text or xml</code> 
    //    copy: 1 - Use malloc to create a copy of string result 
    //          0 - return pointer to result from input data
    
    const char* test_xmls[100];
    const char* test_patterns[100];
    const char* test_expected_results[100];
    int test_varargs[100];
    

    test_xmls[0] = "<Error><Code>AccessDenied</Code><Message>Access Denied</Message><RequestId>AGFTEFECFBV3J1EQ</RequestId><HostId>RjqjBsHB+mNcJwbd0mQcbUCQx3OJb0nboypfW+WJt2pZa3RbK+Vbg7Qoqlnu4oGNZFC62wrK6mU=</HostId></Error>";    
    test_xmls[1] = "<?xml version=\"1.0\" encoding=\"utf-8\"?> <androidx.coordinatorlayout.widget.CoordinatorLayout         xmlns:android=\"http://schemas.android.com/apk/res/android\"         xmlns:tools=\"http://schemas.android.com/tools\"         xmlns:app=\"http://schemas.android.com/apk/res-auto\"         android:layout_width=\"match_parent\"         android:layout_height=\"match_parent\"         android:fitsSystemWindows=\"true\"         tools:background=\"@color/white_50\"         android:clipChildren=\"false\"         tools:context=\".CameraActivity\"  >      <!--Контейнер для камеры и точки-->     <FrameLayout             android:id=\"@+id/preview\"             android:layout_width=\"match_parent\"             android:layout_height=\"match_parent\"             android:background=\"@color/fblack_100\"             tools:background=\"@color/fwhite_50\"             android:fitsSystemWindows=\"true\"             android:clipChildren=\"false\"             tools:visibility=\"invisible\"      >         <ImageView                 android:id=\"@+id/dot\"                 android:layout_width=\"24dp\"                 android:layout_height=\"24dp\"                 android:src=\"@drawable/dot\"                 android:layout_gravity=\"center_horizontal|center_vertical\"                 android:alpha=\"0\"          />      </FrameLayout>      <!--Анти артефактное текстовое поле. Чтобы прорисовывалась верхняя часть экрана-->     <TextView          android:layout_width=\"wrap_content\"         android:layout_height=\"wrap_content\"         android:layout_gravity=\"top\"         android:text=\"\n\n\n\n\"         android:textSize=\"@dimen/menu_item_text_size\"         android:textColor=\"@color/colorwhite\"         android:textStyle=\"bold\"         android:gravity=\"center\"         android:layout_marginTop=\"-40dp\"          />     <!--Анти артефактное текстовое поле. Чтобы прорисовывалась нижняя часть экрана-->     <TextView          android:layout_width=\"wrap_content\"         android:layout_height=\"wrap_content\"         android:layout_gravity=\"bottom\"         android:text=\"\n\n\n\n\"         android:textSize=\"@dimen/menu_item_text_size\"         android:textColor=\"@color/colorwhite\"         android:textStyle=\"bold\"         android:gravity=\"center\"         android:layout_marginBottom=\"-50dp\"          />       <FrameLayout         android:id=\"@+id/modalHide\"         android:layout_width=\"match_parent\"         android:layout_height=\"match_parent\"         android:clipToPadding=\"false\"         android:clipChildren=\"false\"         >          <!--Анимация сканирования-->         <FrameLayout                 android:id=\"@+id/scanningFrame\"                 android:layout_width=\"match_parent\"                 android:layout_height=\"match_parent\"                 android:clipChildren=\"true\"              android:clipToPadding=\"false\"           >              <FrameLayout                 android:id=\"@+id/defaultScanningPanel\"                 android:layout_width=\"match_parent\"                 android:layout_height=\"match_parent\"              >                 <!--Наведите на фото-->                 <TextView                     android:id=\"@+id/descBt\"                     android:layout_width=\"wrap_content\"                     android:layout_height=\"wrap_content\"                     android:layout_gravity=\"center\"                     android:text=\"@string/scanning\"                     android:textSize=\"@dimen/menu_item_text_size\"                     android:textColor=\"@color/fwhite_100\"                     android:textStyle=\"bold\"                     android:gravity=\"center\"                     android:layout_marginTop=\"-200dp\"                     style=\"@style/Font1Bold\"                     />                  <ImageView                     android:id=\"@+id/vcircle\"                     android:layout_width=\"250dp\"                     android:layout_height=\"270dp\"                     app:srcCompat=\"@drawable/scan\"                     android:layout_gravity=\"center\"                 />               </FrameLayout>              <FrameLayout                 android:id=\"@+id/scanningQRPanel\"                 android:layout_width=\"match_parent\"                 android:layout_height=\"match_parent\"                 android:clipChildren=\"false\"                 android:visibility=\"gone\"                 tools:visibility=\"visible\"                 >                 <!--QR-instructions-->                 <include layout=\"@layout/layout_scanning_qr_1\"  />                </FrameLayout>          </FrameLayout>      </FrameLayout>      <!--Таймер-->     <FrameLayout         android:id=\"@+id/timerContainer\"         android:layout_width=\"wrap_content\"         android:layout_height=\"wrap_content\"         android:layout_gravity=\"center|top\"         android:layout_marginTop=\"23dp\"         android:paddingLeft=\"10dp\"         android:paddingRight=\"10dp\"         android:background=\"@drawable/timer_background\"         android:visibility=\"gone\"          >         <TextView             android:id=\"@+id/timerText\"             android:layout_gravity=\"center\"             android:layout_width=\"wrap_content\"             android:layout_height=\"wrap_content\"             android:fontFamily=\"@font/source_sans_pro_regular\"             android:textColor=\"@color/white_100\"             android:textSize=\"18sp\"             android:text=\"00:00\"             />     </FrameLayout>      <!--Пунктирная линия-->     <ru.arproduction.mmrs.cameraeasyar2.videohelp.DashedLineView         android:id=\"@+id/dashedLine\"          android:layout_width=\"0dp\"         android:layout_height=\"150dp\"         android:layout_gravity=\"bottom|center\"         android:layout_marginBottom=\"@dimen/dash_bottom_anchor\"         app:dash_fill=\"4dp\"         app:dash_space=\"4dp\"         app:line_width=\"2dp\"         app:direction=\"TOP_LEFT\"         android:visibility=\"gone\"         />      <!--Баннер призывающий к записи видео-->     <ru.arproduction.mmrs.cameraeasyar2.videohelp.SpringFrameLayout         android:id=\"@+id/videoHelpFragmentContainer\"         android:layout_width=\"wrap_content\"         android:layout_height=\"wrap_content\"         android:layout_gravity=\"center\"          >         <fragment              android:id=\"@+id/videoHelpFragment\"             android:layout_width=\"wrap_content\"             android:layout_height=\"wrap_content\"             android:name=\"ru.arproduction.mmrs.cameraeasyar2.videohelp.VideoHelp\"             tools:layout=\"@layout/video_help_fragment\"             android:layout_gravity=\"center\"              />      </ru.arproduction.mmrs.cameraeasyar2.videohelp.SpringFrameLayout>      <fragment          android:id=\"@+id/settingsFragment\"         android:layout_width=\"wrap_content\"         android:layout_height=\"wrap_content\"         android:name=\"ru.arproduction.mmrs.cameraeasyar2.DisplaySettings\"         tools:layout=\"@layout/display_settings_fragment\"         android:layout_gravity=\"center\"           />      <!--Меню материалы-->     <include layout=\"@layout/layout_header_menu\" />      <!--Video Player-->     <include layout=\"@layout/layout_video_player_faded\" />       <View         android:id=\"@+id/debugTriggerPanel\"         android:layout_width=\"60dp\"         android:layout_height=\"60dp\"         android:layout_gravity=\"top|center\"         android:background=\"@drawable/roundedcorners\"         android:alpha=\"0\"          />      <!--Контейнер для футера-->     <FrameLayout             android:id=\"@+id/footerContainer\"             android:layout_width=\"match_parent\"             android:layout_height=\"match_parent\"             android:clipChildren=\"false\"      >         <!--футер-->         <include layout=\"@layout/layout_footer_light\"  />      </FrameLayout> </androidx.coordinatorlayout.widget.CoordinatorLayout> ";
    test_xmls[2] = "<Вася><Петя>Арена Спартак</Петя><Семен>Шайба</Семен></Вася>";
    test_xmls[3] = "<one attr=\"<two>no</two>\"><two>yes</two></one>";
    test_xmls[4] = "<!one>";
    test_xmls[5] = "<one name=\"<two>no</two>\" type=\"Ха-ха-ха! Ну ты даешь!\" mime=\"text/jpeg\"><two name=\"uio\" n=\"bla bla bla\" ><three name=\"kel mel\">yes</three></two></one>";
    test_xmls[6] = "<!DOCTYPE htmln html=\"html1\"><!DOCTYPE\r\nHTML\rTTO=\"0\"\nPUBLIC shmublic  html=\"html2\"  \"-//W3C//DTD HTML 4.01//EN\"   \"http://www.w3.org/TR/html4/strict.dtd\"><html> <head>   <title>!DOCTYPE</title>   <meta charset=\"utf-8\"/> </head> <body><p 1>P1</p><p 2>P2</p><p 3>P3</p><p 4>P4</p><p><p 11>PP1</p><p 12>PP2</p><p 13>PP3</p></p>  <p>Разум — это Будда, а прекращение умозрительного мышления — это путь.   Перестав мыслить понятиями и размышлять о путях существования и небытия,   о душе и плоти, о пассивном и активном и о других подобных вещах,   начинаешь осознавать, что разум — это Будда,   что Будда — это сущность разума,   и что разум подобен бесконечности.</p> </body><body/> </html>";
    
    test_patterns[0] = "Error"; 
    test_patterns[1] = "error";
    test_patterns[2] = "erro r";
    test_patterns[3] = " error";
    test_patterns[4] = "error ";
    test_patterns[5] = "Error.";
    test_patterns[6] = "Error.Code";
    test_patterns[7] = "Error.Code.";
    test_patterns[8] = "Error.Code.\n";
    test_patterns[9] = "Error.Code\r\n";
    test_patterns[10] = "Error.Code\r";
    test_patterns[11] = "Error.Message";
    test_patterns[12] = "Error.RequestId";
    test_patterns[13] = "Error.HostId";    

    test_patterns[14] = "androidx.coordinatorlayout.widget.CoordinatorLayout.FrameLayout@android:id";
    test_patterns[15] = "?xml";
    test_patterns[16] = "?xml@version";
    test_patterns[17] = "?xml@encoding";
    test_patterns[18] = "androidx.coordinatorlayout.widget.CoordinatorLayout.FrameLayout@tools:visibility";
    test_patterns[19] = "androidx.coordinatorlayout.widget.CoordinatorLayout@xmlns:android";
    test_patterns[20] = "androidx.coordinatorlayout.widget.CoordinatorLayout@xmlns:tools";
    test_patterns[21] = "androidx.coordinatorlayout.widget.CoordinatorLayout.!--";
    test_patterns[22] = "androidx.coordinatorlayout.widget.CoordinatorLayout.FrameLayout.ImageView";
    test_patterns[23] = "androidx.coordinatorlayout.widget.CoordinatorLayout.FrameLayout.ImageView@android:id";

    test_patterns[24] = "Вася.Семен";
    test_patterns[25] = "one.two";
    test_patterns[26] = "one.two@name";    
    test_patterns[27] = "one@attr";
    test_patterns[28] = "!one";    
    test_patterns[29] = "html.body.p[3].p[0]@11";
    test_patterns[30] = "Error..";

    int test_flags[] = {
        0, 0, //0
        0, 1, //1
        1, 0, //2
        1, 1, //3
        2, 0, //4
        2, 1, //5
        3, 0, //6
        3, 1  //7        
    };
    
    test_expected_results[0] = 0;
    test_expected_results[1] = test_xmls[0];
    test_expected_results[2] = test_xmls[0] + 7;//<Code>Acce...
    test_expected_results[3] = test_xmls[0] + 196;//</Error>...
    test_expected_results[4] = "<Code>AccessDenied</Code><Message>Access Denied</Message><RequestId>AGFTEFECFBV3J1EQ</RequestId><HostId>RjqjBsHB+mNcJwbd0mQcbUCQx3OJb0nboypfW+WJt2pZa3RbK+Vbg7Qoqlnu4oGNZFC62wrK6mU=</HostId>";
    test_expected_results[5] = "</Error>";
    test_expected_results[6] = "AccessDenied";
    test_expected_results[7] = "Access Denied";
    test_expected_results[8] = "AGFTEFECFBV3J1EQ";
    test_expected_results[9] = "RjqjBsHB+mNcJwbd0mQcbUCQx3OJb0nboypfW+WJt2pZa3RbK+Vbg7Qoqlnu4oGNZFC62wrK6mU=";
    test_expected_results[10] = "<HostId>RjqjBsHB+mNcJwbd0mQcbUCQx3OJb0nboypfW+WJt2pZa3RbK+Vbg7Qoqlnu4oGNZFC62wrK6mU=</HostId>";
    test_expected_results[11] = "@+id/preview";
    test_expected_results[12] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
    test_expected_results[13] = "";
    test_expected_results[14] = "1.0";
    test_expected_results[15] = "utf-8";
    test_expected_results[16] = "invisible";
    test_expected_results[17] = "http://schemas.android.com/apk/res/android";
    test_expected_results[18] = "http://schemas.android.com/tools";
    test_expected_results[19] = "Контейнер для камеры и точки";
    test_expected_results[20] = "<!--Контейнер для камеры и точки-->";
    test_expected_results[21] = "-->";
    test_expected_results[22] = "<ImageView                 android:id=\"@+id/dot\"                 android:layout_width=\"24dp\"                 android:layout_height=\"24dp\"                 android:src=\"@drawable/dot\"                 android:layout_gravity=\"center_horizontal|center_vertical\"                 android:alpha=\"0\"          />";
    test_expected_results[23] = "@+id/dot";
    test_expected_results[24] = "Шайба";
    test_expected_results[25] = "<Семен>Шайба</Семен>";
    test_expected_results[26] = "</Семен>";
    test_expected_results[27] = "yes";
    test_expected_results[28] = "<two>no</two>";
    test_expected_results[29] = "<!one>";


    int test_cases[] = {
    //xml,  pattern, flag, expected
        0,        0,    0,        1,//0
        0,        0,    2,        2,//1
        0,        0,    4,        3,//2
        0,        0,    3,        4,//3
        0,        0,    7,       13,//4
        0,        6,    3,        6,//5
        0,       11,    3,        7,//6
        0,        2,    0,        0,//7
        0,        3,    1,        0,//8
        0,        4,    2,        0,//9
        0,        5,    3,        6,//10
        0,       12,    3,        8,//11
        0,       13,    3,        9,//12
        0,       13,    1,       10,//13
        0,        7,    3,        0,//14
        0,        8,    3,        0,//15
        0,        9,    3,        6,//16
        0,       10,    3,        6,//17
            
        1,       14,    3,       11,//18 
        1,       15,    3,       13,//19 
        1,       15,    1,       12,//20 
        1,       15,    5,       13,//21 
        1,       15,    7,       13,//22 
        1,       15,    7,       13,//23 
        1,       16,    3,       14,//24 
        1,       17,    3,       15,//25 
        1,       18,    3,       16,//26 
        1,       19,    3,       17,//27 
        1,       19,    3,       17,//28 
        1,       20,    3,       18,//29 
        1,       21,    3,       19,//30 
        1,       21,    1,       20,//31 
        1,       21,    5,       21,//32 
        1,       21,    7,       13,//33 
        1,       22,    3,       13,//34 
        1,       23,    3,       23,//35 


        2,       24,    3,       24,//36 
        2,       24,    1,       25,//37 
        2,       24,    5,       26,//38 
        2,       24,    7,       13,//39 

        3,       25,    3,       27,//40 
        3,       26,    3,        0,//41 
        3,       27,    3,       28,//42

        4,       28,    3,       13, //43
        0,       30,    3,        0 //44
    };

    
    int textLength = 0; 

    int patternLength = 0;

    size_t dataLimit;
    
    const char* result;

    char is_equal;
    
    int cases_length = sizeof(test_cases) / sizeof(int);
    
    int success_tests = 0;
    
    int test_only_i = -1;

    for (int i = 0; i < cases_length; i+=4)
    {
        if (test_only_i >= 0)
        {
            if(test_only_i != i / 4)
            {
                continue;
            }
        }
        const char* text = test_xmls[test_cases[i + 0]];
        const char*  pattern = test_patterns[test_cases[i + 1]];
        int* flags = test_flags + (size_t)test_cases[i + 2] * 2;
        const char* expected = test_expected_results[test_cases[i + 3]];

        STRLEN(text, textLength)
        STRLEN(pattern, patternLength)
        size_t dataLimit = (size_t)text + (size_t)textLength;

        result = xml_parse(text, dataLimit, pattern, flags[0], flags[1]);

        if(result == 0 || expected == 0)
        {
            is_equal = result == expected ? 1 : 0;
        }
        else
        {
            STRCMP(result, expected, is_equal)    
        }
        

        if (is_equal)
        {
            //printf("%d/%d. SUCCESS\n", i / 4, (cases_length / 4 - 1));
            success_tests++;
        }
        else
        {
            printf("%d/%d. FAIL xml_parse(\"%s\", %d, %d) expected: \"%s\", result: \"%s\"\n", i / 4, (cases_length / 4 - 1), pattern, flags[0], flags[1], expected, result);
        }
    }    
    
    if (success_tests == cases_length / 4)
    {
        printf("All %d tests passed SUCCESSFULLY", success_tests);
    }
    
    return 0;
}
#endif
//*/

