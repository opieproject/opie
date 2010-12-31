/*
*   Copyright (C) 2007 Kevin Krammer <kevin.krammer@gmx.at>
*
*   Permission is hereby granted, free of charge, to any person obtaining a
*   copy of this software and associated documentation files (the "Software"),
*   to deal in the Software without restriction, including without limitation
*   the rights to use, copy, modify, merge, publish, distribute, sublicense,
*   and/or sell copies of the Software, and to permit persons to whom the
*   Software is furnished to do so, subject to the following conditions:
*
*   The above copyright notice and this permission notice shall be included
*   in all copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
*   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
*   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
*   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
*   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
*   OTHER DEALINGS IN THE SOFTWARE.
*/

#if !defined(METHODGEN_H_INCLUDED)
#define METHODGEN_H_INCLUDED

// Qt includes
#include <qmap.h>
#include <qstringlist.h>

// forward declarations
class QTextStream;

class Argument
{
public:
    enum Direction
    {
        In,
        Out
    };

    QString name;
    QString annotatedType;
    QString signature;
    QString accessor;
    QString subAccessor;
    QString containerClass;
    Direction direction;
    bool isPrimitive;

    QStringList forwardDeclarations;
    QMap<QString, QStringList> headerIncludes;
    QMap<QString, QStringList> sourceIncludes;

    QString dbusSignature;
};

class Method
{
public:
    QString name;
    QValueList<Argument> arguments;
    bool noReply;
    bool async;
};

class Property : public Argument
{
public:
    bool read;
    bool write;
};

class Class
{
public:
    enum Role
    {
        Interface,
        Proxy,
        Node
    };

    QString name;
    QString dbusName;
    QStringList namespaces;
    QValueList<Method> methods;
    QValueList<Method> signals;
    QValueList<Property> properties;

    QValueList<Method> asyncMethods;
    QValueList<Method> asyncReplySignals;
    QValueList<Method> asyncReplyMethods;
};

class MethodGenerator
{
public:
    static bool extractMethods(const QDomElement& interfaceElement,
                               Class& classData);

    static void writeMethodDeclaration(const Method& method, bool pureVirtual,
                                       bool withError, QTextStream& stream);

    static void writePropertyDeclaration(const Property& property, bool pureVirtual,
                                         QTextStream& stream);

    static void writeMethodCallDeclaration(const Method& method,
                                           QTextStream& stream);

    static void writeMethodCall(const Class& classData, const Method& method,
                                QTextStream& stream);

    static void writeSignalEmitter(const Class& classData, const Method& method,
                                   QTextStream& stream);

    static void writeInterfaceAsyncReplyHandler(const Class& classData,
                                                const Method& method,
                                                QTextStream& stream);

    static void writeInterfaceMainMethod(const Class& classData,
                                         QTextStream& stream);

    static void writeSignalHandler(const Class& classData, QTextStream& stream);

    static void writeProxyBegin(const Class& classData, QTextStream& stream);

    static void writeProxyMethod(const QString& className, const Method& method,
                                 QTextStream& stream);

    static void writeProxyGenericProperty(const Class& classData,
                                          QTextStream& stream);

    static void writeProxyProperty(const Class& classData, const Property& property,
                                   QTextStream& stream);

    static void writeProxyAsyncReplyHandler(const Class& classData,
                                            QTextStream& stream);

    static void writeIntrospectionDataMethod(const Class& classData,
                                             QTextStream& stream);

    static void writeNodePrivate(const Class& classData, QTextStream& stream);

    static void writeNodeBegin(const Class& classData, QTextStream& stream);

    static void writeNodeMethods(const Class& classData,
            const QValueList<Class>& interfaces, QTextStream& stream);
};

#endif

// End of File
