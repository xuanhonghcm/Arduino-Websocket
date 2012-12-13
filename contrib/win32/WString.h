#ifndef WSTRING_H
#define WSTRING_H

/*
    Facade class for Arduino WString String class
*/
#ifdef __cplusplus

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string>

// The string class
class String
{
public:
    // constructors
    // creates a copy of the initial value.
    // if the initial value is null or invalid, or if memory allocation
    // fails, the string will be marked as invalid (i.e. "if (s)" will
    // be false).
    String(const char *cstr = "");
    String(const String &str);
    explicit String(char c);
    explicit String(unsigned char, unsigned char base=10);
    explicit String(int, unsigned char base=10);
    explicit String(unsigned int, unsigned char base=10);
    explicit String(long, unsigned char base=10);
    explicit String(unsigned long, unsigned char base=10);
    ~String(void);

    unsigned int length(void) const {   return m_string.length();   };

    // creates a copy of the assigned value.  if the value is null or
    // invalid, or if the memory allocation fails, the string will be
    // marked as invalid ("if (s)" will be false).
    String & operator = (const String &rhs);
    String & operator = (const char *cstr);

    // concatenate (works w/ built-in types)

    // returns true on success, false on failure (in which case, the string
    // is left unchanged).  if the argument is null or invalid, the
    // concatenation is considered unsucessful.
//    unsigned char concat(const String &str);
//    unsigned char concat(const char *cstr);
    unsigned char concat(char c);
//    unsigned char concat(unsigned char c);
//    unsigned char concat(int num);
//    unsigned char concat(unsigned int num);
//    unsigned char concat(long num);
//    unsigned char concat(unsigned long num);

    char const* c_str() const;

    String & operator += (char c)			{concat(c); return (*this);}

    // comparison (only works w/ Strings and "strings")
    int compareTo(const String &s) const;
    unsigned char equals(const String &s) const;
    unsigned char equals(const char *cstr) const;
//    unsigned char operator == (const String &rhs) const {return equals(rhs);}
//    unsigned char operator == (const char *cstr) const {return equals(cstr);}
//    unsigned char operator != (const String &rhs) const {return !equals(rhs);}
//    unsigned char operator != (const char *cstr) const {return !equals(cstr);}
//    unsigned char operator <  (const String &rhs) const;
//    unsigned char operator >  (const String &rhs) const;
//    unsigned char operator <= (const String &rhs) const;
//    unsigned char operator >= (const String &rhs) const;

    unsigned char equalsIgnoreCase(const String &s) const;
    unsigned char startsWith( const String &prefix) const;
    unsigned char startsWith(const String &prefix, unsigned int offset) const;
    unsigned char endsWith(const String &suffix) const;

    // character acccess
    char operator [] (unsigned int index) const;
    char& operator [] (unsigned int index);
    void getBytes(unsigned char *buf, unsigned int bufsize, unsigned int index=0) const;
    void toCharArray(char *buf, unsigned int bufsize, unsigned int index=0) const
        {getBytes((unsigned char *)buf, bufsize, index);}

protected:
    std::string m_string;
};


#endif  // __cplusplus

#endif // WSTRING_H
