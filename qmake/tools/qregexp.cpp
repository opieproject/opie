/****************************************************************************
** $Id: qregexp.cpp,v 1.1 2002-11-01 00:10:44 kergoth Exp $
**
** Implementation of QRegExp class
**
** Created : 950126
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the tools module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "qregexp.h"

#ifndef QT_NO_REGEXP

#include "qmemarray.h"
#include "qbitarray.h"
#include "qcache.h"
#include "qcleanuphandler.h"
#include "qintdict.h"
#include "qmap.h"
#include "qptrvector.h"
#include "qstring.h"
#include "qtl.h"

#ifdef QT_THREAD_SUPPORT
#include "qmutexpool_p.h"
#endif // QT_THREAD_SUPPORT

#undef QT_TRANSLATE_NOOP
#define QT_TRANSLATE_NOOP( context, sourceText ) sourceText

#include <limits.h>

// error strings for the regexp parser
#define RXERR_OK         QT_TRANSLATE_NOOP( "QRegExp", "no error occurred" )
#define RXERR_DISABLED   QT_TRANSLATE_NOOP( "QRegExp", "disabled feature used" )
#define RXERR_CHARCLASS  QT_TRANSLATE_NOOP( "QRegExp", "bad char class syntax" )
#define RXERR_LOOKAHEAD  QT_TRANSLATE_NOOP( "QRegExp", "bad lookahead syntax" )
#define RXERR_REPETITION QT_TRANSLATE_NOOP( "QRegExp", "bad repetition syntax" )
#define RXERR_OCTAL      QT_TRANSLATE_NOOP( "QRegExp", "invalid octal value" )
#define RXERR_LEFTDELIM  QT_TRANSLATE_NOOP( "QRegExp", "missing left delim" )
#define RXERR_END        QT_TRANSLATE_NOOP( "QRegExp", "unexpected end" )
#define RXERR_LIMIT      QT_TRANSLATE_NOOP( "QRegExp", "met internal limit" )

/*
  WARNING! Be sure to read qregexp.tex before modifying this file.
*/

/*!
    \class QRegExp qregexp.h
    \reentrant
    \brief The QRegExp class provides pattern matching using regular expressions.

    \ingroup tools
    \ingroup misc
    \ingroup shared
    \mainclass
    \keyword regular expression

    Regular expressions, or "regexps", provide a way to find patterns
    within text. This is useful in many contexts, for example:

    \table
    \row \i Validation
	 \i A regexp can be used to check whether a piece of text
	 meets some criteria, e.g. is an integer or contains no
	 whitespace.
    \row \i Searching
	 \i Regexps provide a much more powerful means of searching
	 text than simple string matching does. For example we can
	 create a regexp which says "find one of the words 'mail',
	 'letter' or 'correspondence' but not any of the words
	 'email', 'mailman' 'mailer', 'letterbox' etc."
    \row \i Search and Replace
	 \i A regexp can be used to replace a pattern with a piece of
	 text, for example replace all occurrences of '&' with
	 '\&amp;' except where the '&' is already followed by 'amp;'.
    \row \i String Splitting
	 \i A regexp can be used to identify where a string should be
	 split into its component fields, e.g. splitting tab-delimited
	 strings.
    \endtable

    We present a very brief introduction to regexps, a description of
    Qt's regexp language, some code examples, and finally the function
    documentation itself. QRegExp is modeled on Perl's regexp
    language, and also fully supports Unicode. QRegExp can also be
    used in the weaker 'wildcard' (globbing) mode which works in a
    similar way to command shells. A good text on regexps is \e
    {Mastering Regular Expressions: Powerful Techniques for Perl and
    Other Tools} by Jeffrey E. Friedl, ISBN 1565922573.

    Experienced regexp users may prefer to skip the introduction and
    go directly to the relevant information.

    \tableofcontents

    \section1 Introduction

    Regexps are built up from expressions, quantifiers, and assertions.
    The simplest form of expression is simply a character, e.g.
    <b>x</b> or <b>5</b>. An expression can also be a set of
    characters. For example, <b>[ABCD]</b>, will match an <b>A</b> or
    a <b>B</b> or a <b>C</b> or a <b>D</b>. As a shorthand we could
    write this as <b>[A-D]</b>. If we want to match any of the
    captital letters in the English alphabet we can write
    <b>[A-Z]</b>. A quantifier tells the regexp engine how many
    occurrences of the expression we want, e.g. <b>x{1,1}</b> means
    match an <b>x</b> which occurs at least once and at most once.
    We'll look at assertions and more complex expressions later.

    Note that in general regexps cannot be used to check for balanced
    brackets or tags. For example if you want to match an opening html
    \c <b> and its closing \c </b> you can only use a regexp if you
    know that these tags are not nested; the html fragment, \c{<b>bold
    <b>bolder</b></b>} will not match as expected. If you know the
    maximum level of nesting it is possible to create a regexp that
    will match correctly, but for an unknown level of nesting, regexps
    will fail.

    We'll start by writing a regexp to match integers in the range 0
    to 99. We will require at least one digit so we will start with
    <b>[0-9]{1,1}</b> which means match a digit exactly once. This
    regexp alone will match integers in the range 0 to 9. To match one
    or two digits we can increase the maximum number of occurrences so
    the regexp becomes <b>[0-9]{1,2}</b> meaning match a digit at
    least once and at most twice. However, this regexp as it stands
    will not match correctly. This regexp will match one or two digits
    \e within a string. To ensure that we match against the whole
    string we must use the anchor assertions. We need <b>^</b> (caret)
    which when it is the first character in the regexp means that the
    regexp must match from the beginning of the string. And we also
    need <b>$</b> (dollar) which when it is the last character in the
    regexp means that the regexp must match until the end of the
    string. So now our regexp is <b>^[0-9]{1,2}$</b>. Note that
    assertions, such as <b>^</b> and <b>$</b>, do not match any
    characters.

    If you've seen regexps elsewhere they may have looked different from
    the ones above. This is because some sets of characters and some
    quantifiers are so common that they have special symbols to
    represent them. <b>[0-9]</b> can be replaced with the symbol
    <b>\d</b>. The quantifier to match exactly one occurrence,
    <b>{1,1}</b>, can be replaced with the expression itself. This means
    that <b>x{1,1}</b> is exactly the same as <b>x</b> alone. So our 0
    to 99 matcher could be written <b>^\d{1,2}$</b>. Another way of
    writing it would be <b>^\d\d{0,1}$</b>, i.e. from the start of the
    string match a digit followed by zero or one digits. In practice
    most people would write it <b>^\d\d?$</b>. The <b>?</b> is a
    shorthand for the quantifier <b>{0,1}</b>, i.e. a minimum of no
    occurrences a maximum of one occurrence. This is used to make an
    expression optional. The regexp <b>^\d\d?$</b> means "from the
    beginning of the string match one digit followed by zero or one
    digits and then the end of the string".

    Our second example is matching the words 'mail', 'letter' or
    'correspondence' but without matching 'email', 'mailman',
    'mailer', 'letterbox' etc. We'll start by just matching 'mail'. In
    full the regexp is, <b>m{1,1}a{1,1}i{1,1}l{1,1}</b>, but since
    each expression itself is automatically quantified by <b>{1,1}</b>
    we can simply write this as <b>mail</b>; an 'm' followed by an 'a'
    followed by an 'i' followed by an 'l'. The symbol '|' (bar) is
    used for \e alternation, so our regexp now becomes
    <b>mail|letter|correspondence</b> which means match 'mail' \e or
    'letter' \e or 'correspondence'. Whilst this regexp will find the
    words we want it will also find words we don't want such as
    'email'. We will start by putting our regexp in parentheses,
    <b>(mail|letter|correspondence)</b>. Parentheses have two effects,
    firstly they group expressions together and secondly they identify
    parts of the regexp that we wish to \link #capturing-text capture
    \endlink. Our regexp still matches any of the three words but now
    they are grouped together as a unit. This is useful for building
    up more complex regexps. It is also useful because it allows us to
    examine which of the words actually matched. We need to use
    another assertion, this time <b>\b</b> "word boundary":
    <b>\b(mail|letter|correspondence)\b</b>. This regexp means "match
    a word boundary followed by the expression in parentheses followed
    by another word boundary". The <b>\b</b> assertion matches at a \e
    position in the regexp not a \e character in the regexp. A word
    boundary is any non-word character such as a space a newline or
    the beginning or end of the string.

    For our third example we want to replace ampersands with the HTML
    entity '\&amp;'. The regexp to match is simple: <b>\&</b>, i.e.
    match one ampersand. Unfortunately this will mess up our text if
    some of the ampersands have already been turned into HTML
    entities. So what we really want to say is replace an ampersand
    providing it is not followed by 'amp;'. For this we need the
    negative lookahead assertion and our regexp becomes:
    <b>\&(?!amp;)</b>. The negative lookahead assertion is introduced
    with '(?!' and finishes at the ')'. It means that the text it
    contains, 'amp;' in our example, must \e not follow the expression
    that preceeds it.

    Regexps provide a rich language that can be used in a variety of
    ways. For example suppose we want to count all the occurrences of
    'Eric' and 'Eirik' in a string. Two valid regexps to match these
    are <b>\\b(Eric|Eirik)\\b</b> and <b>\\bEi?ri[ck]\\b</b>. We need
    the word boundary '\b' so we don't get 'Ericsson' etc. The second
    regexp actually matches more than we want, 'Eric', 'Erik', 'Eiric'
    and 'Eirik'.

    We will implement some the examples above in the
    \link #code-examples code examples \endlink section.

    \target characters-and-abbreviations-for-sets-of-characters
    \section1 Characters and Abbreviations for Sets of Characters

    \table
    \header \i Element \i Meaning
    \row \i <b>c</b>
	 \i Any character represents itself unless it has a special
	 regexp meaning. Thus <b>c</b> matches the character \e c.
    \row \i <b>\\c</b>
	 \i A character that follows a backslash matches the character
	 itself except where mentioned below. For example if you
	 wished to match a literal caret at the beginning of a string
	 you would write <b>\^</b>.
    \row \i <b>\\a</b>
	 \i This matches the ASCII bell character (BEL, 0x07).
    \row \i <b>\\f</b>
	 \i This matches the ASCII form feed character (FF, 0x0C).
    \row \i <b>\\n</b>
	 \i This matches the ASCII line feed character (LF, 0x0A, Unix newline).
    \row \i <b>\\r</b>
	 \i This matches the ASCII carriage return character (CR, 0x0D).
    \row \i <b>\\t</b>
	 \i This matches the ASCII horizontal tab character (HT, 0x09).
    \row \i <b>\\v</b>
	 \i This matches the ASCII vertical tab character (VT, 0x0B).
    \row \i <b>\\xhhhh</b>
	 \i This matches the Unicode character corresponding to the
	 hexadecimal number hhhh (between 0x0000 and 0xFFFF). \0ooo
	 (i.e., \zero ooo) matches the ASCII/Latin-1 character
	 corresponding to the octal number ooo (between 0 and 0377).
    \row \i <b>. (dot)</b>
	 \i This matches any character (including newline).
    \row \i <b>\\d</b>
	 \i This matches a digit (see QChar::isDigit()).
    \row \i <b>\\D</b>
	 \i This matches a non-digit.
    \row \i <b>\\s</b>
	 \i This matches a whitespace (see QChar::isSpace()).
    \row \i <b>\\S</b>
	 \i This matches a non-whitespace.
    \row \i <b>\\w</b>
	 \i This matches a word character (see QChar::isLetterOrNumber()).
    \row \i <b>\\W</b>
	 \i This matches a non-word character.
    \row \i <b>\\n</b>
	 \i The n-th \link #capturing-text backreference \endlink,
	 e.g. \1, \2, etc.
    \endtable

    \e {Note that the C++ compiler transforms backslashes in strings
    so to include a <b>\\</b> in a regexp you will need to enter it
    twice, i.e. <b>\\\\</b>.}

    \target sets-of-characters
    \section1 Sets of Characters

    Square brackets are used to match any character in the set of
    characters contained within the square brackets. All the character
    set abbreviations described above can be used within square
    brackets. Apart from the character set abbreviations and the
    following two exceptions no characters have special meanings in
    square brackets.

    \table
    \row \i <b>^</b>
	 \i The caret negates the character set if it occurs as the
	 first character, i.e. immediately after the opening square
	 bracket. For example, <b>[abc]</b> matches 'a' or 'b' or 'c',
	 but <b>[^abc]</b> matches anything \e except 'a' or 'b' or
	 'c'.
    \row \i <b>-</b>
	 \i The dash is used to indicate a range of characters, for
	 example <b>[W-Z]</b> matches 'W' or 'X' or 'Y' or 'Z'.
    \endtable

    Using the predefined character set abbreviations is more portable
    than using character ranges across platforms and languages. For
    example, <b>[0-9]</b> matches a digit in Western alphabets but
    <b>\d</b> matches a digit in \e any alphabet.

    Note that in most regexp literature sets of characters are called
    "character classes".

    \target quantifiers
    \section1 Quantifiers

    By default an expression is automatically quantified by
    <b>{1,1}</b>, i.e. it should occur exactly once. In the following
    list <b>\e {E}</b> stands for any expression. An expression is a
    character or an abbreviation for a set of characters or a set of
    characters in square brackets or any parenthesised expression.

    \table
    \row \i <b>\e {E}?</b>
	 \i Matches zero or one occurrence of \e E. This quantifier
	 means "the previous expression is optional" since it will
	 match whether or not the expression occurs in the string. It
	 is the same as <b>\e {E}{0,1}</b>. For example <b>dents?</b>
	 will match 'dent' and 'dents'.

    \row \i <b>\e {E}+</b>
	 \i Matches one or more occurrences of \e E. This is the same
	 as <b>\e {E}{1,MAXINT}</b>. For example, <b>0+</b> will match
	 '0', '00', '000', etc.

    \row \i <b>\e {E}*</b>
	 \i Matches zero or more occurrences of \e E. This is the same
	 as <b>\e {E}{0,MAXINT}</b>. The <b>*</b> quantifier is often
	 used by a mistake. Since it matches \e zero or more
	 occurrences it will match no occurrences at all. For example
	 if we want to match strings that end in whitespace and use
	 the regexp <b>\s*$</b> we would get a match on every string.
	 This is because we have said find zero or more whitespace
	 followed by the end of string, so even strings that don't end
	 in whitespace will match. The regexp we want in this case is
	 <b>\s+$</b> to match strings that have at least one
	 whitespace at the end.

    \row \i <b>\e {E}{n}</b>
	 \i Matches exactly \e n occurrences of the expression. This
	 is the same as repeating the expression \e n times. For
	 example, <b>x{5}</b> is the same as <b>xxxxx</b>. It is also
	 the same as <b>\e {E}{n,n}</b>, e.g. <b>x{5,5}</b>.

    \row \i <b>\e {E}{n,}</b>
	 \i Matches at least \e n occurrences of the expression. This
	 is the same as <b>\e {E}{n,MAXINT}</b>.

    \row \i <b>\e {E}{,m}</b>
	 \i Matches at most \e m occurrences of the expression. This
	 is the same as <b>\e {E}{0,m}</b>.

    \row \i <b>\e {E}{n,m}</b>
	 \i Matches at least \e n occurrences of the expression and at
	 most \e m occurrences of the expression.
    \endtable

    (MAXINT is implementation dependent but will not be smaller than
    1024.)

    If we wish to apply a quantifier to more than just the preceding
    character we can use parentheses to group characters together in
    an expression. For example, <b>tag+</b> matches a 't' followed by
    an 'a' followed by at least one 'g', whereas <b>(tag)+</b> matches
    at least one occurrence of 'tag'.

    Note that quantifiers are "greedy". They will match as much text
    as they can. For example, <b>0+</b> will match as many zeros as it
    can from the first zero it finds, e.g. '2.<u>000</u>5'.
    Quantifiers can be made non-greedy, see setMinimal().

    \target capturing-text
    \section1 Capturing Text

    Parentheses allow us to group elements together so that we can
    quantify and capture them. For example if we have the expression
    <b>mail|letter|correspondence</b> that matches a string we know
    that \e one of the words matched but not which one. Using
    parentheses allows us to "capture" whatever is matched within
    their bounds, so if we used <b>(mail|letter|correspondence)</b>
    and matched this regexp against the string "I sent you some email"
    we can use the cap() or capturedTexts() functions to extract the
    matched characters, in this case 'mail'.

    We can use captured text within the regexp itself. To refer to the
    captured text we use \e backreferences which are indexed from 1,
    the same as for cap(). For example we could search for duplicate
    words in a string using <b>\b(\w+)\W+\1\b</b> which means match a
    word boundary followed by one or more word characters followed by
    one or more non-word characters followed by the same text as the
    first parenthesised expression followed by a word boundary.

    If we want to use parentheses purely for grouping and not for
    capturing we can use the non-capturing syntax, e.g.
    <b>(?:green|blue)</b>. Non-capturing parentheses begin '(?:' and
    end ')'. In this example we match either 'green' or 'blue' but we
    do not capture the match so we only know whether or not we matched
    but not which color we actually found. Using non-capturing
    parentheses is more efficient than using capturing parentheses
    since the regexp engine has to do less book-keeping.

    Both capturing and non-capturing parentheses may be nested.

    \target assertions
    \section1 Assertions

    Assertions make some statement about the text at the point where
    they occur in the regexp but they do not match any characters. In
    the following list <b>\e {E}</b> stands for any expression.

    \table
    \row \i <b>^</b>
	 \i The caret signifies the beginning of the string. If you
	 wish to match a literal \c{^} you must escape it by
	 writing \c{\\^}. For example, <b>^#include</b> will only
	 match strings which \e begin with the characters '#include'.
	 (When the caret is the first character of a character set it
	 has a special meaning, see \link #sets-of-characters Sets of
	 Characters \endlink.)

    \row \i <b>$</b>
	 \i The dollar signifies the end of the string. For example
	 <b>\d\s*$</b> will match strings which end with a digit
	 optionally followed by whitespace. If you wish to match a
	 literal \c{$} you must escape it by writing
	 \c{\\$}.

    \row \i <b>\\b</b>
	 \i A word boundary. For example the regexp
	 <b>\\bOK\\b</b> means match immediately after a word
	 boundary (e.g. start of string or whitespace) the letter 'O'
	 then the letter 'K' immediately before another word boundary
	 (e.g. end of string or whitespace). But note that the
	 assertion does not actually match any whitespace so if we
	 write <b>(\\bOK\\b)</b> and we have a match it will only
	 contain 'OK' even if the string is "Its <u>OK</u> now".

    \row \i <b>\\B</b>
	 \i A non-word boundary. This assertion is true wherever
	 <b>\\b</b> is false. For example if we searched for
	 <b>\\Bon\\B</b> in "Left on" the match would fail (space
	 and end of string aren't non-word boundaries), but it would
	 match in "t<u>on</u>ne".

    \row \i <b>(?=\e E)</b>
	 \i Positive lookahead. This assertion is true if the
	 expression matches at this point in the regexp. For example,
	 <b>const(?=\\s+char)</b> matches 'const' whenever it is
	 followed by 'char', as in 'static <u>const</u> char *'.
	 (Compare with <b>const\\s+char</b>, which matches 'static
	 <u>const char</u> *'.)

    \row \i <b>(?!\e E)</b>
	 \i Negative lookahead. This assertion is true if the
	 expression does not match at this point in the regexp. For
	 example, <b>const(?!\\s+char)</b> matches 'const' \e except
	 when it is followed by 'char'.
    \endtable

    \target wildcard-matching
    \section1 Wildcard Matching (globbing)

    Most command shells such as \e bash or \e cmd.exe support "file
    globbing", the ability to identify a group of files by using
    wildcards. The setWildcard() function is used to switch between
    regexp and wildcard mode. Wildcard matching is much simpler than
    full regexps and has only four features:

    \table
    \row \i <b>c</b>
	 \i Any character represents itself apart from those mentioned
	 below. Thus <b>c</b> matches the character \e c.
    \row \i <b>?</b>
	 \i This matches any single character. It is the same as
	 <b>.</b> in full regexps.
    \row \i <b>*</b>
	 \i This matches zero or more of any characters. It is the
	 same as <b>.*</b> in full regexps.
    \row \i <b>[...]</b>
	 \i Sets of characters can be represented in square brackets,
	 similar to full regexps. Within the character class, like
	 outside, backslash has no special meaning.
    \endtable

    For example if we are in wildcard mode and have strings which
    contain filenames we could identify HTML files with <b>*.html</b>.
    This will match zero or more characters followed by a dot followed
    by 'h', 't', 'm' and 'l'.

    \target perl-users
    \section1 Notes for Perl Users

    Most of the character class abbreviations supported by Perl are
    supported by QRegExp, see \link
    #characters-and-abbreviations-for-sets-of-characters characters
    and abbreviations for sets of characters \endlink.

    In QRegExp, apart from within character classes, \c{^} always
    signifies the start of the string, so carets must always be
    escaped unless used for that purpose. In Perl the meaning of caret
    varies automagically depending on where it occurs so escaping it
    is rarely necessary. The same applies to \c{$} which in
    QRegExp always signifies the end of the string.

    QRegExp's quantifiers are the same as Perl's greedy quantifiers.
    Non-greedy matching cannot be applied to individual quantifiers,
    but can be applied to all the quantifiers in the pattern. For
    example, to match the Perl regexp <b>ro+?m</b> requires:
    \code
    QRegExp rx( "ro+m" );
    rx.setMinimal( TRUE );
    \endcode

    The equivalent of Perl's \c{/i} option is
    setCaseSensitive(FALSE).

    Perl's \c{/g} option can be emulated using a \link
    #cap_in_a_loop loop \endlink.

    In QRegExp <b>.</b> matches any character, therefore all QRegExp
    regexps have the equivalent of Perl's \c{/s} option. QRegExp
    does not have an equivalent to Perl's \c{/m} option, but this
    can be emulated in various ways for example by splitting the input
    into lines or by looping with a regexp that searches for newlines.

    Because QRegExp is string oriented there are no \A, \Z or \z
    assertions. The \G assertion is not supported but can be emulated
    in a loop.

    Perl's $& is cap(0) or capturedTexts()[0]. There are no QRegExp
    equivalents for $`, $' or $+. Perl's capturing variables, $1, $2,
    ... correspond to cap(1) or capturedTexts()[1], cap(2) or
    capturedTexts()[2], etc.

    To substitute a pattern use QString::replace().

    Perl's extended \c{/x} syntax is not supported, nor are
    regexp comments (?#comment) or directives, e.g. (?i).

    Both zero-width positive and zero-width negative lookahead
    assertions (?=pattern) and (?!pattern) are supported with the same
    syntax as Perl. Perl's lookbehind assertions, "independent"
    subexpressions and conditional expressions are not supported.

    Non-capturing parentheses are also supported, with the same
    (?:pattern) syntax.

    See QStringList::split() and QStringList::join() for equivalents
    to Perl's split and join functions.

    Note: because C++ transforms \\'s they must be written \e twice in
    code, e.g. <b>\\b</b> must be written <b>\\\\b</b>.

    \target code-examples
    \section1 Code Examples

    \code
    QRegExp rx( "^\\d\\d?$" );  // match integers 0 to 99
    rx.search( "123" );         // returns -1 (no match)
    rx.search( "-6" );          // returns -1 (no match)
    rx.search( "6" );           // returns 0 (matched as position 0)
    \endcode

    The third string matches '<u>6</u>'. This is a simple validation
    regexp for integers in the range 0 to 99.

    \code
    QRegExp rx( "^\\S+$" );     // match strings without whitespace
    rx.search( "Hello world" ); // returns -1 (no match)
    rx.search( "This_is-OK" );  // returns 0 (matched at position 0)
    \endcode

    The second string matches '<u>This_is-OK</u>'. We've used the
    character set abbreviation '\S' (non-whitespace) and the anchors
    to match strings which contain no whitespace.

    In the following example we match strings containing 'mail' or
    'letter' or 'correspondence' but only match whole words i.e. not
    'email'

    \code
    QRegExp rx( "\\b(mail|letter|correspondence)\\b" );
    rx.search( "I sent you an email" );     // returns -1 (no match)
    rx.search( "Please write the letter" ); // returns 17
    \endcode

    The second string matches "Please write the <u>letter</u>". The
    word 'letter' is also captured (because of the parentheses). We
    can see what text we've captured like this:

    \code
    QString captured = rx.cap( 1 ); // captured == "letter"
    \endcode

    This will capture the text from the first set of capturing
    parentheses (counting capturing left parentheses from left to
    right). The parentheses are counted from 1 since cap( 0 ) is the
    whole matched regexp (equivalent to '&' in most regexp engines).

    \code
    QRegExp rx( "&(?!amp;)" );      // match ampersands but not &amp;
    QString line1 = "This & that";
    line1.replace( rx, "&amp;" );
    // line1 == "This &amp; that"
    QString line2 = "His &amp; hers & theirs";
    line2.replace( rx, "&amp;" );
    // line2 == "His &amp; hers &amp; theirs"
    \endcode

    Here we've passed the QRegExp to QString's replace() function to
    replace the matched text with new text.

    \code
    QString str = "One Eric another Eirik, and an Ericsson."
		    " How many Eiriks, Eric?";
    QRegExp rx( "\\b(Eric|Eirik)\\b" ); // match Eric or Eirik
    int pos = 0;    // where we are in the string
    int count = 0;  // how many Eric and Eirik's we've counted
    while ( pos >= 0 ) {
	pos = rx.search( str, pos );
	if ( pos >= 0 ) {
	    pos++;      // move along in str
	    count++;    // count our Eric or Eirik
	}
    }
    \endcode

    We've used the search() function to repeatedly match the regexp in
    the string. Note that instead of moving forward by one character
    at a time \c pos++ we could have written \c {pos +=
    rx.matchedLength()} to skip over the already matched string. The
    count will equal 3, matching 'One <u>Eric</u> another
    <u>Eirik</u>, and an Ericsson. How many Eiriks, <u>Eric</u>?'; it
    doesn't match 'Ericsson' or 'Eiriks' because they are not bounded
    by non-word boundaries.

    One common use of regexps is to split lines of delimited data into
    their component fields.

    \code
    str = "Trolltech AS\twww.trolltech.com\tNorway";
    QString company, web, country;
    rx.setPattern( "^([^\t]+)\t([^\t]+)\t([^\t]+)$" );
    if ( rx.search( str ) != -1 ) {
	company = rx.cap( 1 );
	web = rx.cap( 2 );
	country = rx.cap( 3 );
    }
    \endcode

    In this example our input lines have the format company name, web
    address and country. Unfortunately the regexp is rather long and
    not very versatile -- the code will break if we add any more
    fields. A simpler and better solution is to look for the
    separator, '\t' in this case, and take the surrounding text. The
    QStringList split() function can take a separator string or regexp
    as an argument and split a string accordingly.

    \code
    QStringList field = QStringList::split( "\t", str );
    \endcode

    Here field[0] is the company, field[1] the web address and so on.

    To imitate the matching of a shell we can use wildcard mode.

    \code
    QRegExp rx( "*.html" );     // invalid regexp: * doesn't quantify anything
    rx.setWildcard( TRUE );     // now it's a valid wildcard regexp
    rx.search( "index.html" );  // returns 0 (matched at position 0)
    rx.search( "default.htm" ); // returns -1 (no match)
    rx.search( "readme.txt" );  // returns -1 (no match)
    \endcode

    Wildcard matching can be convenient because of its simplicity, but
    any wildcard regexp can be defined using full regexps, e.g.
    <b>.*\.html$</b>. Notice that we can't match both \c .html and \c
    .htm files with a wildcard unless we use <b>*.htm*</b> which will
    also match 'test.html.bak'. A full regexp gives us the precision
    we need, <b>.*\\.html?$</b>.

    QRegExp can match case insensitively using setCaseSensitive(), and
    can use non-greedy matching, see setMinimal(). By default QRegExp
    uses full regexps but this can be changed with setWildcard().
    Searching can be forward with search() or backward with
    searchRev(). Captured text can be accessed using capturedTexts()
    which returns a string list of all captured strings, or using
    cap() which returns the captured string for the given index. The
    pos() function takes a match index and returns the position in the
    string where the match was made (or -1 if there was no match).

    \sa QRegExpValidator QString QStringList

    \target member-function-documentation
*/

const int NumBadChars = 64;
#define BadChar( ch ) ( (ch).unicode() % NumBadChars )

const int NoOccurrence = INT_MAX;
const int EmptyCapture = INT_MAX;
const int InftyLen = INT_MAX;
const int InftyRep = 1025;
const int EOS = -1;

/*
  Merges two QMemArrays of ints and puts the result into the first one.
*/
static void mergeInto( QMemArray<int> *a, const QMemArray<int>& b )
{
    int asize = a->size();
    int bsize = b.size();
    if ( asize == 0 ) {
	*a = b.copy();
#ifndef QT_NO_REGEXP_OPTIM
    } else if ( bsize == 1 && (*a)[asize - 1] < b[0] ) {
	a->resize( asize + 1 );
	(*a)[asize] = b[0];
#endif
    } else if ( bsize >= 1 ) {
	int csize = asize + bsize;
	QMemArray<int> c( csize );
	int i = 0, j = 0, k = 0;
	while ( i < asize ) {
	    if ( j < bsize ) {
		if ( (*a)[i] == b[j] ) {
		    i++;
		    csize--;
		} else if ( (*a)[i] < b[j] ) {
		    c[k++] = (*a)[i++];
		} else {
		    c[k++] = b[j++];
		}
	    } else {
		memcpy( c.data() + k, (*a).data() + i,
			(asize - i) * sizeof(int) );
		break;
	    }
	}
	c.resize( csize );
	if ( j < bsize )
	    memcpy( c.data() + k, b.data() + j, (bsize - j) * sizeof(int) );
	*a = c;
    }
}

/*
  Merges two disjoint QMaps of (int, int) pairs and puts the result into the
  first one.
*/
static void mergeInto( QMap<int, int> *a, const QMap<int, int>& b )
{
    QMap<int, int>::ConstIterator it;
    for ( it = b.begin(); it != b.end(); ++it )
	a->insert( it.key(), *it );
}

/*
  Returns the value associated to key k in QMap m of (int, int) pairs, or 0 if
  no such value is explicitly present.
*/
static int at( const QMap<int, int>& m, int k )
{
    QMap<int, int>::ConstIterator it = m.find( k );
    if ( it == m.end() )
	return 0;
    else
	return *it;
}

#ifndef QT_NO_REGEXP_WILDCARD
/*
  Translates a wildcard pattern to an equivalent regular expression pattern
  (e.g., *.cpp to .*\.cpp).
*/
static QString wc2rx( const QString& wc_str )
{
    int wclen = wc_str.length();
    QString rx = QString::fromLatin1( "" );
    int i = 0;
    const QChar *wc = wc_str.unicode();
    while ( i < wclen ) {
	QChar c = wc[i++];
	switch ( c.unicode() ) {
	case '*':
	    rx += QString::fromLatin1( ".*" );
	    break;
	case '?':
	    rx += QChar( '.' );
	    break;
	case '$':
	case '(':
	case ')':
	case '+':
	case '.':
	case '\\':
	case '^':
	case '{':
	case '|':
	case '}':
	    rx += QChar( '\\' );
	    rx += c;
	    break;
	case '[':
	    rx += c;
	    if ( wc[i] == QChar('^') )
		rx += wc[i++];
	    if ( i < wclen ) {
		if ( rx[i] == ']' )
		    rx += wc[i++];
		while ( i < wclen && wc[i] != QChar(']') ) {
		    if ( wc[i] == '\\' )
			rx += QChar( '\\' );
		    rx += wc[i++];
		}
	    }
	    break;
	default:
	    rx += c;
	}
    }
    return rx;
}
#endif

/*
  The class QRegExpEngine encapsulates a modified nondeterministic
  finite automaton (NFA).
*/
class QRegExpEngine : public QShared
{
public:
#ifndef QT_NO_REGEXP_CCLASS
    /*
      The class CharClass represents a set of characters, such as can
      be found in regular expressions (e.g., [a-z] denotes the set
      {a, b, ..., z}).
    */
    class CharClass
    {
    public:
	CharClass();
	CharClass( const CharClass& cc ) { operator=( cc ); }

	CharClass& operator=( const CharClass& cc );

	void clear();
	bool negative() const { return n; }
	void setNegative( bool negative );
	void addCategories( int cats );
	void addRange( ushort from, ushort to );
	void addSingleton( ushort ch ) { addRange( ch, ch ); }

	bool in( QChar ch ) const;
#ifndef QT_NO_REGEXP_OPTIM
	const QMemArray<int>& firstOccurrence() const { return occ1; }
#endif

#if defined(QT_DEBUG)
	void dump() const;
#endif

    private:
	/*
	  The struct Range represents a range of characters (e.g.,
	  [0-9] denotes range 48 to 57).
	*/
	struct Range
	{
	    ushort from; // 48
	    ushort to; // 57
	};

	int c; // character classes
	QMemArray<Range> r; // character ranges
	bool n; // negative?
#ifndef QT_NO_REGEXP_OPTIM
	QMemArray<int> occ1; // first-occurrence array
#endif
    };
#else
    struct CharClass
    {
	int dummy;

#ifndef QT_NO_REGEXP_OPTIM
	CharClass() { occ1.fill( 0, NumBadChars ); }

	const QMemArray<int>& firstOccurrence() const { return occ1; }
	QMemArray<int> occ1;
#endif
    };
#endif

    QRegExpEngine( bool caseSensitive ) { setup( caseSensitive ); }
    QRegExpEngine( const QString& rx, bool caseSensitive );
#ifndef QT_NO_REGEXP_OPTIM
    ~QRegExpEngine();
#endif

    bool isValid() const { return valid; }
    const QString& errorString() const { return yyError; }
    bool caseSensitive() const { return cs; }
    int numCaptures() const { return officialncap; }
    QMemArray<int> match( const QString& str, int pos, bool minimal,
			  bool oneTest, int caretIndex );
    int matchedLength() const { return mmMatchedLen; }

    int createState( QChar ch );
    int createState( const CharClass& cc );
#ifndef QT_NO_REGEXP_BACKREF
    int createState( int bref );
#endif

    void addCatTransitions( const QMemArray<int>& from,
			    const QMemArray<int>& to );
#ifndef QT_NO_REGEXP_CAPTURE
    void addPlusTransitions( const QMemArray<int>& from,
			     const QMemArray<int>& to, int atom );
#endif

#ifndef QT_NO_REGEXP_ANCHOR_ALT
    int anchorAlternation( int a, int b );
    int anchorConcatenation( int a, int b );
#else
    int anchorAlternation( int a, int b ) { return a & b; }
    int anchorConcatenation( int a, int b ) { return a | b; }
#endif
    void addAnchors( int from, int to, int a );

#ifndef QT_NO_REGEXP_OPTIM
    void setupGoodStringHeuristic( int earlyStart, int lateStart,
				   const QString& str );
    void setupBadCharHeuristic( int minLen, const QMemArray<int>& firstOcc );
    void heuristicallyChooseHeuristic();
#endif

#if defined(QT_DEBUG)
    void dump() const;
#endif

private:
    enum { CharClassBit = 0x10000, BackRefBit = 0x20000 };

    /*
      The struct State represents one state in a modified NFA. The
      input characters matched are stored in the state instead of on
      the transitions, something possible for an automaton
      constructed from a regular expression.
    */
    struct State
    {
#ifndef QT_NO_REGEXP_CAPTURE
	int atom; // which atom does this state belong to?
#endif
	int match; // what does it match? (see CharClassBit and BackRefBit)
	QMemArray<int> outs; // out-transitions
	QMap<int, int> *reenter; // atoms reentered when transiting out
	QMap<int, int> *anchors; // anchors met when transiting out

#ifndef QT_NO_REGEXP_CAPTURE
	State( int a, int m )
	    : atom( a ), match( m ), reenter( 0 ), anchors( 0 ) { }
#else
	State( int m )
	    : match( m ), reenter( 0 ), anchors( 0 ) { }
#endif
	~State() { delete reenter; delete anchors; }
    };

#ifndef QT_NO_REGEXP_LOOKAHEAD
    /*
      The struct Lookahead represents a lookahead a la Perl (e.g., (?=foo) and
      (?!bar)).
    */
    struct Lookahead
    {
	QRegExpEngine *eng; // NFA representing the embedded regular expression
	bool neg; // negative lookahead?

	Lookahead( QRegExpEngine *eng0, bool neg0 )
	    : eng( eng0 ), neg( neg0 ) { }
	~Lookahead() { delete eng; }
    };
#endif

#ifndef QT_NO_REGEXP_CAPTURE
    /*
      The struct Atom represents one node in the hierarchy of regular
      expression atoms.
    */
    struct Atom
    {
	int parent; // index of parent in array of atoms
	int capture; // index of capture, from 1 to ncap
    };
#endif

#ifndef QT_NO_REGEXP_ANCHOR_ALT
    /*
      The struct AnchorAlternation represents a pair of anchors with
      OR semantics.
    */
    struct AnchorAlternation
    {
	int a; // this anchor...
	int b; // ...or this one
    };
#endif

    enum { InitialState = 0, FinalState = 1 };
    void setup( bool caseSensitive );
    int setupState( int match );

    /*
      Let's hope that 13 lookaheads and 14 back-references are
      enough.
     */
    enum { MaxLookaheads = 13, MaxBackRefs = 14 };
    enum { Anchor_Dollar = 0x00000001, Anchor_Caret = 0x00000002,
	   Anchor_Word = 0x00000004, Anchor_NonWord = 0x00000008,
	   Anchor_FirstLookahead = 0x00000010,
	   Anchor_BackRef1Empty = Anchor_FirstLookahead << MaxLookaheads,
	   Anchor_BackRef0Empty = Anchor_BackRef1Empty >> 1,
	   Anchor_Alternation = Anchor_BackRef1Empty << MaxBackRefs,

	   Anchor_LookaheadMask = ( Anchor_FirstLookahead - 1 ) ^
		   ( (Anchor_FirstLookahead << MaxLookaheads) - 1 ) };
#ifndef QT_NO_REGEXP_CAPTURE
    int startAtom( bool capture );
    void finishAtom( int atom ) { cf = f[atom].parent; }
#endif

#ifndef QT_NO_REGEXP_LOOKAHEAD
    int addLookahead( QRegExpEngine *eng, bool negative );
#endif

#ifndef QT_NO_REGEXP_CAPTURE
    bool isBetterCapture( const int *begin1, const int *end1, const int *begin2,
			  const int *end2 );
#endif
    bool testAnchor( int i, int a, const int *capBegin );

#ifndef QT_NO_REGEXP_OPTIM
    bool goodStringMatch();
    bool badCharMatch();
#else
    bool bruteMatch();
#endif
    bool matchHere();

    QPtrVector<State> s; // array of states
    int ns; // number of states
#ifndef QT_NO_REGEXP_CAPTURE
    QMemArray<Atom> f; // atom hierarchy
    int nf; // number of atoms
    int cf; // current atom
#endif
    int officialncap; // number of captures, seen from the outside
    int ncap; // number of captures, seen from the inside
#ifndef QT_NO_REGEXP_CCLASS
    QPtrVector<CharClass> cl; // array of character classes
#endif
#ifndef QT_NO_REGEXP_LOOKAHEAD
    QPtrVector<Lookahead> ahead; // array of lookaheads
#endif
#ifndef QT_NO_REGEXP_ANCHOR_ALT
    QMemArray<AnchorAlternation> aa; // array of (a, b) pairs of anchors
#endif
#ifndef QT_NO_REGEXP_OPTIM
    bool caretAnchored; // does the regexp start with ^?
#endif
    bool valid; // is the regular expression valid?
    bool cs; // case sensitive?
#ifndef QT_NO_REGEXP_BACKREF
    int nbrefs; // number of back-references
#endif

#ifndef QT_NO_REGEXP_OPTIM
    bool useGoodStringHeuristic; // use goodStringMatch? otherwise badCharMatch

    int goodEarlyStart; // the index where goodStr can first occur in a match
    int goodLateStart; // the index where goodStr can last occur in a match
    QString goodStr; // the string that any match has to contain

    int minl; // the minimum length of a match
    QMemArray<int> occ1; // first-occurrence array
#endif

    /*
      The class Box is an abstraction for a regular expression
      fragment. It can also be seen as one node in the syntax tree of
      a regular expression with synthetized attributes.

      It's interface is ugly for performance reasons.
    */
    class Box
    {
    public:
	Box( QRegExpEngine *engine );
	Box( const Box& b ) { operator=( b ); }

	Box& operator=( const Box& b );

	void clear() { operator=(Box(eng)); }
	void set( QChar ch );
	void set( const CharClass& cc );
#ifndef QT_NO_REGEXP_BACKREF
	void set( int bref );
#endif

	void cat( const Box& b );
	void orx( const Box& b );
	void plus( int atom );
	void opt();
	void catAnchor( int a );
#ifndef QT_NO_REGEXP_OPTIM
	void setupHeuristics();
#endif

#if defined(QT_DEBUG)
	void dump() const;
#endif

    private:
	void addAnchorsToEngine( const Box& to ) const;

	QRegExpEngine *eng; // the automaton under construction
	QMemArray<int> ls; // the left states (firstpos)
	QMemArray<int> rs; // the right states (lastpos)
	QMap<int, int> lanchors; // the left anchors
	QMap<int, int> ranchors; // the right anchors
	int skipanchors; // the anchors to match if the box is skipped

#ifndef QT_NO_REGEXP_OPTIM
	int earlyStart; // the index where str can first occur
	int lateStart; // the index where str can last occur
	QString str; // a string that has to occur in any match
	QString leftStr; // a string occurring at the left of this box
	QString rightStr; // a string occurring at the right of this box
	int maxl; // the maximum length of this box (possibly InftyLen)
#endif

	int minl; // the minimum length of this box
#ifndef QT_NO_REGEXP_OPTIM
	QMemArray<int> occ1; // first-occurrence array
#endif
    };
    friend class Box;

    /*
      This is the lexical analyzer for regular expressions.
    */
    enum { Tok_Eos, Tok_Dollar, Tok_LeftParen, Tok_MagicLeftParen,
	   Tok_PosLookahead, Tok_NegLookahead, Tok_RightParen, Tok_CharClass,
	   Tok_Caret, Tok_Quantifier, Tok_Bar, Tok_Word, Tok_NonWord,
	   Tok_Char = 0x10000, Tok_BackRef = 0x20000 };
    int getChar();
    int getEscape();
#ifndef QT_NO_REGEXP_INTERVAL
    int getRep( int def );
#endif
#ifndef QT_NO_REGEXP_LOOKAHEAD
    void skipChars( int n );
#endif
    void error( const char *msg );
    void startTokenizer( const QChar *rx, int len );
    int getToken();

    const QChar *yyIn; // a pointer to the input regular expression pattern
    int yyPos0; // the position of yyTok in the input pattern
    int yyPos; // the position of the next character to read
    int yyLen; // the length of yyIn
    int yyCh; // the last character read
    CharClass *yyCharClass; // attribute for Tok_CharClass tokens
    int yyMinRep; // attribute for Tok_Quantifier
    int yyMaxRep; // ditto
    QString yyError; // syntax error or overflow during parsing?

    /*
      This is the syntactic analyzer for regular expressions.
    */
    int parse( const QChar *rx, int len );
    void parseAtom( Box *box );
    void parseFactor( Box *box );
    void parseTerm( Box *box );
    void parseExpression( Box *box );

    int yyTok; // the last token read
    bool yyMayCapture; // set this to FALSE to disable capturing

    /*
      This is the engine state during matching.
    */
    const QString *mmStr; // a pointer to the input QString
    const QChar *mmIn; // a pointer to the input string data
    int mmPos; // the current position in the string
    int mmCaretPos;
    int mmLen; // the length of the input string
    bool mmMinimal; // minimal matching?
    QMemArray<int> mmCaptured; // an array of pairs (start, len)
    QMemArray<int> mmCapturedNoMatch; // an array of pairs (-1, -1)
    QMemArray<int> mmBigArray; // big QMemArray<int> array
    int *mmInNextStack; // is state is mmNextStack?
    int *mmCurStack; // stack of current states
    int *mmNextStack; // stack of next states
    int *mmCurCapBegin; // start of current states' captures
    int *mmNextCapBegin; // start of next states' captures
    int *mmCurCapEnd; // end of current states' captures
    int *mmNextCapEnd; // end of next states' captures
    int *mmTempCapBegin; // start of temporary captures
    int *mmTempCapEnd; // end of temporary captures
    int *mmCapBegin; // start of captures for a next state
    int *mmCapEnd; // end of captures for a next state
    int *mmSlideTab; // bump-along slide table for bad-character heuristic
    int mmSlideTabSize; // size of slide table
#ifndef QT_NO_REGEXP_BACKREF
    QIntDict<int> mmSleeping; // dictionary of back-reference sleepers
#endif
    int mmMatchLen; // length of match
    int mmMatchedLen; // length of partial match
};

QRegExpEngine::QRegExpEngine( const QString& rx, bool caseSensitive )
#ifndef QT_NO_REGEXP_BACKREF
    : mmSleeping( 101 )
#endif
{
    setup( caseSensitive );
    valid = ( parse(rx.unicode(), rx.length()) == (int) rx.length() );
    if ( !valid )
	error( RXERR_LEFTDELIM );
}

#ifndef QT_NO_REGEXP_OPTIM
QRegExpEngine::~QRegExpEngine()
{
}
#endif

/*
  Tries to match in str and returns an array of (begin, length) pairs
  for captured text. If there is no match, all pairs are (-1, -1).
*/
QMemArray<int> QRegExpEngine::match( const QString& str, int pos, bool minimal,
				     bool oneTest, int caretIndex )
{
    mmStr = &str;
    mmIn = str.unicode();
    if ( mmIn == 0 )
	mmIn = &QChar::null;
    mmPos = pos;
    mmCaretPos = caretIndex;
    mmLen = str.length();
    mmMinimal = minimal;
    mmMatchLen = 0;
    mmMatchedLen = 0;

    bool matched = FALSE;
    if ( valid && mmPos >= 0 && mmPos <= mmLen ) {
#ifndef QT_NO_REGEXP_OPTIM
	if ( oneTest ) {
	    matched = matchHere();
	} else {
	    if ( mmPos <= mmLen - minl ) {
		if ( caretAnchored ) {
		    matched = matchHere();
		} else if ( useGoodStringHeuristic ) {
		    matched = goodStringMatch();
		} else {
		    matched = badCharMatch();
		}
	    }
	}
#else
	matched = oneTest ? matchHere() : bruteMatch();
#endif
    }

    if ( matched ) {
	mmCaptured.detach();
	mmCaptured[0] = mmPos;
	mmCaptured[1] = mmMatchLen;
	for ( int j = 0; j < officialncap; j++ ) {
	    int len = mmCapEnd[j] - mmCapBegin[j];
	    mmCaptured[2 + 2 * j] = len > 0 ? mmPos + mmCapBegin[j] : 0;
	    mmCaptured[2 + 2 * j + 1] = len;
	}
	return mmCaptured;
    } else {
	return mmCapturedNoMatch;
    }
}

/*
  The three following functions add one state to the automaton and
  return the number of the state.
*/

int QRegExpEngine::createState( QChar ch )
{
    return setupState( ch.unicode() );
}

int QRegExpEngine::createState( const CharClass& cc )
{
#ifndef QT_NO_REGEXP_CCLASS
    int n = cl.size();
    cl.resize( n + 1 );
    cl.insert( n, new CharClass(cc) );
    return setupState( CharClassBit | n );
#else
    Q_UNUSED( cc );
    return setupState( CharClassBit );
#endif
}

#ifndef QT_NO_REGEXP_BACKREF
int QRegExpEngine::createState( int bref )
{
    if ( bref > nbrefs ) {
	nbrefs = bref;
	if ( nbrefs > MaxBackRefs ) {
	    error( RXERR_LIMIT );
	    return 0;
	}
    }
    return setupState( BackRefBit | bref );
}
#endif

/*
  The two following functions add a transition between all pairs of
  states (i, j) where i is fond in from, and j is found in to.

  Cat-transitions are distinguished from plus-transitions for
  capturing.
*/

void QRegExpEngine::addCatTransitions( const QMemArray<int>& from,
				       const QMemArray<int>& to )
{
    for ( int i = 0; i < (int) from.size(); i++ ) {
	State *st = s[from[i]];
	mergeInto( &st->outs, to );
    }
}

#ifndef QT_NO_REGEXP_CAPTURE
void QRegExpEngine::addPlusTransitions( const QMemArray<int>& from,
					const QMemArray<int>& to, int atom )
{
    for ( int i = 0; i < (int) from.size(); i++ ) {
	State *st = s[from[i]];
	QMemArray<int> oldOuts = st->outs.copy();
	mergeInto( &st->outs, to );
	if ( f[atom].capture >= 0 ) {
	    if ( st->reenter == 0 )
		st->reenter = new QMap<int, int>;
	    for ( int j = 0; j < (int) to.size(); j++ ) {
		if ( !st->reenter->contains(to[j]) &&
		     oldOuts.bsearch(to[j]) < 0 )
		    st->reenter->insert( to[j], atom );
	    }
	}
    }
}
#endif

#ifndef QT_NO_REGEXP_ANCHOR_ALT
/*
  Returns an anchor that means a OR b.
*/
int QRegExpEngine::anchorAlternation( int a, int b )
{
    if ( ((a & b) == a || (a & b) == b) && ((a | b) & Anchor_Alternation) == 0 )
	return a & b;

    int n = aa.size();
#ifndef QT_NO_REGEXP_OPTIM
    if ( n > 0 && aa[n - 1].a == a && aa[n - 1].b == b )
	return Anchor_Alternation | ( n - 1 );
#endif

    aa.resize( n + 1 );
    aa[n].a = a;
    aa[n].b = b;
    return Anchor_Alternation | n;
}

/*
  Returns an anchor that means a AND b.
*/
int QRegExpEngine::anchorConcatenation( int a, int b )
{
    if ( ((a | b) & Anchor_Alternation) == 0 )
	return a | b;
    if ( (b & Anchor_Alternation) != 0 )
	qSwap( a, b );

    int aprime = anchorConcatenation( aa[a ^ Anchor_Alternation].a, b );
    int bprime = anchorConcatenation( aa[a ^ Anchor_Alternation].b, b );
    return anchorAlternation( aprime, bprime );
}
#endif

/*
  Adds anchor a on a transition caracterised by its from state and
  its to state.
*/
void QRegExpEngine::addAnchors( int from, int to, int a )
{
    State *st = s[from];
    if ( st->anchors == 0 )
	st->anchors = new QMap<int, int>;
    if ( st->anchors->contains(to) )
	a = anchorAlternation( (*st->anchors)[to], a );
    st->anchors->insert( to, a );
}

#ifndef QT_NO_REGEXP_OPTIM
/*
  The two following functions provide the engine with the information
  needed by its matching heuristics.
*/

void QRegExpEngine::setupGoodStringHeuristic( int earlyStart, int lateStart,
					      const QString& str )
{
    goodEarlyStart = earlyStart;
    goodLateStart = lateStart;
    goodStr = cs ? str : str.lower();
}

void QRegExpEngine::setupBadCharHeuristic( int minLen,
					   const QMemArray<int>& firstOcc )
{
    minl = minLen;
    if ( cs ) {
	occ1 = firstOcc;
    } else {
	occ1.fill( 0, NumBadChars );
    }
}

/*
  This function chooses between the good-string and the bad-character
  heuristics. It computes two scores and chooses the heuristic with
  the highest score.

  Here are some common-sense constraints on the scores that should be
  respected if the formulas are ever modified: (1) If goodStr is
  empty, the good-string heuristic scores 0. (2) If the search is
  case insensitive, the good-string heuristic should be used, unless
  it scores 0. (Case insensitivity turns all entries of occ1 to 0.)
  (3) If (goodLateStart - goodEarlyStart) is big, the good-string
  heuristic should score less.
*/
void QRegExpEngine::heuristicallyChooseHeuristic()
{
    int i;

    if ( minl == 0 )
	return;

    /*
      Magic formula:  The good string has to constitute a good
      proportion of the minimum-length string, and appear at a
      more-or-less known index.
    */
    int goodStringScore = ( 64 * goodStr.length() / minl ) -
			  ( goodLateStart - goodEarlyStart );

    /*
      Less magic formula:  We pick a couple of characters at random,
      and check whether they are good or bad.
    */
    int badCharScore = 0;
    int step = QMAX( 1, NumBadChars / 32 );
    for ( i = 1; i < NumBadChars; i += step ) {
	if ( occ1[i] == NoOccurrence )
	    badCharScore += minl;
	else
	    badCharScore += occ1[i];
    }
    badCharScore /= minl;

    useGoodStringHeuristic = ( goodStringScore > badCharScore );
}
#endif

#if defined(QT_DEBUG)
void QRegExpEngine::dump() const
{
    int i, j;
    qDebug( "Case %ssensitive engine", cs ? "" : "in" );
    qDebug( "  States" );
    for ( i = 0; i < ns; i++ ) {
	qDebug( "  %d%s", i,
		i == InitialState ? " (initial)" :
		i == FinalState ? " (final)" : "" );
#ifndef QT_NO_REGEXP_CAPTURE
	qDebug( "    in atom %d", s[i]->atom );
#endif
	int m = s[i]->match;
	if ( (m & CharClassBit) != 0 ) {
	    qDebug( "    match character class %d", m ^ CharClassBit );
#ifndef QT_NO_REGEXP_CCLASS
	    cl[m ^ CharClassBit]->dump();
#else
	    qDebug( "    negative character class" );
#endif
	} else if ( (m & BackRefBit) != 0 ) {
	    qDebug( "    match back-reference %d", m ^ BackRefBit );
	} else if ( m >= 0x20 && m <= 0x7e ) {
	    qDebug( "    match 0x%.4x (%c)", m, m );
	} else {
	    qDebug( "    match 0x%.4x", m );
	}
	for ( j = 0; j < (int) s[i]->outs.size(); j++ ) {
	    int next = s[i]->outs[j];
	    qDebug( "    -> %d", next );
	    if ( s[i]->reenter != 0 && s[i]->reenter->contains(next) )
		qDebug( "       [reenter %d]", (*s[i]->reenter)[next] );
	    if ( s[i]->anchors != 0 && at(*s[i]->anchors, next) != 0 )
		qDebug( "       [anchors 0x%.8x]", (*s[i]->anchors)[next] );
	}
    }
#ifndef QT_NO_REGEXP_CAPTURE
    if ( nf > 0 ) {
	qDebug( "  Atom    Parent  Capture" );
	for ( i = 0; i < nf; i++ )
	    qDebug( "  %6d  %6d  %6d", i, f[i].parent, f[i].capture );
    }
#endif
#ifndef QT_NO_REGEXP_ANCHOR_ALT
    for ( i = 0; i < (int) aa.size(); i++ )
	qDebug( "  Anchor alternation 0x%.8x: 0x%.8x 0x%.9x", i, aa[i].a,
		aa[i].b );
#endif
}
#endif

void QRegExpEngine::setup( bool caseSensitive )
{
    s.setAutoDelete( TRUE );
    s.resize( 32 );
    ns = 0;
#ifndef QT_NO_REGEXP_CAPTURE
    f.resize( 32 );
    nf = 0;
    cf = -1;
#endif
    officialncap = 0;
    ncap = 0;
#ifndef QT_NO_REGEXP_CCLASS
    cl.setAutoDelete( TRUE );
#endif
#ifndef QT_NO_REGEXP_LOOKAHEAD
    ahead.setAutoDelete( TRUE );
#endif
#ifndef QT_NO_REGEXP_OPTIM
    caretAnchored = TRUE;
#endif
    valid = FALSE;
    cs = caseSensitive;
#ifndef QT_NO_REGEXP_BACKREF
    nbrefs = 0;
#endif
#ifndef QT_NO_REGEXP_OPTIM
    useGoodStringHeuristic = FALSE;
    minl = 0;
    occ1.fill( 0, NumBadChars );
#endif
    mmCapturedNoMatch.fill( -1, 2 );
}

int QRegExpEngine::setupState( int match )
{
    if ( (ns & (ns + 1)) == 0 && ns + 1 >= (int) s.size() )
	s.resize( (ns + 1) << 1 );
#ifndef QT_NO_REGEXP_CAPTURE
    s.insert( ns, new State(cf, match) );
#else
    s.insert( ns, new State(match) );
#endif
    return ns++;
}

#ifndef QT_NO_REGEXP_CAPTURE
/*
  Functions startAtom() and finishAtom() should be called to delimit
  atoms. When a state is created, it is assigned to the current atom.
  The information is later used for capturing.
*/
int QRegExpEngine::startAtom( bool capture )
{
    if ( (nf & (nf + 1)) == 0 && nf + 1 >= (int) f.size() )
	f.resize( (nf + 1) << 1 );
    f[nf].parent = cf;
    cf = nf++;
    f[cf].capture = capture ? ncap++ : -1;
    return cf;
}
#endif

#ifndef QT_NO_REGEXP_LOOKAHEAD
/*
  Creates a lookahead anchor.
*/
int QRegExpEngine::addLookahead( QRegExpEngine *eng, bool negative )
{
    int n = ahead.size();
    if ( n == MaxLookaheads ) {
	error( RXERR_LIMIT );
	return 0;
    }
    ahead.resize( n + 1 );
    ahead.insert( n, new Lookahead(eng, negative) );
    return Anchor_FirstLookahead << n;
}
#endif

#ifndef QT_NO_REGEXP_CAPTURE
/*
  We want the longest leftmost captures.
*/
bool QRegExpEngine::isBetterCapture( const int *begin1, const int *end1,
				     const int *begin2, const int *end2 )
{
    for ( int i = 0; i < ncap; i++ ) {
	int delta = begin2[i] - begin1[i]; // it has to start early...
	if ( delta == 0 )
	    delta = end1[i] - end2[i]; // ...and end late (like a party)

	if ( delta != 0 )
	    return delta > 0;
    }
    return FALSE;
}
#endif

/*
  Returns TRUE if anchor a matches at position mmPos + i in the input
  string, otherwise FALSE.
*/
bool QRegExpEngine::testAnchor( int i, int a, const int *capBegin )
{
    int j;

#ifndef QT_NO_REGEXP_ANCHOR_ALT
    if ( (a & Anchor_Alternation) != 0 ) {
	return testAnchor( i, aa[a ^ Anchor_Alternation].a, capBegin ) ||
	       testAnchor( i, aa[a ^ Anchor_Alternation].b, capBegin );
    }
#endif

    if ( (a & Anchor_Caret) != 0 ) {
	if ( mmPos + i != mmCaretPos )
	    return FALSE;
    }
    if ( (a & Anchor_Dollar) != 0 ) {
	if ( mmPos + i != mmLen )
	    return FALSE;
    }
#ifndef QT_NO_REGEXP_ESCAPE
    if ( (a & (Anchor_Word | Anchor_NonWord)) != 0 ) {
	bool before = FALSE;
	bool after = FALSE;
	if ( mmPos + i != 0 )
	    before = mmIn[mmPos + i - 1].isLetterOrNumber();
	if ( mmPos + i != mmLen )
	    after = mmIn[mmPos + i].isLetterOrNumber();
	if ( (a & Anchor_Word) != 0 && (before == after) )
	    return FALSE;
	if ( (a & Anchor_NonWord) != 0 && (before != after) )
	    return FALSE;
    }
#endif
#ifndef QT_NO_REGEXP_LOOKAHEAD
    bool catchx = TRUE;

    if ( (a & Anchor_LookaheadMask) != 0 ) {
	QConstString cstr = QConstString( (QChar *) mmIn + mmPos + i,
					   mmLen - mmPos - i );
	for ( j = 0; j < (int) ahead.size(); j++ ) {
	    if ( (a & (Anchor_FirstLookahead << j)) != 0 ) {
		catchx = ahead[j]->eng->match( cstr.string(), 0, TRUE, TRUE,
					       mmCaretPos - mmPos - i )[0] == 0;
		if ( catchx == ahead[j]->neg )
		    return FALSE;
	    }
	}
    }
#endif
#ifndef QT_NO_REGEXP_CAPTURE
#ifndef QT_NO_REGEXP_BACKREF
    for ( j = 0; j < nbrefs; j++ ) {
	if ( (a & (Anchor_BackRef1Empty << j)) != 0 ) {
	    if ( capBegin[j] != EmptyCapture )
		return FALSE;
	}
    }
#endif
#endif
    return TRUE;
}

#ifndef QT_NO_REGEXP_OPTIM
/*
  The three following functions are what Jeffrey Friedl would call
  transmissions (or bump-alongs). Using one or the other should make
  no difference except in performance.
*/

bool QRegExpEngine::goodStringMatch()
{
    int k = mmPos + goodEarlyStart;

    while ( (k = mmStr->find(goodStr, k, cs)) != -1 ) {
	int from = k - goodLateStart;
	int to = k - goodEarlyStart;
	if ( from > mmPos )
	    mmPos = from;

	while ( mmPos <= to ) {
	    if ( matchHere() )
		return TRUE;
	    mmPos++;
	}
	k++;
    }
    return FALSE;
}

bool QRegExpEngine::badCharMatch()
{
    int slideHead = 0;
    int slideNext = 0;
    int i;
    int lastPos = mmLen - minl;
    memset( mmSlideTab, 0, mmSlideTabSize * sizeof(int) );

    /*
      Set up the slide table, used for the bad-character heuristic,
      using the table of first occurrence of each character.
    */
    for ( i = 0; i < minl; i++ ) {
	int sk = occ1[BadChar(mmIn[mmPos + i])];
	if ( sk == NoOccurrence )
	    sk = i + 1;
	if ( sk > 0 ) {
	    int k = i + 1 - sk;
	    if ( k < 0 ) {
		sk = i + 1;
		k = 0;
	    }
	    if ( sk > mmSlideTab[k] )
		mmSlideTab[k] = sk;
	}
    }

    if ( mmPos > lastPos )
	return FALSE;

    for ( ;; ) {
	if ( ++slideNext >= mmSlideTabSize )
	    slideNext = 0;
	if ( mmSlideTab[slideHead] > 0 ) {
	    if ( mmSlideTab[slideHead] - 1 > mmSlideTab[slideNext] )
		mmSlideTab[slideNext] = mmSlideTab[slideHead] - 1;
	    mmSlideTab[slideHead] = 0;
	} else {
	    if ( matchHere() )
		return TRUE;
	}

	if ( mmPos == lastPos )
	    break;

	/*
	  Update the slide table. This code has much in common with
	  the initialization code.
	*/
	int sk = occ1[BadChar(mmIn[mmPos + minl])];
	if ( sk == NoOccurrence ) {
	    mmSlideTab[slideNext] = minl;
	} else if ( sk > 0 ) {
	    int k = slideNext + minl - sk;
	    if ( k >= mmSlideTabSize )
		k -= mmSlideTabSize;
	    if ( sk > mmSlideTab[k] )
		mmSlideTab[k] = sk;
	}
	slideHead = slideNext;
	mmPos++;
    }
    return FALSE;
}
#else
bool QRegExpEngine::bruteMatch()
{
    while ( mmPos <= mmLen ) {
	if ( matchHere() )
	    return TRUE;
	mmPos++;
    }
    return FALSE;
}
#endif

/*
  Here's the core of the engine. It tries to do a match here and now.
*/
bool QRegExpEngine::matchHere()
{
    int ncur = 1, nnext = 0;
    int i = 0, j, k, m;
    bool stop = FALSE;

    mmMatchLen = -1;
    mmMatchedLen = -1;
    mmCurStack[0] = InitialState;

#ifndef QT_NO_REGEXP_CAPTURE
    if ( ncap > 0 ) {
	for ( j = 0; j < ncap; j++ ) {
	    mmCurCapBegin[j] = EmptyCapture;
	    mmCurCapEnd[j] = EmptyCapture;
	}
    }
#endif

#ifndef QT_NO_REGEXP_BACKREF
    int *zzZ = 0;

    while ( (ncur > 0 || !mmSleeping.isEmpty()) && i <= mmLen - mmPos &&
	    !stop )
#else
    while ( ncur > 0 && i <= mmLen - mmPos && !stop )
#endif
    {
	int ch = ( i < mmLen - mmPos ) ? mmIn[mmPos + i].unicode() : 0;
	for ( j = 0; j < ncur; j++ ) {
	    int cur = mmCurStack[j];
	    State *scur = s[cur];
	    QMemArray<int>& outs = scur->outs;
	    for ( k = 0; k < (int) outs.size(); k++ ) {
		int next = outs[k];
		State *snext = s[next];
		bool in = TRUE;
#ifndef QT_NO_REGEXP_BACKREF
		int needSomeSleep = 0;
#endif

		/*
		  First, check if the anchors are anchored properly.
		*/
		if ( scur->anchors != 0 ) {
		    int a = at( *scur->anchors, next );
		    if ( a != 0 && !testAnchor(i, a, mmCurCapBegin + j * ncap) )
			in = FALSE;
		}
		/*
		  If indeed they are, check if the input character is
		  correct for this transition.
		*/
		if ( in ) {
		    m = snext->match;
		    if ( (m & (CharClassBit | BackRefBit)) == 0 ) {
			if ( cs )
			    in = ( m == ch );
			else
			    in = ( QChar(m).lower() == QChar(ch).lower() );
		    } else if ( next == FinalState ) {
			mmMatchLen = i;
			stop = mmMinimal;
			in = TRUE;
		    } else if ( (m & CharClassBit) != 0 ) {
#ifndef QT_NO_REGEXP_CCLASS
			const CharClass *cc = cl[m ^ CharClassBit];
			if ( cs )
			    in = cc->in( ch );
			else if ( cc->negative() )
			    in = cc->in( QChar(ch).lower() ) &&
				 cc->in( QChar(ch).upper() );
			else
			    in = cc->in( QChar(ch).lower() ) ||
				 cc->in( QChar(ch).upper() );
#endif
#ifndef QT_NO_REGEXP_BACKREF
		    } else { /* ( (m & BackRefBit) != 0 ) */
			int bref = m ^ BackRefBit;
			int ell = j * ncap + ( bref - 1 );

			in = bref <= ncap && mmCurCapBegin[ell] != EmptyCapture;
			if ( in ) {
			    if ( cs )
				in = ( mmIn[mmPos + mmCurCapBegin[ell]]
				       == QChar(ch) );
			    else
				in = ( mmIn[mmPos + mmCurCapBegin[ell]].lower()
				       == QChar(ch).lower() );
			}

			if ( in ) {
			    int delta;
			    if ( mmCurCapEnd[ell] == EmptyCapture )
				delta = i - mmCurCapBegin[ell];
			    else
				delta = mmCurCapEnd[ell] - mmCurCapBegin[ell];

			    in = ( delta <= mmLen - (mmPos + i) );
			    if ( in && delta > 1 ) {
				int n = 1;
				if ( cs ) {
				    while ( n < delta ) {
					if ( mmIn[mmPos +
						  mmCurCapBegin[ell] + n] !=
					     mmIn[mmPos + i + n] )
					    break;
					n++;
				    }
				} else {
				    while ( n < delta ) {
					QChar a = mmIn[mmPos +
						       mmCurCapBegin[ell] + n];
					QChar b = mmIn[mmPos + i + n];
					if ( a.lower() != b.lower() )
					    break;
					n++;
				    }
				}
				in = ( n == delta );
				if ( in )
				    needSomeSleep = delta - 1;
			    }
			}
#endif
		    }
		}

		/*
		  We must now update our data structures.
		*/
		if ( in ) {
#ifndef QT_NO_REGEXP_CAPTURE
		    int *capBegin, *capEnd;
#endif
		    /*
		      If the next state was not encountered yet, all
		      is fine.
		    */
		    if ( (m = mmInNextStack[next]) == -1 ) {
			m = nnext++;
			mmNextStack[m] = next;
			mmInNextStack[next] = m;
#ifndef QT_NO_REGEXP_CAPTURE
			capBegin = mmNextCapBegin + m * ncap;
			capEnd = mmNextCapEnd + m * ncap;

		    /*
		      Otherwise, we'll first maintain captures in
		      temporary arrays, and decide at the end whether
		      it's best to keep the previous capture zones or
		      the new ones.
		    */
		    } else {
			capBegin = mmTempCapBegin;
			capEnd = mmTempCapEnd;
#endif
		    }

#ifndef QT_NO_REGEXP_CAPTURE
		    /*
		      Updating the capture zones is much of a task.
		    */
		    if ( ncap > 0 ) {
			memcpy( capBegin, mmCurCapBegin + j * ncap,
				ncap * sizeof(int) );
			memcpy( capEnd, mmCurCapEnd + j * ncap,
				ncap * sizeof(int) );
			int c = scur->atom, n = snext->atom;
			int p = -1, q = -1;
			int cap;

			/*
			  Lemma 1. For any x in the range [0..nf), we
			  have f[x].parent < x.

			  Proof. By looking at startAtom(), it is
			  clear that cf < nf holds all the time, and
			  thus that f[nf].parent < nf.
			*/

			/*
			  If we are reentering an atom, we empty all
			  capture zones inside it.
			*/
			if ( scur->reenter != 0 &&
			     (q = at(*scur->reenter, next)) != 0 ) {
			    QBitArray b;
			    b.fill( FALSE, nf );
			    b.setBit( q, TRUE );
			    for ( int ell = q + 1; ell < nf; ell++ ) {
				if ( b.testBit(f[ell].parent) ) {
				    b.setBit( ell, TRUE );
				    cap = f[ell].capture;
				    if ( cap >= 0 ) {
					capBegin[cap] = EmptyCapture;
					capEnd[cap] = EmptyCapture;
				    }
				}
			    }
			    p = f[q].parent;

			/*
			  Otherwise, close the capture zones we are
			  leaving. We are leaving f[c].capture,
			  f[f[c].parent].capture,
			  f[f[f[c].parent].parent].capture, ...,
			  until f[x].capture, with x such that
			  f[x].parent is the youngest common ancestor
			  for c and n.

			  We go up along c's and n's ancestry until
			  we find x.
			*/
			} else {
			    p = c;
			    q = n;
			    while ( p != q ) {
				if ( p > q ) {
				    cap = f[p].capture;
				    if ( cap >= 0 ) {
					if ( capBegin[cap] == i ) {
					    capBegin[cap] = EmptyCapture;
					    capEnd[cap] = EmptyCapture;
					} else {
					    capEnd[cap] = i;
					}
				    }
				    p = f[p].parent;
				} else {
				    q = f[q].parent;
				}
			    }
			}

			/*
			  In any case, we now open the capture zones
			  we are entering. We work upwards from n
			  until we reach p (the parent of the atom we
			  reenter or the youngest common ancestor).
			*/
			while ( n > p ) {
			    cap = f[n].capture;
			    if ( cap >= 0 ) {
				capBegin[cap] = i;
				capEnd[cap] = EmptyCapture;
			    }
			    n = f[n].parent;
			}
			/*
			  If the next state was already in
			  mmNextStack, we must choose carefully which
			  capture zones we want to keep.
			*/
			if ( capBegin == mmTempCapBegin &&
			     isBetterCapture(capBegin, capEnd,
					     mmNextCapBegin + m * ncap,
					     mmNextCapEnd + m * ncap) ) {
			    memcpy( mmNextCapBegin + m * ncap, capBegin,
				    ncap * sizeof(int) );
			    memcpy( mmNextCapEnd + m * ncap, capEnd,
				    ncap * sizeof(int) );
			}
		    }
#ifndef QT_NO_REGEXP_BACKREF
		    /*
		      We are done with updating the capture zones.
		      It's now time to put the next state to sleep,
		      if it needs to, and to remove it from
		      mmNextStack.
		    */
		    if ( needSomeSleep > 0 ) {
			zzZ = new int[1 + 2 * ncap];
			zzZ[0] = next;
			if ( ncap > 0 ) {
			    memcpy( zzZ + 1, capBegin, ncap * sizeof(int) );
			    memcpy( zzZ + 1 + ncap, capEnd,
				    ncap * sizeof(int) );
			}
			mmInNextStack[mmNextStack[--nnext]] = -1;
			mmSleeping.insert( i + needSomeSleep, zzZ );
		    }
#endif
#endif
		}
	    }
	}
#ifndef QT_NO_REGEXP_CAPTURE
	/*
	  If we reached the final state, hurray! Copy the captured
	  zone.
	*/
	if ( ncap > 0 && (m = mmInNextStack[FinalState]) != -1 ) {
	    memcpy( mmCapBegin, mmNextCapBegin + m * ncap, ncap * sizeof(int) );
	    memcpy( mmCapEnd, mmNextCapEnd + m * ncap, ncap * sizeof(int) );
	}
#ifndef QT_NO_REGEXP_BACKREF
	/*
	  It's time to wake up the sleepers.
	*/
	if ( !mmSleeping.isEmpty() ) {
	    while ( (zzZ = mmSleeping.take(i)) != 0 ) {
		int next = zzZ[0];
		int *capBegin = zzZ + 1;
		int *capEnd = zzZ + 1 + ncap;
		bool copyOver = TRUE;

		if ( (m = mmInNextStack[zzZ[0]]) == -1 ) {
		    m = nnext++;
		    mmNextStack[m] = next;
		    mmInNextStack[next] = m;
		} else {
		    copyOver = isBetterCapture( mmNextCapBegin + m * ncap,
						mmNextCapEnd + m * ncap,
						capBegin, capEnd );
		}
		if ( copyOver ) {
		    memcpy( mmNextCapBegin + m * ncap, capBegin,
			    ncap * sizeof(int) );
		    memcpy( mmNextCapEnd + m * ncap, capEnd,
			    ncap * sizeof(int) );
		}
		delete[] zzZ;
	    }
	}
#endif
#endif
	for ( j = 0; j < nnext; j++ )
	    mmInNextStack[mmNextStack[j]] = -1;

	// avoid needless iteration that confuses mmMatchedLen
	if ( nnext == 1 && mmNextStack[0] == FinalState
#ifndef QT_NO_REGEXP_BACKREF
	     && mmSleeping.isEmpty()
#endif
	     )
	    stop = TRUE;

	qSwap( mmCurStack, mmNextStack );
#ifndef QT_NO_REGEXP_CAPTURE
	qSwap( mmCurCapBegin, mmNextCapBegin );
	qSwap( mmCurCapEnd, mmNextCapEnd );
#endif
	ncur = nnext;
	nnext = 0;
	i++;
    }

#ifndef QT_NO_REGEXP_BACKREF
    /*
      If minimal matching is enabled, we might have some sleepers
      left.
    */
    while ( !mmSleeping.isEmpty() ) {
	zzZ = mmSleeping.take( *QIntDictIterator<int>(mmSleeping) );
	delete[] zzZ;
    }
#endif

    mmMatchedLen = i - 1;
    return ( mmMatchLen >= 0 );
}

#ifndef QT_NO_REGEXP_CCLASS

QRegExpEngine::CharClass::CharClass()
    : c( 0 ), n( FALSE )
{
#ifndef QT_NO_REGEXP_OPTIM
    occ1.fill( NoOccurrence, NumBadChars );
#endif
}

QRegExpEngine::CharClass& QRegExpEngine::CharClass::operator=(
	const CharClass& cc )
{
    c = cc.c;
    r = cc.r.copy();
    n = cc.n;
#ifndef QT_NO_REGEXP_OPTIM
    occ1 = cc.occ1;
#endif
    return *this;
}

void QRegExpEngine::CharClass::clear()
{
    c = 0;
    r.resize( 0 );
    n = FALSE;
}

void QRegExpEngine::CharClass::setNegative( bool negative )
{
    n = negative;
#ifndef QT_NO_REGEXP_OPTIM
    occ1.fill( 0, NumBadChars );
#endif
}

void QRegExpEngine::CharClass::addCategories( int cats )
{
    c |= cats;
#ifndef QT_NO_REGEXP_OPTIM
    occ1.fill( 0, NumBadChars );
#endif
}

void QRegExpEngine::CharClass::addRange( ushort from, ushort to )
{
    if ( from > to )
	qSwap( from, to );
    int m = r.size();
    r.resize( m + 1 );
    r[m].from = from;
    r[m].to = to;

#ifndef QT_NO_REGEXP_OPTIM
    int i;

    if ( to - from < NumBadChars ) {
	occ1.detach();
	if ( from % NumBadChars <= to % NumBadChars ) {
	    for ( i = from % NumBadChars; i <= to % NumBadChars; i++ )
		occ1[i] = 0;
	} else {
	    for ( i = 0; i <= to % NumBadChars; i++ )
		occ1[i] = 0;
	    for ( i = from % NumBadChars; i < NumBadChars; i++ )
		occ1[i] = 0;
	}
    } else {
	occ1.fill( 0, NumBadChars );
    }
#endif
}

bool QRegExpEngine::CharClass::in( QChar ch ) const
{
#ifndef QT_NO_REGEXP_OPTIM
    if ( occ1[BadChar(ch)] == NoOccurrence )
	return n;
#endif

    if ( c != 0 && (c & (1 << (int) ch.category())) != 0 )
	return !n;
    for ( int i = 0; i < (int) r.size(); i++ ) {
	if ( ch.unicode() >= r[i].from && ch.unicode() <= r[i].to )
	    return !n;
    }
    return n;
}

#if defined(QT_DEBUG)
void QRegExpEngine::CharClass::dump() const
{
    int i;
    qDebug( "    %stive character class", n ? "nega" : "posi" );
#ifndef QT_NO_REGEXP_CCLASS
    if ( c != 0 )
	qDebug( "      categories 0x%.8x", c );
#endif
    for ( i = 0; i < (int) r.size(); i++ )
	qDebug( "      0x%.4x through 0x%.4x", r[i].from, r[i].to );
}
#endif
#endif

QRegExpEngine::Box::Box( QRegExpEngine *engine )
    : eng( engine ), skipanchors( 0 )
#ifndef QT_NO_REGEXP_OPTIM
      , earlyStart( 0 ), lateStart( 0 ), maxl( 0 )
#endif
{
#ifndef QT_NO_REGEXP_OPTIM
    occ1.fill( NoOccurrence, NumBadChars );
#endif
    minl = 0;
}

QRegExpEngine::Box& QRegExpEngine::Box::operator=( const Box& b )
{
    eng = b.eng;
    ls = b.ls;
    rs = b.rs;
    lanchors = b.lanchors;
    ranchors = b.ranchors;
    skipanchors = b.skipanchors;
#ifndef QT_NO_REGEXP_OPTIM
    earlyStart = b.earlyStart;
    lateStart = b.lateStart;
    str = b.str;
    leftStr = b.leftStr;
    rightStr = b.rightStr;
    maxl = b.maxl;
    occ1 = b.occ1;
#endif
    minl = b.minl;
    return *this;
}

void QRegExpEngine::Box::set( QChar ch )
{
    ls.resize( 1 );
    ls[0] = eng->createState( ch );
    rs = ls;
    rs.detach();
#ifndef QT_NO_REGEXP_OPTIM
    str = ch;
    leftStr = ch;
    rightStr = ch;
    maxl = 1;
    occ1.detach();
    occ1[BadChar(ch)] = 0;
#endif
    minl = 1;
}

void QRegExpEngine::Box::set( const CharClass& cc )
{
    ls.resize( 1 );
    ls[0] = eng->createState( cc );
    rs = ls;
    rs.detach();
#ifndef QT_NO_REGEXP_OPTIM
    maxl = 1;
    occ1 = cc.firstOccurrence();
#endif
    minl = 1;
}

#ifndef QT_NO_REGEXP_BACKREF
void QRegExpEngine::Box::set( int bref )
{
    ls.resize( 1 );
    ls[0] = eng->createState( bref );
    rs = ls;
    rs.detach();
    if ( bref >= 1 && bref <= MaxBackRefs )
	skipanchors = Anchor_BackRef0Empty << bref;
#ifndef QT_NO_REGEXP_OPTIM
    maxl = InftyLen;
#endif
    minl = 0;
}
#endif

void QRegExpEngine::Box::cat( const Box& b )
{
    eng->addCatTransitions( rs, b.ls );
    addAnchorsToEngine( b );
    if ( minl == 0 ) {
	mergeInto( &lanchors, b.lanchors );
	if ( skipanchors != 0 ) {
	    for ( int i = 0; i < (int) b.ls.size(); i++ ) {
		int a = eng->anchorConcatenation( at(lanchors, b.ls[i]),
						  skipanchors );
		lanchors.insert( b.ls[i], a );
	    }
	}
	mergeInto( &ls, b.ls );
    }
    if ( b.minl == 0 ) {
	mergeInto( &ranchors, b.ranchors );
	if ( b.skipanchors != 0 ) {
	    for ( int i = 0; i < (int) rs.size(); i++ ) {
		int a = eng->anchorConcatenation( at(ranchors, rs[i]),
						  b.skipanchors );
		ranchors.insert( rs[i], a );
	    }
	}
	mergeInto( &rs, b.rs );
    } else {
	ranchors = b.ranchors;
	rs = b.rs;
    }

#ifndef QT_NO_REGEXP_OPTIM
    if ( maxl != InftyLen ) {
	if ( rightStr.length() + b.leftStr.length() >
	     QMAX(str.length(), b.str.length()) ) {
	    earlyStart = minl - rightStr.length();
	    lateStart = maxl - rightStr.length();
	    str = rightStr + b.leftStr;
	} else if ( b.str.length() > str.length() ) {
	    earlyStart = minl + b.earlyStart;
	    lateStart = maxl + b.lateStart;
	    str = b.str;
	}
    }

    if ( (int) leftStr.length() == maxl )
	leftStr += b.leftStr;
    if ( (int) b.rightStr.length() == b.maxl )
	rightStr += b.rightStr;
    else
	rightStr = b.rightStr;

    if ( maxl == InftyLen || b.maxl == InftyLen )
	maxl = InftyLen;
    else
	maxl += b.maxl;

    occ1.detach();
    for ( int i = 0; i < NumBadChars; i++ ) {
	if ( b.occ1[i] != NoOccurrence && minl + b.occ1[i] < occ1[i] )
	    occ1[i] = minl + b.occ1[i];
    }
#endif

    minl += b.minl;
    if ( minl == 0 )
	skipanchors = eng->anchorConcatenation( skipanchors, b.skipanchors );
    else
	skipanchors = 0;
}

void QRegExpEngine::Box::orx( const Box& b )
{
    mergeInto( &ls, b.ls );
    mergeInto( &lanchors, b.lanchors );
    mergeInto( &rs, b.rs );
    mergeInto( &ranchors, b.ranchors );

    if ( b.minl == 0 ) {
	if ( minl == 0 )
	    skipanchors = eng->anchorAlternation( skipanchors, b.skipanchors );
	else
	    skipanchors = b.skipanchors;
    }

#ifndef QT_NO_REGEXP_OPTIM
    occ1.detach();
    for ( int i = 0; i < NumBadChars; i++ ) {
	if ( occ1[i] > b.occ1[i] )
	    occ1[i] = b.occ1[i];
    }
    earlyStart = 0;
    lateStart = 0;
    str = QString();
    leftStr = QString();
    rightStr = QString();
    if ( b.maxl > maxl )
	maxl = b.maxl;
#endif
    if ( b.minl < minl )
	minl = b.minl;
}

void QRegExpEngine::Box::plus( int atom )
{
#ifndef QT_NO_REGEXP_CAPTURE
    eng->addPlusTransitions( rs, ls, atom );
#else
    Q_UNUSED( atom );
    eng->addCatTransitions( rs, ls );
#endif
    addAnchorsToEngine( *this );
#ifndef QT_NO_REGEXP_OPTIM
    maxl = InftyLen;
#endif
}

void QRegExpEngine::Box::opt()
{
#ifndef QT_NO_REGEXP_OPTIM
    earlyStart = 0;
    lateStart = 0;
    str = QString();
    leftStr = QString();
    rightStr = QString();
#endif
    skipanchors = 0;
    minl = 0;
}

void QRegExpEngine::Box::catAnchor( int a )
{
    if ( a != 0 ) {
	for ( int i = 0; i < (int) rs.size(); i++ ) {
	    a = eng->anchorConcatenation( at(ranchors, rs[i]), a );
	    ranchors.insert( rs[i], a );
	}
	if ( minl == 0 )
	    skipanchors = eng->anchorConcatenation( skipanchors, a );
    }
}

#ifndef QT_NO_REGEXP_OPTIM
void QRegExpEngine::Box::setupHeuristics()
{
    eng->setupGoodStringHeuristic( earlyStart, lateStart, str );

    /*
      A regular expression such as 112|1 has occ1['2'] = 2 and minl =
      1 at this point. An entry of occ1 has to be at most minl or
      infinity for the rest of the algorithm to go well.

      We waited until here before normalizing these cases (instead of
      doing it in Box::orx()) because sometimes things improve by
      themselves. Consider for example (112|1)34.
    */
    for ( int i = 0; i < NumBadChars; i++ ) {
	if ( occ1[i] != NoOccurrence && occ1[i] >= minl )
	    occ1[i] = minl;
    }
    eng->setupBadCharHeuristic( minl, occ1 );

    eng->heuristicallyChooseHeuristic();
}
#endif

#if defined(QT_DEBUG)
void QRegExpEngine::Box::dump() const
{
    int i;
    qDebug( "Box of at least %d character%s", minl, minl == 1 ? "" : "s" );
    qDebug( "  Left states:" );
    for ( i = 0; i < (int) ls.size(); i++ ) {
	if ( at(lanchors, ls[i]) == 0 )
	    qDebug( "    %d", ls[i] );
	else
	    qDebug( "    %d [anchors 0x%.8x]", ls[i], lanchors[ls[i]] );
    }
    qDebug( "  Right states:" );
    for ( i = 0; i < (int) rs.size(); i++ ) {
	if ( at(ranchors, rs[i]) == 0 )
	    qDebug( "    %d", rs[i] );
	else
	    qDebug( "    %d [anchors 0x%.8x]", rs[i], ranchors[rs[i]] );
    }
    qDebug( "  Skip anchors: 0x%.8x", skipanchors );
}
#endif

void QRegExpEngine::Box::addAnchorsToEngine( const Box& to ) const
{
    for ( int i = 0; i < (int) to.ls.size(); i++ ) {
	for ( int j = 0; j < (int) rs.size(); j++ ) {
	    int a = eng->anchorConcatenation( at(ranchors, rs[j]),
					      at(to.lanchors, to.ls[i]) );
	    eng->addAnchors( rs[j], to.ls[i], a );
	}
    }
}

int QRegExpEngine::getChar()
{
    return ( yyPos == yyLen ) ? EOS : yyIn[yyPos++].unicode();
}

int QRegExpEngine::getEscape()
{
#ifndef QT_NO_REGEXP_ESCAPE
    const char tab[] = "afnrtv"; // no b, as \b means word boundary
    const char backTab[] = "\a\f\n\r\t\v";
    ushort low;
    int i;
#endif
    ushort val;
    int prevCh = yyCh;

    if ( prevCh == EOS ) {
	error( RXERR_END );
	return Tok_Char | '\\';
    }
    yyCh = getChar();
#ifndef QT_NO_REGEXP_ESCAPE
    if ( (prevCh & ~0xff) == 0 ) {
	const char *p = strchr( tab, prevCh );
	if ( p != 0 )
	    return Tok_Char | backTab[p - tab];
    }
#endif

    switch ( prevCh ) {
#ifndef QT_NO_REGEXP_ESCAPE
    case '0':
	val = 0;
	for ( i = 0; i < 3; i++ ) {
	    if ( yyCh >= '0' && yyCh <= '7' )
		val = ( val << 3 ) | ( yyCh - '0' );
	    else
		break;
	    yyCh = getChar();
	}
	if ( (val & ~0377) != 0 )
	    error( RXERR_OCTAL );
	return Tok_Char | val;
#endif
#ifndef QT_NO_REGEXP_ESCAPE
    case 'B':
	return Tok_NonWord;
#endif
#ifndef QT_NO_REGEXP_CCLASS
    case 'D':
	// see QChar::isDigit()
	yyCharClass->addCategories( 0x7fffffef );
	return Tok_CharClass;
    case 'S':
	// see QChar::isSpace()
	yyCharClass->addCategories( 0x7ffff87f );
	yyCharClass->addRange( 0x0000, 0x0008 );
	yyCharClass->addRange( 0x000e, 0x001f );
	yyCharClass->addRange( 0x007f, 0x009f );
	return Tok_CharClass;
    case 'W':
	// see QChar::isLetterOrNumber()
	yyCharClass->addCategories( 0x7ff07f8f );
	return Tok_CharClass;
#endif
#ifndef QT_NO_REGEXP_ESCAPE
    case 'b':
	return Tok_Word;
#endif
#ifndef QT_NO_REGEXP_CCLASS
    case 'd':
	// see QChar::isDigit()
	yyCharClass->addCategories( 0x00000010 );
	return Tok_CharClass;
    case 's':
	// see QChar::isSpace()
	yyCharClass->addCategories( 0x00000380 );
	yyCharClass->addRange( 0x0009, 0x000d );
	return Tok_CharClass;
    case 'w':
	// see QChar::isLetterOrNumber()
	yyCharClass->addCategories( 0x000f8070 );
	return Tok_CharClass;
#endif
#ifndef QT_NO_REGEXP_ESCAPE
    case 'x':
	val = 0;
	for ( i = 0; i < 4; i++ ) {
	    low = QChar( yyCh ).lower();
	    if ( low >= '0' && low <= '9' )
		val = ( val << 4 ) | ( low - '0' );
	    else if ( low >= 'a' && low <= 'f' )
		val = ( val << 4 ) | ( low - 'a' + 10 );
	    else
		break;
	    yyCh = getChar();
	}
	return Tok_Char | val;
#endif
    default:
	if ( prevCh >= '1' && prevCh <= '9' ) {
#ifndef QT_NO_REGEXP_BACKREF
	    val = prevCh - '0';
	    while ( yyCh >= '0' && yyCh <= '9' ) {
		val = ( val *= 10 ) | ( yyCh - '0' );
		yyCh = getChar();
	    }
	    return Tok_BackRef | val;
#else
	    error( RXERR_DISABLED );
#endif
	}
	return Tok_Char | prevCh;
    }
}

#ifndef QT_NO_REGEXP_INTERVAL
int QRegExpEngine::getRep( int def )
{
    if ( yyCh >= '0' && yyCh <= '9' ) {
	int rep = 0;
	do {
	    rep = 10 * rep + yyCh - '0';
	    if ( rep >= InftyRep ) {
		error( RXERR_REPETITION );
		rep = def;
	    }
	    yyCh = getChar();
	} while ( yyCh >= '0' && yyCh <= '9' );
	return rep;
    } else {
	return def;
    }
}
#endif

#ifndef QT_NO_REGEXP_LOOKAHEAD
void QRegExpEngine::skipChars( int n )
{
    if ( n > 0 ) {
	yyPos += n - 1;
	yyCh = getChar();
    }
}
#endif

void QRegExpEngine::error( const char *msg )
{
    if ( yyError.isEmpty() )
	yyError = QString::fromLatin1( msg );
}

void QRegExpEngine::startTokenizer( const QChar *rx, int len )
{
    yyIn = rx;
    yyPos0 = 0;
    yyPos = 0;
    yyLen = len;
    yyCh = getChar();
    yyCharClass = new CharClass;
    yyMinRep = 0;
    yyMaxRep = 0;
    yyError = QString();
}

int QRegExpEngine::getToken()
{
#ifndef QT_NO_REGEXP_CCLASS
    ushort pendingCh = 0;
    bool charPending;
    bool rangePending;
    int tok;
#endif
    int prevCh = yyCh;

    yyPos0 = yyPos - 1;
#ifndef QT_NO_REGEXP_CCLASS
    yyCharClass->clear();
#endif
    yyMinRep = 0;
    yyMaxRep = 0;
    yyCh = getChar();
    switch ( prevCh ) {
    case EOS:
	yyPos0 = yyPos;
	return Tok_Eos;
    case '$':
	return Tok_Dollar;
    case '(':
	if ( yyCh == '?' ) {
	    prevCh = getChar();
	    yyCh = getChar();
	    switch ( prevCh ) {
#ifndef QT_NO_REGEXP_LOOKAHEAD
	    case '!':
		return Tok_NegLookahead;
	    case '=':
		return Tok_PosLookahead;
#endif
	    case ':':
		return Tok_MagicLeftParen;
	    default:
		error( RXERR_LOOKAHEAD );
		return Tok_MagicLeftParen;
	    }
	} else {
	    return Tok_LeftParen;
	}
    case ')':
	return Tok_RightParen;
    case '*':
	yyMinRep = 0;
	yyMaxRep = InftyRep;
	return Tok_Quantifier;
    case '+':
	yyMinRep = 1;
	yyMaxRep = InftyRep;
	return Tok_Quantifier;
    case '.':
#ifndef QT_NO_REGEXP_CCLASS
	yyCharClass->setNegative( TRUE );
#endif
	return Tok_CharClass;
    case '?':
	yyMinRep = 0;
	yyMaxRep = 1;
	return Tok_Quantifier;
    case '[':
#ifndef QT_NO_REGEXP_CCLASS
	if ( yyCh == '^' ) {
	    yyCharClass->setNegative( TRUE );
	    yyCh = getChar();
	}
	charPending = FALSE;
	rangePending = FALSE;
	do {
	    if ( yyCh == '-' && charPending && !rangePending ) {
		rangePending = TRUE;
		yyCh = getChar();
	    } else {
		if ( charPending && !rangePending ) {
		    yyCharClass->addSingleton( pendingCh );
		    charPending = FALSE;
		}
		if ( yyCh == '\\' ) {
		    yyCh = getChar();
		    tok = getEscape();
		    if ( tok == Tok_Word )
			tok = '\b';
		} else {
		    tok = Tok_Char | yyCh;
		    yyCh = getChar();
		}
		if ( tok == Tok_CharClass ) {
		    if ( rangePending ) {
			yyCharClass->addSingleton( '-' );
			yyCharClass->addSingleton( pendingCh );
			charPending = FALSE;
			rangePending = FALSE;
		    }
		} else if ( (tok & Tok_Char) != 0 ) {
		    if ( rangePending ) {
			yyCharClass->addRange( pendingCh, tok ^ Tok_Char );
			charPending = FALSE;
			rangePending = FALSE;
		    } else {
			pendingCh = tok ^ Tok_Char;
			charPending = TRUE;
		    }
		} else {
		    error( RXERR_CHARCLASS );
		}
	    }
	}  while ( yyCh != ']' && yyCh != EOS );
	if ( rangePending )
	    yyCharClass->addSingleton( '-' );
	if ( charPending )
	    yyCharClass->addSingleton( pendingCh );
	if ( yyCh == EOS )
	    error( RXERR_END );
	else
	    yyCh = getChar();
	return Tok_CharClass;
#else
	error( RXERR_END );
	return Tok_Char | '[';
#endif
    case '\\':
	return getEscape();
    case ']':
	error( RXERR_LEFTDELIM );
	return Tok_Char | ']';
    case '^':
	return Tok_Caret;
    case '{':
#ifndef QT_NO_REGEXP_INTERVAL
	yyMinRep = getRep( 0 );
	yyMaxRep = yyMinRep;
	if ( yyCh == ',' ) {
	    yyCh = getChar();
	    yyMaxRep = getRep( InftyRep );
	}
	if ( yyMaxRep < yyMinRep )
	    qSwap( yyMinRep, yyMaxRep );
	if ( yyCh != '}' )
	    error( RXERR_REPETITION );
	yyCh = getChar();
	return Tok_Quantifier;
#else
	error( RXERR_DISABLED );
	return Tok_Char | '{';
#endif
    case '|':
	return Tok_Bar;
    case '}':
	error( RXERR_LEFTDELIM );
	return Tok_Char | '}';
    default:
	return Tok_Char | prevCh;
    }
}

int QRegExpEngine::parse( const QChar *pattern, int len )
{
    valid = TRUE;
    startTokenizer( pattern, len );
    yyTok = getToken();
#ifndef QT_NO_REGEXP_CAPTURE
    yyMayCapture = TRUE;
#else
    yyMayCapture = FALSE;
#endif

#ifndef QT_NO_REGEXP_CAPTURE
    int atom = startAtom( FALSE );
#endif
    CharClass anything;
    Box box( this ); // create InitialState
    box.set( anything );
    Box rightBox( this ); // create FinalState
    rightBox.set( anything );

    Box middleBox( this );
    parseExpression( &middleBox );
#ifndef QT_NO_REGEXP_CAPTURE
    finishAtom( atom );
#endif
#ifndef QT_NO_REGEXP_OPTIM
    middleBox.setupHeuristics();
#endif
    box.cat( middleBox );
    box.cat( rightBox );
    delete yyCharClass;
    yyCharClass = 0;

    officialncap = ncap;
#ifndef QT_NO_REGEXP_BACKREF
    if ( nbrefs > ncap )
	ncap = nbrefs;
#endif

    mmCaptured.resize( 2 + 2 * officialncap );
    mmCapturedNoMatch.fill( -1, 2 + 2 * officialncap );

    /*
      We use one QMemArray<int> for all the big data used a lot in
      matchHere() and friends.
    */
#ifndef QT_NO_REGEXP_OPTIM
    mmSlideTabSize = QMAX( minl + 1, 16 );
#else
    mmSlideTabSize = 0;
#endif
    mmBigArray.resize( (3 + 4 * ncap) * ns + 4 * ncap + mmSlideTabSize );

    mmInNextStack = mmBigArray.data();
    memset( mmInNextStack, -1, ns * sizeof(int) );
    mmCurStack = mmInNextStack + ns;
    mmNextStack = mmInNextStack + 2 * ns;

    mmCurCapBegin = mmInNextStack + 3 * ns;
    mmNextCapBegin = mmCurCapBegin + ncap * ns;
    mmCurCapEnd = mmCurCapBegin + 2 * ncap * ns;
    mmNextCapEnd = mmCurCapBegin + 3 * ncap * ns;

    mmTempCapBegin = mmCurCapBegin + 4 * ncap * ns;
    mmTempCapEnd = mmTempCapBegin + ncap;
    mmCapBegin = mmTempCapBegin + 2 * ncap;
    mmCapEnd = mmTempCapBegin + 3 * ncap;

    mmSlideTab = mmTempCapBegin + 4 * ncap;

    if ( !yyError.isEmpty() )
	return -1;

#ifndef QT_NO_REGEXP_OPTIM
    State *sinit = s[InitialState];
    caretAnchored = ( sinit->anchors != 0 );
    if ( caretAnchored ) {
	QMap<int, int>& anchors = *sinit->anchors;
	QMap<int, int>::ConstIterator a;
	for ( a = anchors.begin(); a != anchors.end(); ++a ) {
#ifndef QT_NO_REGEXP_ANCHOR_ALT
	    if ( (*a & Anchor_Alternation) != 0 )
		break;
#endif
	    if ( (*a & Anchor_Caret) == 0 ) {
		caretAnchored = FALSE;
		break;
	    }
	}
    }
#endif
    return yyPos0;
}

void QRegExpEngine::parseAtom( Box *box )
{
#ifndef QT_NO_REGEXP_LOOKAHEAD
    QRegExpEngine *eng = 0;
    bool neg;
    int len;
#endif

    switch ( yyTok ) {
    case Tok_Dollar:
	box->catAnchor( Anchor_Dollar );
	break;
    case Tok_Caret:
	box->catAnchor( Anchor_Caret );
	break;
#ifndef QT_NO_REGEXP_LOOKAHEAD
    case Tok_PosLookahead:
    case Tok_NegLookahead:
	neg = ( yyTok == Tok_NegLookahead );
	eng = new QRegExpEngine( cs );
	len = eng->parse( yyIn + yyPos - 1, yyLen - yyPos + 1 );
	if ( len >= 0 )
	    skipChars( len );
	else
	    error( RXERR_LOOKAHEAD );
	box->catAnchor( addLookahead(eng, neg) );
	yyTok = getToken();
	if ( yyTok != Tok_RightParen )
	    error( RXERR_LOOKAHEAD );
	break;
#endif
#ifndef QT_NO_REGEXP_ESCAPE
    case Tok_Word:
	box->catAnchor( Anchor_Word );
	break;
    case Tok_NonWord:
	box->catAnchor( Anchor_NonWord );
	break;
#endif
    case Tok_LeftParen:
    case Tok_MagicLeftParen:
	yyTok = getToken();
	parseExpression( box );
	if ( yyTok != Tok_RightParen )
	    error( RXERR_END );
	break;
    case Tok_CharClass:
	box->set( *yyCharClass );
	break;
    case Tok_Quantifier:
	error( RXERR_REPETITION );
	break;
    default:
	if ( (yyTok & Tok_Char) != 0 )
	    box->set( QChar(yyTok ^ Tok_Char) );
#ifndef QT_NO_REGEXP_BACKREF
	else if ( (yyTok & Tok_BackRef) != 0 )
	    box->set( yyTok ^ Tok_BackRef );
#endif
	else
	    error( RXERR_DISABLED );
    }
    yyTok = getToken();
}

void QRegExpEngine::parseFactor( Box *box )
{
#ifndef QT_NO_REGEXP_CAPTURE
    int atom = startAtom( yyMayCapture && yyTok == Tok_LeftParen );
#else
    static const int atom = 0;
#endif

#ifndef QT_NO_REGEXP_INTERVAL
#define YYREDO() \
	yyIn = in, yyPos0 = pos0, yyPos = pos, yyLen = len, yyCh = ch, \
	*yyCharClass = charClass, yyMinRep = 0, yyMaxRep = 0, yyTok = tok

    const QChar *in = yyIn;
    int pos0 = yyPos0;
    int pos = yyPos;
    int len = yyLen;
    int ch = yyCh;
    CharClass charClass;
    if ( yyTok == Tok_CharClass )
	charClass = *yyCharClass;
    int tok = yyTok;
    bool mayCapture = yyMayCapture;
#endif

    parseAtom( box );
#ifndef QT_NO_REGEXP_CAPTURE
    finishAtom( atom );
#endif

    if ( yyTok == Tok_Quantifier ) {
	if ( yyMaxRep == InftyRep ) {
	    box->plus( atom );
#ifndef QT_NO_REGEXP_INTERVAL
	} else if ( yyMaxRep == 0 ) {
	    box->clear();
#endif
	}
	if ( yyMinRep == 0 )
	    box->opt();

#ifndef QT_NO_REGEXP_INTERVAL
	yyMayCapture = FALSE;
	int alpha = ( yyMinRep == 0 ) ? 0 : yyMinRep - 1;
	int beta = ( yyMaxRep == InftyRep ) ? 0 : yyMaxRep - ( alpha + 1 );

	Box rightBox( this );
	int i;

	for ( i = 0; i < beta; i++ ) {
	    YYREDO();
	    Box leftBox( this );
	    parseAtom( &leftBox );
	    leftBox.cat( rightBox );
	    leftBox.opt();
	    rightBox = leftBox;
	}
	for ( i = 0; i < alpha; i++ ) {
	    YYREDO();
	    Box leftBox( this );
	    parseAtom( &leftBox );
	    leftBox.cat( rightBox );
	    rightBox = leftBox;
	}
	rightBox.cat( *box );
	*box = rightBox;
#endif
	yyTok = getToken();
#ifndef QT_NO_REGEXP_INTERVAL
	yyMayCapture = mayCapture;
#endif
    }
#undef YYREDO
}

void QRegExpEngine::parseTerm( Box *box )
{
#ifndef QT_NO_REGEXP_OPTIM
    if ( yyTok != Tok_Eos && yyTok != Tok_RightParen && yyTok != Tok_Bar )
	parseFactor( box );
#endif
    while ( yyTok != Tok_Eos && yyTok != Tok_RightParen && yyTok != Tok_Bar ) {
	Box rightBox( this );
	parseFactor( &rightBox );
	box->cat( rightBox );
    }
}

void QRegExpEngine::parseExpression( Box *box )
{
    parseTerm( box );
    while ( yyTok == Tok_Bar ) {
	Box rightBox( this );
	yyTok = getToken();
	parseTerm( &rightBox );
	box->orx( rightBox );
    }
}

/*
  The struct QRegExpPrivate contains the private data of a regular
  expression other than the automaton. It makes it possible for many
  QRegExp objects to use the same QRegExpEngine object with different
  QRegExpPrivate objects.
*/
struct QRegExpPrivate
{
    QString pattern; // regular-expression or wildcard pattern
    QString rxpattern; // regular-expression pattern
#ifndef QT_NO_REGEXP_WILDCARD
    bool wc; // wildcard mode?
#endif
    bool min; // minimal matching? (instead of maximal)
#ifndef QT_NO_REGEXP_CAPTURE
    QString t; // last string passed to QRegExp::search() or searchRev()
    QStringList capturedCache; // what QRegExp::capturedTexts() returned last
#endif
    QMemArray<int> captured; // what QRegExpEngine::search() returned last

    QRegExpPrivate() { captured.fill( -1, 2 ); }
};

#ifndef QT_NO_REGEXP_OPTIM
static QCache<QRegExpEngine> *engineCache = 0;
static QSingleCleanupHandler<QCache<QRegExpEngine> > cleanup_cache;
#endif

static QRegExpEngine *newEngine( const QString& pattern, bool caseSensitive )
{
#ifndef QT_NO_REGEXP_OPTIM
    if ( engineCache != 0 ) {
#ifdef QT_THREAD_SUPPORT
	QMutexLocker locker( qt_global_mutexpool->get( &engineCache ) );
#endif
	QRegExpEngine *eng = engineCache->take( pattern );
	if ( eng == 0 || eng->caseSensitive() != caseSensitive ) {
	    delete eng;
	} else {
	    eng->ref();
	    return eng;
	}
    }
#endif
    return new QRegExpEngine( pattern, caseSensitive );
}

static void derefEngine( QRegExpEngine *eng, const QString& pattern )
{
    if ( eng != 0 && eng->deref() ) {
#ifndef QT_NO_REGEXP_OPTIM
#ifdef QT_THREAD_SUPPORT
	QMutexLocker locker( qt_global_mutexpool->get( &engineCache ) );
#endif
	if ( engineCache == 0 ) {
	    engineCache = new QCache<QRegExpEngine>;
	    engineCache->setAutoDelete( TRUE );
	    cleanup_cache.set( &engineCache );
	}
	if ( !pattern.isNull() &&
	     engineCache->insert(pattern, eng, 4 + pattern.length() / 4) )
	    return;
#else
	Q_UNUSED( pattern );
#endif
	delete eng;
    }
}

/*!
    \enum QRegExp::CaretMode

    The CaretMode enum defines the different meanings of the caret
    (<b>^</b>) in a regular expression. The possible values are:

    \value CaretAtZero
	   The caret corresponds to index 0 in the searched string.

    \value CaretAtOffset
	   The caret corresponds to the start offset of the search.

    \value CaretWontMatch
	   The caret never matches.
*/

/*!
    Constructs an empty regexp.

    \sa isValid() errorString()
*/
QRegExp::QRegExp()
{
    eng = new QRegExpEngine( TRUE );
    priv = new QRegExpPrivate;
#ifndef QT_NO_REGEXP_WILDCARD
    priv->wc = FALSE;
#endif
    priv->min = FALSE;
    compile( TRUE );
}

/*!
    Constructs a regular expression object for the given \a pattern
    string. The pattern must be given using wildcard notation if \a
    wildcard is TRUE (default is FALSE). The pattern is case
    sensitive, unless \a caseSensitive is FALSE. Matching is greedy
    (maximal), but can be changed by calling setMinimal().

    \sa setPattern() setCaseSensitive() setWildcard() setMinimal()
*/
QRegExp::QRegExp( const QString& pattern, bool caseSensitive, bool wildcard )
{
    eng = 0;
    priv = new QRegExpPrivate;
    priv->pattern = pattern;
#ifndef QT_NO_REGEXP_WILDCARD
    priv->wc = wildcard;
#endif
    priv->min = FALSE;
    compile( caseSensitive );
}

/*!
    Constructs a regular expression as a copy of \a rx.

    \sa operator=()
*/
QRegExp::QRegExp( const QRegExp& rx )
{
    eng = 0;
    priv = new QRegExpPrivate;
    operator=( rx );
}

/*!
    Destroys the regular expression and cleans up its internal data.
*/
QRegExp::~QRegExp()
{
    derefEngine( eng, priv->rxpattern );
    delete priv;
}

/*!
    Copies the regular expression \a rx and returns a reference to the
    copy. The case sensitivity, wildcard and minimal matching options
    are also copied.
*/
QRegExp& QRegExp::operator=( const QRegExp& rx )
{
    rx.eng->ref();
    derefEngine( eng, priv->rxpattern );
    eng = rx.eng;
    priv->pattern = rx.priv->pattern;
    priv->rxpattern = rx.priv->rxpattern;
#ifndef QT_NO_REGEXP_WILDCARD
    priv->wc = rx.priv->wc;
#endif
    priv->min = rx.priv->min;
#ifndef QT_NO_REGEXP_CAPTURE
    priv->t = rx.priv->t;
    priv->capturedCache = rx.priv->capturedCache;
#endif
    priv->captured = rx.priv->captured;
    return *this;
}

/*!
    Returns TRUE if this regular expression is equal to \a rx;
    otherwise returns FALSE.

    Two QRegExp objects are equal if they have the same pattern
    strings and the same settings for case sensitivity, wildcard and
    minimal matching.
*/
bool QRegExp::operator==( const QRegExp& rx ) const
{
    return priv->pattern == rx.priv->pattern &&
	   eng->caseSensitive() == rx.eng->caseSensitive() &&
#ifndef QT_NO_REGEXP_WILDCARD
	   priv->wc == rx.priv->wc &&
#endif
	   priv->min == rx.priv->min;
}

/*!
    \fn bool QRegExp::operator!=( const QRegExp& rx ) const

    Returns TRUE if this regular expression is not equal to \a rx;
    otherwise returns FALSE.

    \sa operator==()
*/

/*!
    Returns TRUE if the pattern string is empty; otherwise returns
    FALSE.

    If you call exactMatch() with an empty pattern on an empty string
    it will return TRUE; otherwise it returns FALSE since it operates
    over the whole string. If you call search() with an empty pattern
    on \e any string it will return the start offset (0 by default)
    because the empty pattern matches the 'emptiness' at the start of
    the string. In this case the length of the match returned by
    matchedLength() will be 0.

    See QString::isEmpty().
*/

bool QRegExp::isEmpty() const
{
    return priv->pattern.isEmpty();
}

/*!
    Returns TRUE if the regular expression is valid; otherwise returns
    FALSE. An invalid regular expression never matches.

    The pattern <b>[a-z</b> is an example of an invalid pattern, since
    it lacks a closing square bracket.

    Note that the validity of a regexp may also depend on the setting
    of the wildcard flag, for example <b>*.html</b> is a valid
    wildcard regexp but an invalid full regexp.

    \sa errorString()
*/
bool QRegExp::isValid() const
{
    return eng->isValid();
}

/*!
    Returns the pattern string of the regular expression. The pattern
    has either regular expression syntax or wildcard syntax, depending
    on wildcard().

    \sa setPattern()
*/
QString QRegExp::pattern() const
{
    return priv->pattern;
}

/*!
    Sets the pattern string to \a pattern. The case sensitivity,
    wildcard and minimal matching options are not changed.

    \sa pattern()
*/
void QRegExp::setPattern( const QString& pattern )
{
    if ( priv->pattern != pattern ) {
	priv->pattern = pattern;
	compile( caseSensitive() );
    }
}

/*!
    Returns TRUE if case sensitivity is enabled; otherwise returns
    FALSE. The default is TRUE.

    \sa setCaseSensitive()
*/
bool QRegExp::caseSensitive() const
{
    return eng->caseSensitive();
}

/*!
    Sets case sensitive matching to \a sensitive.

    If \a sensitive is TRUE, <b>\\.txt$</b> matches \c{readme.txt} but
    not \c{README.TXT}.

    \sa caseSensitive()
*/
void QRegExp::setCaseSensitive( bool sensitive )
{
    if ( sensitive != eng->caseSensitive() )
	compile( sensitive );
}

#ifndef QT_NO_REGEXP_WILDCARD
/*!
    Returns TRUE if wildcard mode is enabled; otherwise returns FALSE.
    The default is FALSE.

    \sa setWildcard()
*/
bool QRegExp::wildcard() const
{
    return priv->wc;
}

/*!
    Sets the wildcard mode for the regular expression. The default is
    FALSE.

    Setting \a wildcard to TRUE enables simple shell-like wildcard
    matching. (See \link #wildcard-matching wildcard matching
    (globbing) \endlink.)

    For example, <b>r*.txt</b> matches the string \c{readme.txt} in
    wildcard mode, but does not match \c{readme}.

    \sa wildcard()
*/
void QRegExp::setWildcard( bool wildcard )
{
    if ( wildcard != priv->wc ) {
	priv->wc = wildcard;
	compile( caseSensitive() );
    }
}
#endif

/*!
    Returns TRUE if minimal (non-greedy) matching is enabled;
    otherwise returns FALSE.

    \sa setMinimal()
*/
bool QRegExp::minimal() const
{
    return priv->min;
}

/*!
    Enables or disables minimal matching. If \a minimal is FALSE,
    matching is greedy (maximal) which is the default.

    For example, suppose we have the input string "We must be
    \<b>bold\</b>, very \<b>bold\</b>!" and the pattern
    <b>\<b>.*\</b></b>. With the default greedy (maximal) matching,
    the match is "We must be <u>\<b>bold\</b>, very
    \<b>bold\</b></u>!". But with minimal (non-greedy) matching the
    first match is: "We must be <u>\<b>bold\</b></u>, very
    \<b>bold\</b>!" and the second match is "We must be \<b>bold\</b>,
    very <u>\<b>bold\</b></u>!". In practice we might use the pattern
    <b>\<b>[^\<]+\</b></b> instead, although this will still fail for
    nested tags.

    \sa minimal()
*/
void QRegExp::setMinimal( bool minimal )
{
    priv->min = minimal;
}

/*!
    Returns TRUE if \a str is matched exactly by this regular
    expression; otherwise returns FALSE. You can determine how much of
    the string was matched by calling matchedLength().

    For a given regexp string, R, exactMatch("R") is the equivalent of
    search("^R$") since exactMatch() effectively encloses the regexp
    in the start of string and end of string anchors, except that it
    sets matchedLength() differently.

    For example, if the regular expression is <b>blue</b>, then
    exactMatch() returns TRUE only for input \c blue. For inputs \c
    bluebell, \c blutak and \c lightblue, exactMatch() returns FALSE
    and matchedLength() will return 4, 3 and 0 respectively.

    Although const, this function sets matchedLength(),
    capturedTexts() and pos().

    \sa search() searchRev() QRegExpValidator
*/
bool QRegExp::exactMatch( const QString& str ) const
{
#ifndef QT_NO_REGEXP_CAPTURE
    priv->t = str;
    priv->capturedCache.clear();
#endif

    priv->captured = eng->match( str, 0, priv->min, TRUE, 0 );
    if ( priv->captured[1] == (int) str.length() ) {
	return TRUE;
    } else {
	priv->captured.detach();
	priv->captured[0] = 0;
	priv->captured[1] = eng->matchedLength();
	return FALSE;
    }
}

#ifndef QT_NO_COMPAT
/*! \obsolete

  Attempts to match in \a str, starting from position \a index.
  Returns the position of the match, or -1 if there was no match.

  The length of the match is stored in \a *len, unless \a len is a
  null pointer.

  If \a indexIsStart is TRUE (the default), the position \a index in
  the string will match the start of string anchor, <b>^</b>, in the
  regexp, if present. Otherwise, position 0 in \a str will match.

  Use search() and matchedLength() instead of this function.

  \sa QString::mid() QConstString
*/
int QRegExp::match( const QString& str, int index, int *len,
		    bool indexIsStart ) const
{
    int pos = search( str, index, indexIsStart ? CaretAtOffset : CaretAtZero );
    if ( len != 0 )
	*len = matchedLength();
    return pos;
}
#endif // QT_NO_COMPAT

/*!
    \overload

    This convenience function searches with a \c CaretMode of \c
    CaretAtZero which is the most common usage.
*/

int QRegExp::search( const QString& str, int offset ) const
{
    return search( str, offset, CaretAtZero );
}

/*!
    Attempts to find a match in \a str from position \a offset (0 by
    default). If \a offset is -1, the search starts at the last
    character; if -2, at the next to last character; etc.

    Returns the position of the first match, or -1 if there was no
    match.

    The \a caretMode parameter can be used to instruct whether <b>^</b>
    should match at index 0 or at \a offset.

    You might prefer to use QString::find(), QString::contains() or
    even QStringList::grep(). To replace matches use
    QString::replace().

    Example:
    \code
	QString str = "offsets: 1.23 .50 71.00 6.00";
	QRegExp rx( "\\d*\\.\\d+" );    // primitive floating point matching
	int count = 0;
	int pos = 0;
	while ( (pos = rx.search(str, pos)) != -1 ) {
	    count++;
	    pos += rx.matchedLength();
	}
	// pos will be 9, 14, 18 and finally 24; count will end up as 4
    \endcode

    Although const, this function sets matchedLength(),
    capturedTexts() and pos().

    \sa searchRev() exactMatch()
*/

int QRegExp::search( const QString& str, int offset, CaretMode caretMode ) const
{
    if ( offset < 0 )
	offset += str.length();
#ifndef QT_NO_REGEXP_CAPTURE
    priv->t = str;
    priv->capturedCache.clear();
#endif
    priv->captured = eng->match( str, offset, priv->min, FALSE,
				 caretIndex(offset, caretMode) );
    return priv->captured[0];
}


/*!
    \overload

    This convenience function searches with a \c CaretMode of \c
    CaretAtZero which is the most common usage.
*/

int QRegExp::searchRev( const QString& str, int offset ) const
{
    return searchRev( str, offset, CaretAtZero );
}

/*!
    Attempts to find a match backwards in \a str from position \a
    offset. If \a offset is -1 (the default), the search starts at the
    last character; if -2, at the next to last character; etc.

    Returns the position of the first match, or -1 if there was no
    match.

    The \a caretMode parameter can be used to instruct whether <b>^</b>
    should match at index 0 or at \a offset.

    Although const, this function sets matchedLength(),
    capturedTexts() and pos().

    \warning Searching backwards is much slower than searching
    forwards.

    \sa search() exactMatch()
*/

int QRegExp::searchRev( const QString& str, int offset,
			CaretMode caretMode ) const
{
    if ( offset < 0 )
	offset += str.length();
#ifndef QT_NO_REGEXP_CAPTURE
    priv->t = str;
    priv->capturedCache.clear();
#endif
    if ( offset < 0 || offset > (int) str.length() ) {
	priv->captured.detach();
	priv->captured.fill( -1 );
	return -1;
    }

    while ( offset >= 0 ) {
	priv->captured = eng->match( str, offset, priv->min, TRUE,
				     caretIndex(offset, caretMode) );
	if ( priv->captured[0] == offset )
	    return offset;
	offset--;
    }
    return -1;
}

/*!
    Returns the length of the last matched string, or -1 if there was
    no match.

    \sa exactMatch() search() searchRev()
*/
int QRegExp::matchedLength() const
{
    return priv->captured[1];
}

#ifndef QT_NO_REGEXP_CAPTURE
/*! 
  Returns the number of captures contained in the regular expression.
 */
int QRegExp::numCaptures() const
{
    return eng->numCaptures();
}



/*!
    Returns a list of the captured text strings.

    The first string in the list is the entire matched string. Each
    subsequent list element contains a string that matched a
    (capturing) subexpression of the regexp.

    For example:
    \code
	QRegExp rx( "(\\d+)(\\s*)(cm|inch(es)?)" );
	int pos = rx.search( "Length: 36 inches" );
	QStringList list = rx.capturedTexts();
	// list is now ( "36 inches", "36", " ", "inches", "es" )
    \endcode

    The above example also captures elements that may be present but
    which we have no interest in. This problem can be solved by using
    non-capturing parentheses:

    \code
	QRegExp rx( "(\\d+)(?:\\s*)(cm|inch(?:es)?)" );
	int pos = rx.search( "Length: 36 inches" );
	QStringList list = rx.capturedTexts();
	// list is now ( "36 inches", "36", "inches" )
    \endcode

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
	QStringList list = rx.capturedTexts();
	QStringList::Iterator it = list.begin();
	while( it != list.end() ) {
	    myProcessing( *it );
	    ++it;
	}
    \endcode

    Some regexps can match an indeterminate number of times. For
    example if the input string is "Offsets: 12 14 99 231 7" and the
    regexp, \c{rx}, is <b>(\\d+)+</b>, we would hope to get a list of
    all the numbers matched. However, after calling
    \c{rx.search(str)}, capturedTexts() will return the list ( "12",
    "12" ), i.e. the entire match was "12" and the first subexpression
    matched was "12". The correct approach is to use cap() in a \link
    #cap_in_a_loop loop \endlink.

    The order of elements in the string list is as follows. The first
    element is the entire matching string. Each subsequent element
    corresponds to the next capturing open left parentheses. Thus
    capturedTexts()[1] is the text of the first capturing parentheses,
    capturedTexts()[2] is the text of the second and so on
    (corresponding to $1, $2, etc., in some other regexp languages).

    \sa cap() pos() exactMatch() search() searchRev()
*/
QStringList QRegExp::capturedTexts()
{
    if ( priv->capturedCache.isEmpty() ) {
	for ( int i = 0; i < (int) priv->captured.size(); i += 2 ) {
	    QString m;
	    if ( priv->captured[i + 1] == 0 )
		m = QString::fromLatin1( "" );
	    else if ( priv->captured[i] >= 0 )
		m = priv->t.mid( priv->captured[i],
				 priv->captured[i + 1] );
	    priv->capturedCache.append( m );
	}
	priv->t = QString::null;
    }
    return priv->capturedCache;
}

/*!
    Returns the text captured by the \a nth subexpression. The entire
    match has index 0 and the parenthesized subexpressions have
    indices starting from 1 (excluding non-capturing parentheses).

    \code
    QRegExp rxlen( "(\\d+)(?:\\s*)(cm|inch)" );
    int pos = rxlen.search( "Length: 189cm" );
    if ( pos > -1 ) {
	QString value = rxlen.cap( 1 ); // "189"
	QString unit = rxlen.cap( 2 );  // "cm"
	// ...
    }
    \endcode

    The order of elements matched by cap() is as follows. The first
    element, cap(0), is the entire matching string. Each subsequent
    element corresponds to the next capturing open left parentheses.
    Thus cap(1) is the text of the first capturing parentheses, cap(2)
    is the text of the second, and so on.

    \target cap_in_a_loop
    Some patterns may lead to a number of matches which cannot be
    determined in advance, for example:

    \code
    QRegExp rx( "(\\d+)" );
    str = "Offsets: 12 14 99 231 7";
    QStringList list;
    pos = 0;
    while ( pos >= 0 ) {
	pos = rx.search( str, pos );
	if ( pos > -1 ) {
	    list += rx.cap( 1 );
	    pos  += rx.matchedLength();
	}
    }
    // list contains "12", "14", "99", "231", "7"
    \endcode

    \sa capturedTexts() pos() exactMatch() search() searchRev()
*/
QString QRegExp::cap( int nth )
{
    if ( nth < 0 || nth >= (int) priv->captured.size() / 2 )
	return QString::null;
    else
	return capturedTexts()[nth];
}

/*!
    Returns the position of the \a nth captured text in the searched
    string. If \a nth is 0 (the default), pos() returns the position
    of the whole match.

    Example:
    \code
    QRegExp rx( "/([a-z]+)/([a-z]+)" );
    rx.search( "Output /dev/null" );    // returns 7 (position of /dev/null)
    rx.pos( 0 );                        // returns 7 (position of /dev/null)
    rx.pos( 1 );                        // returns 8 (position of dev)
    rx.pos( 2 );                        // returns 12 (position of null)
    \endcode

    For zero-length matches, pos() always returns -1. (For example, if
    cap(4) would return an empty string, pos(4) returns -1.) This is
    due to an implementation tradeoff.

    \sa capturedTexts() exactMatch() search() searchRev()
*/
int QRegExp::pos( int nth )
{
    if ( nth < 0 || nth >= (int) priv->captured.size() / 2 )
	return -1;
    else
	return priv->captured[2 * nth];
}

/*!
  Returns a text string that explains why a regexp pattern is
  invalid the case being; otherwise returns "no error occurred".

  \sa isValid()
*/
QString QRegExp::errorString()
{
    if ( isValid() ) {
	return QString( RXERR_OK );
    } else {
	return eng->errorString();
    }
}
#endif

/*!
  Returns the string \a str with every regexp special character
  escaped with a backslash. The special characters are $, (, ), *, +,
  ., ?, [, \, ], ^, {, | and }.

  Example:
  \code
     s1 = QRegExp::escape( "bingo" );   // s1 == "bingo"
     s2 = QRegExp::escape( "f(x)" );    // s2 == "f\\(x\\)"
  \endcode

  This function is useful to construct regexp patterns dynamically:

  \code
    QRegExp rx( "(" + QRegExp::escape(name) +
		"|" + QRegExp::escape(alias) + ")" );
  \endcode
*/
QString QRegExp::escape( const QString& str )
{
    static const char meta[] = "$()*+.?[\\]^{|}";
    QString quoted = str;
    int i = 0;

    while ( i < (int) quoted.length() ) {
	if ( strchr(meta, quoted[i].latin1()) != 0 )
	    quoted.insert( i++, "\\" );
	i++;
    }
    return quoted;
}

void QRegExp::compile( bool caseSensitive )
{
    derefEngine( eng, priv->rxpattern );
#ifndef QT_NO_REGEXP_WILDCARD
    if ( priv->wc )
	priv->rxpattern = wc2rx( priv->pattern );
    else
#endif
	priv->rxpattern = priv->pattern.isNull() ? QString::fromLatin1( "" )
			  : priv->pattern;
    eng = newEngine( priv->rxpattern, caseSensitive );
#ifndef QT_NO_REGEXP_CAPTURE
    priv->t = QString();
    priv->capturedCache.clear();
#endif
    priv->captured.detach();
    priv->captured.fill( -1, 2 + 2 * eng->numCaptures() );
}

int QRegExp::caretIndex( int offset, CaretMode caretMode )
{
    if ( caretMode == CaretAtZero ) {
	return 0;
    } else if ( caretMode == CaretAtOffset ) {
	return offset;
    } else { // CaretWontMatch
	return -1;
    }
}

#endif // QT_NO_REGEXP
