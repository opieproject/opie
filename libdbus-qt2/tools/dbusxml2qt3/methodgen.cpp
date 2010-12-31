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

// Qt includes
#include <qdom.h>
#include <qtextstream.h>

// local includes
#include "methodgen.h"

static bool parseDBusSignature(const QString& signature, Argument& argument)
{
    argument.dbusSignature = signature;

    if (signature.length() == 1)
    {
        if (signature == "b")
        {
            argument.signature = "bool";
            argument.accessor = "Bool";
            argument.isPrimitive = true;
        }
        else if (signature == "y")
        {
            argument.signature = "Q_UINT8";
            argument.accessor = "Byte";
            argument.isPrimitive = true;
        }
        else if (signature == "n")
        {
            argument.signature = "Q_INT16";
            argument.accessor = "Int16";
            argument.isPrimitive = true;
        }
        else if (signature == "q")
        {
            argument.signature = "Q_UINT16";
            argument.accessor = "UInt16";
            argument.isPrimitive = true;
        }
        else if (signature == "i")
        {
            argument.signature = "Q_INT32";
            argument.accessor = "Int32";
            argument.isPrimitive = true;
        }
        else if (signature == "u")
        {
            argument.signature = "Q_UINT32";
            argument.accessor = "UInt32";
            argument.isPrimitive = true;
        }
        else if (signature == "x")
        {
            argument.signature = "Q_INT64";
            argument.accessor = "Int64";
            argument.isPrimitive = true;
        }
        else if (signature == "t")
        {
            argument.signature = "Q_UINT64";
            argument.accessor = "UInt64";
            argument.isPrimitive = true;
        }
        else if (signature == "d")
        {
            argument.signature = "double";
            argument.accessor = "Double";
            argument.isPrimitive = true;
        }
        else if (signature == "s")
        {
            argument.signature = "QString";
            argument.accessor = "String";
            argument.isPrimitive = false;

            argument.forwardDeclarations.append("class QString");
            argument.sourceIncludes["Qt"].append("<qstring.h>");
        }
        else if (signature == "o")
        {
            argument.signature = "QDBusObjectPath";
            argument.accessor = "ObjectPath";
            argument.isPrimitive = false;

            argument.forwardDeclarations.append("class QDBusObjectPath");
            argument.sourceIncludes["qdbus"].append("<dbus/qdbusobjectpath.h>");
        }
        else if (signature == "v")
        {
            argument.signature = "QDBusVariant";
            argument.accessor = "Variant";
            argument.isPrimitive = false;

            argument.forwardDeclarations.append("class QDBusVariant");
            argument.sourceIncludes["qdbus"].append("<dbus/qdbusvariant.h>");
        }
        else
            return false;
    }
    else if (signature.startsWith("a"))
    {
        if (signature == "as")
        {
            argument.signature = "QStringList";
            argument.accessor = "List";
            argument.subAccessor = "QStringList";
            argument.isPrimitive = false;

            argument.forwardDeclarations.append("class QStringList");

            argument.sourceIncludes["qdbus"].append("<dbus/qdbusdatalist.h>");
            argument.sourceIncludes["Qt"].append("<qstringlist.h>");
        }
        else if (signature.startsWith("a{"))
        {
            int from = signature.find("{");
            int to = signature.findRev("}");
            if (from == -1 || to == -1 || (to - from - 1) < 2) return false;

            QString dictSignature = signature.mid(from + 1, (to - from - 1));

            Argument key;
            if (!parseDBusSignature(dictSignature.left(1), key)) return false;

            Argument value;
            if (parseDBusSignature(dictSignature.mid(1), value))
            {
                if (!value.subAccessor.isEmpty())
                {
                    argument.isPrimitive = false;
                    argument.containerClass = "QDBusDataMap<" + key.signature + ">";
                    argument.signature = "QDBusDataMap<" + key.signature + ">";
                    argument.accessor = key.accessor + "KeyMap";

                    argument.forwardDeclarations.append("template <typename K> class QDBusDataMap");
                    argument.forwardDeclarations += key.forwardDeclarations;

                    argument.sourceIncludes = key.sourceIncludes;
                    argument.sourceIncludes["qdbus"].append("<dbus/qdbusdata.h>");
                    argument.sourceIncludes["qdbus"].append("<dbus/qdbusdatamap.h>");
                }
                else
                {
                    argument.isPrimitive = false;
                    argument.containerClass = "QDBusDataMap<" + key.signature + ">";
                    argument.signature = "QMap<" + key.signature +
                                        ", " + value.signature + ">";
                    argument.accessor = key.accessor + "KeyMap";
                    argument.subAccessor = value.accessor + "Map";

                    argument.forwardDeclarations.append("template <typename K, typename V> class QMap");
                    argument.forwardDeclarations += key.forwardDeclarations;
                    argument.forwardDeclarations += value.forwardDeclarations;

                    argument.sourceIncludes = key.sourceIncludes;
                    argument.sourceIncludes["Qt"].append("<qmap.h>");
                    argument.sourceIncludes["qdbus"].append("<dbus/qdbusdata.h>");
                    argument.sourceIncludes["qdbus"].append("<dbus/qdbusdatamap.h>");

                    QMap<QString, QStringList>::const_iterator it =
                        value.sourceIncludes.begin();
                    QMap<QString, QStringList>::const_iterator endIt =
                        value.sourceIncludes.end();
                    for (; it != endIt; ++it)
                    {
                        argument.sourceIncludes[it.key()] += it.data();
                    }
                }
            }
            else
            {
                argument.isPrimitive = false;
                argument.containerClass = "QDBusDataMap<" + key.signature + ">";
                argument.signature = "QDBusDataMap<" + key.signature + ">";
                argument.accessor = key.accessor + "KeyMap";

                argument.forwardDeclarations.append("template <typename K> class QDBusDataMap");
                argument.forwardDeclarations += key.forwardDeclarations;

                argument.sourceIncludes = key.sourceIncludes;
                argument.sourceIncludes["qdbus"].append("<dbus/qdbusdata.h>");
                argument.sourceIncludes["qdbus"].append("<dbus/qdbusdatamap.h>");
            }
        }
        else
        {
            QString itemSignature = signature.mid(1);

            Argument item;
            if (parseDBusSignature(itemSignature, item))
            {
                argument.isPrimitive = false;
                argument.signature = "QValueList<" + item.signature + ">";
                argument.accessor = "List";
                argument.subAccessor = item.accessor + "List";
                argument.containerClass = "QDBusDataList";

                argument.forwardDeclarations.append("class QDBusDataList");
                argument.forwardDeclarations.append("template <typename T> class QValueList");
                argument.forwardDeclarations += item.forwardDeclarations;

                argument.sourceIncludes["Qt"].append("<qvaluelist.h>");
                argument.sourceIncludes["qdbus"].append("<dbus/qdbusdatalist.h>");

                QMap<QString, QStringList>::const_iterator it =
                    item.sourceIncludes.begin();
                QMap<QString, QStringList>::const_iterator endIt =
                    item.sourceIncludes.end();
                for (; it != endIt; ++it)
                {
                    argument.sourceIncludes[it.key()] += it.data();
                }
            }
            else
            {
                argument.signature = "QDBusDataList";
                argument.accessor = "List";
                argument.isPrimitive = false;

                argument.forwardDeclarations.append("class QDBusDataList");

                argument.sourceIncludes["qdbus"].append("<dbus/qdbusdatalist.h>");
            }
        }
    }
    else
        return false;

    return true;
}

static QMap<QString, QString> extractTypeAnnotations(const QDomElement& element)
{
    const QString annotationPrefix = "org.freedesktop.DBus.Qt3.Type.";

    QMap<QString, QString> annotations;

    QDomNode node = element.firstChild();
    for (uint count = 1; !node.isNull(); node = node.nextSibling(), ++count)
    {
        if (!node.isElement()) continue;

        QDomElement element = node.toElement();
        if (element.tagName() != "annotation") continue;

        QString name = element.attribute("name");
        if (name.isEmpty()) continue;

        QString value = element.attribute("value").stripWhiteSpace();
        if (value.isEmpty()) continue;

        if (!name.startsWith(annotationPrefix)) continue;

        QString arg = name.mid(annotationPrefix.length());

        annotations.insert(arg, value);
    }

    return annotations;
}

static bool hasAnnotation(const QDomElement& element, const QString& annotation, QString* value = 0)
{
    for (QDomNode node = element.firstChild(); !node.isNull();
         node = node.nextSibling())
    {
        if (!node.isElement()) continue;

        QDomElement childElement = node.toElement();
        if (childElement.tagName() != "annotation") continue;
        if (childElement.attribute("name") != annotation) continue;

        if (value != 0) *value = childElement.attribute("value");
        return true;
    }

    return false;
}

static QValueList<Argument> extractArguments(const QDomElement& methodElement,
        Class& classData)
{
    QMap<QString, QString> argAnnotations = extractTypeAnnotations(methodElement);

    QValueList<Argument> arguments;

    bool isSignal = methodElement.tagName() == "signal";

    uint inCount  = 0;
    uint outCount = 0;
    for (QDomNode node = methodElement.firstChild(); !node.isNull();
         node = node.nextSibling())
    {
        if (!node.isElement()) continue;

        QDomElement element = node.toElement();
        if (element.tagName() != "arg") continue;
        if (element.attribute("type").isEmpty()) continue;

        Argument argument;
        argument.name = element.attribute("name");
        if (argument.name.isEmpty())
            argument.name = QString("arg%1").arg(inCount + outCount);

        argument.direction = Argument::In;
        if (!isSignal && element.attribute("direction", "in") == "out")
            argument.direction = Argument::Out;

        QString annotation;
        if (!isSignal && argument.direction == Argument::In)
        {
            annotation = argAnnotations[QString("In%1").arg(inCount)];
            ++inCount;
        }
        else
        {
            annotation = argAnnotations[QString("Out%1").arg(outCount)];
            ++outCount;
        }

        if (!annotation.isEmpty())
        {
            // just assume nobody uses annotations for primitives
            argument.annotatedType = annotation;
            argument.signature     = annotation;
            argument.isPrimitive   = false;
            argument.dbusSignature = element.attribute("type");

            QString includeBase =
                QString("\"%1type%2.h\"").arg(classData.name.lower());

            argument.headerIncludes["local"].append(includeBase.arg("declarations"));
            argument.sourceIncludes["local"].append(includeBase.arg("includes"));
            argument.sourceIncludes["qdbus"].append("<dbus/qdbusdataconverter.h>");
        }
        else if (!parseDBusSignature(element.attribute("type"), argument))
        {
            argument.signature = "QDBusData";
            argument.isPrimitive = false;

            argument.forwardDeclarations.append("class QDBusData");
            argument.sourceIncludes["qdbus"].append("<dbus/qdbusdata.h>");
        }

        arguments.append(argument);
    }

    return arguments;
}

static void writeVariable(const Argument& argument, uint index,
        const QString& prefix, QTextStream& stream)
{
    stream << prefix << argument.signature << " _" << argument.name;
    if (argument.direction == Argument::In)
    {
        if (!argument.annotatedType.isEmpty())
        {
            stream << ";" << endl;

            // TODO: error handling?
            stream << prefix << "QDBusDataConverter::convertFromQDBusData<"
                   << argument.annotatedType
                   << QString(">(message[%1], _").arg(index)
                   << argument.name << ")";
        }
        else if (!argument.accessor.isEmpty())
        {
            stream << QString::fromUtf8(" = message[%1].to").arg(index);
            stream << argument.accessor;

            if (!argument.subAccessor.isEmpty())
            {
                stream << QString("().to%1").arg(argument.subAccessor);
            }

            stream << "()";
        }
        else
            stream << QString::fromUtf8(" = message[%1]").arg(index);
    }

    stream << ";" << endl;
}

static void writeVariables(const QString& prefix, const Method& method,
        QTextStream& stream)
{
    uint count = 0;
    QValueList<Argument>::const_iterator it    = method.arguments.begin();
    QValueList<Argument>::const_iterator endIt = method.arguments.end();
    for (; it != endIt; ++it)
    {
        writeVariable(*it, count, prefix, stream);

        if ((*it).direction == Argument::In) ++count;
    }
}

static void writeSignalEmit(const Method& signal, QTextStream& stream)
{
    stream << "        emit " << signal.name << "(";

    QValueList<Argument>::const_iterator it    = signal.arguments.begin();
    QValueList<Argument>::const_iterator endIt = signal.arguments.end();
    for (; it != endIt;)
    {
        stream << "_" << (*it).name;

        ++it;
        if (it != endIt) stream << ", ";
    }

    stream << ");" << endl;
}

static void writeMethodIntrospection(const Method& method, bool& firstArgument,
    QTextStream& stream)
{
    stream << "    methodElement.setAttribute(\"name\", \""
           << method.name << "\");" << endl;

    QValueList<Argument>::const_iterator it    = method.arguments.begin();
    QValueList<Argument>::const_iterator endIt = method.arguments.end();
    for (; it != endIt; ++it)
    {
        stream << endl;
        if (firstArgument)
        {
            firstArgument = false;

            stream << "    QDomElement argumentElement = document.createElement("
                   << "\"arg\");" << endl;
        }
        else
        {
            stream << "    argumentElement = document.createElement("
                   << "\"arg\");" << endl;
        }

        stream << "    argumentElement.setAttribute(\"name\",      \""
               << (*it).name << "\");" << endl;

        stream << "    argumentElement.setAttribute(\"type\",      \""
               << (*it).dbusSignature << "\");" << endl;

        stream << "    argumentElement.setAttribute(\"direction\", \""
               << ((*it).direction == Argument::In ? "in" : "out") << "\");"
               << endl;

        stream << "    methodElement.appendChild(argumentElement);" << endl;
    }
    stream << endl;
}

static void writeNodeInitialization(const Class& classData,
        const QValueList<Class>& interfaces, QTextStream& stream)
{
    stream << "bool " << classData.name
           << "::registerObject(const QDBusConnection& connection, "
           << "const QString& path)" << endl;
    stream << "{" << endl;
    stream << "    if (path.isEmpty()) return false;" << endl;
    stream << endl;

    stream << "    if (!m_private->objectPath.isEmpty()) unregisterObject();"
           << endl;
    stream << endl;

    stream << "    m_private->connection = connection;" << endl;
    stream << "    m_private->objectPath = path;" << endl;
    stream << endl;
    stream << "    if (!m_private->connection.registerObject(path, this))" << endl;
    stream << "    {" << endl;
    stream << "        m_private->connection = QDBusConnection();" << endl;
    stream << "        m_private->objectPath = QString::null;" << endl;
    stream << endl;
    stream << "        return false;" << endl;
    stream << "    }" << endl;
    stream << endl;

    stream << "    if (m_private->interfaces.isEmpty())" << endl;
    stream << "    {" << endl;
    stream << "        QString name = \"org.freedesktop.DBus.Introspectable\";"
           << endl;
    stream << "        QDBusObjectBase* interface = m_private;" << endl;
    stream << "        m_private->interfaces.insert(name, interface);" << endl;

    QValueList<Class>::const_iterator it    = interfaces.begin();
    QValueList<Class>::const_iterator endIt = interfaces.end();
    for (; it != endIt; ++it)
    {
        stream << endl;
        stream << "        name = \"" << (*it).dbusName << "\";" << endl;
        stream << "        interface = createInterface(name);" << endl;
        stream << "        Q_ASSERT(interface != 0);" << endl;
        stream << "        m_private->interfaces.insert(name, interface);" << endl;
    }

    stream << "    }" << endl;
    stream << endl;
    stream << "    return true;" << endl;
    stream << "}" << endl;
    stream << endl;
}
static void writeNodeIntrospection(const Class& classData,
        const QValueList<Class>& interfaces, QTextStream& stream)
{
    stream << "void " << classData.name << "::Private"
           << "::cacheIntrospectionData()" << endl;
    stream << "{" << endl;

    stream << "    QDomDocument doc;" << endl;
    stream << "    QDomElement nodeElement = doc.createElement(\"node\");" << endl;
    stream << "    QDomElement interfaceElement = doc.createElement(\"interface\");"
           << endl;
    stream << "    org::freedesktop::DBus::Introspectable"
           << "::buildIntrospectionData(interfaceElement);" << endl;
    stream << "    nodeElement.appendChild(interfaceElement);" << endl;

    QValueList<Class>::const_iterator it    = interfaces.begin();
    QValueList<Class>::const_iterator endIt = interfaces.end();
    for (; it != endIt; ++it)
    {
        if ((*it).dbusName == "org.freedesktop.DBus.Introspectable") continue;

        stream << endl;
        stream << "    interfaceElement = doc.createElement(\"interface\");"
               << endl;
        stream << "    " << (*it).namespaces.join("::") + "::" + (*it).name
               << "::buildIntrospectionData(interfaceElement);" << endl;
        stream << "    nodeElement.appendChild(interfaceElement);" << endl;
    }

    stream << endl;
    stream << "    doc.appendChild(nodeElement);" << endl;
    stream << endl;

    stream << "    introspectionData = \"<!DOCTYPE node PUBLIC \\\""
           << "-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\\\"\\n"
           << "\\\"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd"
           << "\\\">\\n\";" << endl;
    stream << "    introspectionData += doc.toString();" << endl;
    stream << "}" << endl;
    stream << endl;
}

bool MethodGenerator::extractMethods(const QDomElement& interfaceElement,
        Class& classData)
{
    QMap<QString, QString> propertyAnnotations =
        extractTypeAnnotations(interfaceElement);

    uint propertyCount = 0;
    for (QDomNode node = interfaceElement.firstChild(); !node.isNull();
         node = node.nextSibling())
    {
        if (!node.isElement()) continue;

        QDomElement element = node.toElement();
        if (element.attribute("name").isEmpty()) continue;

        if (element.tagName() == "method" || element.tagName() == "signal")
        {
            Method method;
            method.name = element.attribute("name");
            method.arguments = extractArguments(element, classData);
            method.noReply = false;
            method.async = false;

            if (element.tagName() == "method")
            {
                method.async = hasAnnotation(element, "org.freedesktop.DBus.GLib.Async");
                classData.methods.append(method);
            }
            else
                classData.signals.append(method);
        }
        else if (element.tagName() == "property")
        {
            Property property;
            property.name  = element.attribute("name");
            property.read  = element.attribute("access").find("read")  != -1;
            property.write = element.attribute("access").find("write") != -1;

            QString annotation =
                propertyAnnotations[QString("Property%1").arg(propertyCount)];

            if (!annotation.isEmpty())
            {
                property.annotatedType = annotation;
                property.signature     = annotation;
                property.dbusSignature = element.attribute("type");
                property.isPrimitive   = false;

                QString includeBase =
                    QString("\"%1type%2.h\"").arg(classData.name.lower());

                property.headerIncludes["local"].append(includeBase.arg("declarations"));
                property.sourceIncludes["local"].append(includeBase.arg("includes"));
                property.sourceIncludes["qdbus"].append("<dbus/qdbusdataconverter.h>");
            }
            else if (!parseDBusSignature(element.attribute("type"), property))
            {
                property.signature = "QDBusData";
                property.isPrimitive = false;

                property.forwardDeclarations.append("class QDBusData");
                property.sourceIncludes["qdbus"].append("<dbus/qdbusdata.h>");
            }

            classData.properties.append(property);
            ++propertyCount;
        }
    }

    return !classData.methods.isEmpty() || !classData.signals.isEmpty() ||
           !classData.properties.isEmpty();
}

void MethodGenerator::writeMethodDeclaration(const Method& method, bool pureVirtual,
        bool withError, QTextStream& stream)
{
    stream << method.name << "(";

    QValueList<Argument>::const_iterator it    = method.arguments.begin();
    QValueList<Argument>::const_iterator endIt = method.arguments.end();
    for (; it != endIt;)
    {
        if (!(*it).isPrimitive && (*it).direction == Argument::In)
            stream << "const ";

        stream << (*it).signature;

        if (!(*it).isPrimitive || (*it).direction == Argument::Out) stream << "&";

        stream << " " << (*it).name;

        ++it;
        if (it != endIt || withError) stream << ", ";
    }

    if (withError)
        stream << "QDBusError& error)";
    else
        stream << ")";

    if (pureVirtual)
        stream << " = 0;" << endl;
    else
        stream << ";" << endl;

    stream << endl;
}

void MethodGenerator::writePropertyDeclaration(const Property& property,
        bool pureVirtual, QTextStream& stream)
{
    if (property.write)
    {
        stream << "    virtual void set" << property.name << "(";

        if (!property.isPrimitive) stream << "const ";

        stream << property.signature;

        if (!property.isPrimitive) stream << "&";

        stream << " value, QDBusError& error)";

        if (pureVirtual)
            stream << " = 0;" << endl;
        else
            stream << ";" << endl;
    }

    if (property.read)
    {
        stream << "    virtual " << property.signature << " get"
               << property.name << "(QDBusError& error) const";

        if (pureVirtual)
            stream << " = 0;" << endl;
        else
            stream << ";" << endl;
    }

    if (property.read || property.write) stream << endl;
}

void MethodGenerator::writeMethodCallDeclaration(const Method& method,
        QTextStream& stream)
{
    if (method.async)
        stream << "void call" << method.name << "Async";
    else
        stream << "QDBusMessage call" << method.name;

    stream << "(const QDBusMessage& message);" << endl;
    stream << endl;
}

void MethodGenerator::writeMethodCall(const Class& classData,
        const Method& method, QTextStream& stream)
{
    if (method.async)
        stream << "void " << classData.name << "::call" << method.name << "Async";
    else
        stream << "QDBusMessage " << classData.name << "::call" << method.name;

    stream << "(const QDBusMessage& message)" << endl;

    stream << "{" << endl;

    if (method.async)
    {
        // FIXME: using writeVariables by removing asyncCallId argument
        Method reducedMethod = method;
        reducedMethod.arguments.pop_front();

        writeVariables("    ", reducedMethod, stream);
    }
    else
    {
        stream << "    QDBusError   error;" << endl;
        stream << "    QDBusMessage reply;" << endl;
        stream << endl;

        writeVariables("    ", method, stream);
    }

    stream << endl;

    if (method.async)
    {
        stream << "    int _asyncCallId = 0;" << endl;
        stream << "    while (m_asyncCalls.find(_asyncCallId) != m_asyncCalls.end())"
               << endl;
        stream << "    {" << endl;
        stream << "        ++_asyncCallId;" << endl;
        stream << "    }" << endl;
        stream << "    m_asyncCalls.insert(_asyncCallId, message);" << endl;
        stream << endl;

        stream << "    " << method.name << "Async(";
    }
    else
        stream << "    if (" << method.name << "(";

    QValueList<Argument>::const_iterator it    = method.arguments.begin();
    QValueList<Argument>::const_iterator endIt = method.arguments.end();
    while (it != endIt)
    {
        stream << "_" << (*it).name;

        ++it;
        if (it != endIt) stream << ", ";
    }

    if (method.async)
    {
        stream << ");" << endl;
        stream << endl;

        stream << "    return;" << endl;
        stream << "}" << endl;
        stream << endl;
        return;
    }

	if (method.arguments.count() > 0) stream << ", ";
    stream << "error))" << endl;

    stream << "    {" << endl;
    stream << "        reply = QDBusMessage::methodReply(message);" << endl;

    it = method.arguments.begin();
    for (; it != endIt; ++it)
    {
        if ((*it).direction == Argument::Out)
        {
            if (!(*it).annotatedType.isEmpty())
            {
                stream << "        QDBusData " << (*it).name << "Data;" << endl;
                stream << "        QDBusDataConverter::convertToQDBusData<"
                       << (*it).annotatedType << ">(_"
                       << (*it).name << ", " << (*it).name << "Data);"
                       << endl;
                stream << "        reply << " << (*it).name << "Data";
            }
            else if (!(*it).accessor.isEmpty())
            {
                stream << "        reply << QDBusData::from" << (*it).accessor;
                if (!(*it).subAccessor.isEmpty())
                {
                    stream << "(" << (*it).containerClass;
                }

                stream << "(_" << (*it).name << ")";

                if (!(*it).subAccessor.isEmpty())
                {
                    stream << ")";
                }
            }
            else
                stream << "        reply << _" << (*it).name;

            stream << ";" << endl;
        }
    }
    stream << "    }" << endl;
    stream << "    else" << endl;
    stream << "    {" << endl;

    stream << "        if (!error.isValid())" << endl;
    stream << "        {" << endl;
    stream << "            qWarning(\"Call to implementation of ";

    QStringList::const_iterator nsIt    = classData.namespaces.begin();
    QStringList::const_iterator nsEndIt = classData.namespaces.end();
    for (; nsIt != nsEndIt; ++nsIt)
    {
        stream << *nsIt << "::";
    }

    stream  << classData.name << "::" << method.name;
    stream << " returned 'false' but error object is not valid!\");" << endl;
    stream << endl;
    stream << "            error = QDBusError::stdFailed(\"";

    nsIt    = classData.namespaces.begin();
    for (; nsIt != nsEndIt; ++nsIt)
    {
        stream << *nsIt << ".";
    }

    stream << classData.name << "." << method.name << " execution failed\");"
           << endl;
    stream << "        }" << endl;
    stream << endl;

    stream << "        reply = QDBusMessage::methodError(message, error);" << endl;

    stream << "    }" << endl;
    stream << endl;
    stream << "    return reply;" << endl;
    stream << "}" << endl;
    stream << endl;
}

void MethodGenerator::writeSignalEmitter(const Class& classData,
        const Method& method, QTextStream& stream)
{
    stream << "bool " << classData.name << "::emit" << method.name << "(";

    QValueList<Argument>::const_iterator it    = method.arguments.begin();
    QValueList<Argument>::const_iterator endIt = method.arguments.end();
    for (; it != endIt;)
    {
        if (!(*it).isPrimitive && (*it).direction == Argument::In)
            stream << "const ";

        stream << (*it).signature;

        if (!(*it).isPrimitive || (*it).direction == Argument::Out) stream << "&";

        stream << " " << (*it).name;

        ++it;
        if (it != endIt) stream << ", ";
    }

    stream << ")" << endl;

    stream << "{" << endl;

    // TODO: create error or use enum for return
    stream << "    QString path = objectPath();" << endl;
    stream << "    Q_ASSERT(!path.isEmpty());" << endl;
    stream << endl;

    stream << "    QDBusMessage message = QDBusMessage::signal(path, \"";
    stream << classData.dbusName << "\", \"" << method.name << "\");" << endl;
    stream << endl;

    it = method.arguments.begin();
    for (; it != endIt; ++it)
    {
        if ((*it).direction == Argument::In)
        {
            if (!(*it).annotatedType.isEmpty())
            {
                // TODO: error handling
                stream << "    QDBusData " << (*it).name << "Data;" << endl;
                stream << "    if (QDBusDataConverter:convertToQDBusData<"
                       << (*it).annotatedType << ">("
                       << (*it).name << ", " << (*it).name << "Data"
                       << ") != QDBusDataConverter::Success) return false;"
                       << endl;
                stream << "    message << " << (*it).name << "Data";
            }
            else if (!(*it).accessor.isEmpty())
            {
                stream << "    message << QDBusData::from" << (*it).accessor;
                if (!(*it).subAccessor.isEmpty())
                {
                    stream << "(" << (*it).containerClass;
                }

                stream << "(" << (*it).name << ")";

                if (!(*it).subAccessor.isEmpty())
                {
                    stream << ")";
                }
            }
            else
                stream << "    message << " << (*it).name;

            stream << ";" << endl;
        }
    }
    stream << endl;

    stream << "    return handleSignalSend(message);" << endl;

    stream << "}" << endl;
    stream << endl;
}


void MethodGenerator::writeInterfaceAsyncReplyHandler(const Class& classData,
    const Method& method, QTextStream& stream)
{
    stream << "void " << classData.name << "::" << method.name
           << "AsyncReply(";

    QValueList<Argument>::const_iterator it    = method.arguments.begin();
    QValueList<Argument>::const_iterator endIt = method.arguments.end();
    while (it != endIt)
    {
        if (!(*it).isPrimitive && (*it).direction == Argument::In)
            stream << "const ";

        stream << (*it).signature;

        if (!(*it).isPrimitive || (*it).direction == Argument::Out) stream << "&";

        stream << " " << (*it).name;

        ++it;
        if (it != endIt) stream << ", ";
    }
    stream << ")" << endl;
    stream << endl;
    stream << "{" << endl;

    stream << "    QMap<int, QDBusMessage>::iterator findIt = m_asyncCalls.find(asyncCallId);" << endl;
    stream << "    if (findIt == m_asyncCalls.end()) return;" << endl;
    stream << endl;

    stream << "    QDBusMessage call = findIt.data();" << endl;
    stream << "    m_asyncCalls.erase(findIt);" << endl;
    stream << endl;

    stream << "    QDBusMessage reply = QDBusMessage::methodReply(call);"
           << endl;

    it = method.arguments.begin();
    for (++it; it != endIt; ++it) // skip asyncCallId at beginning
    {
        if (!(*it).annotatedType.isEmpty())
        {
            stream << "    QDBusData " << (*it).name << "Data;" << endl;

            // TODO error handling
            stream << "    if (QDBusDataConverter::convertToQDBusData<"
                   << (*it).annotatedType << ">(" << (*it).name << ", "
                   << (*it).name << "Data"
                   << ") != QDBusDataConverter::Success) return false;"
                   << endl;
            stream << "    reply << " << (*it).name << "Data;" << endl;
        }
        else if (!(*it).accessor.isEmpty())
        {
            stream << "    reply << QDBusData::from" << (*it).accessor << "(";

            if ((*it).subAccessor.isEmpty())
                stream << (*it).name;
            else
                stream << (*it).containerClass << "(" << (*it).name << ")";

            stream << ");" << endl;
        }
        else
            stream << "    reply << " << (*it).name << ";" << endl;
    }
    stream << endl;

    stream << "    handleMethodReply(reply);" << endl;

    stream << "}" << endl;
    stream << endl;

    stream << "void " << classData.name << "::" << method.name
           << "AsyncError(int asyncCallId, const QDBusError& error)";
    stream << endl;

    stream << "{" << endl;

    stream << "    QMap<int, QDBusMessage>::iterator findIt = m_asyncCalls.find(asyncCallId);" << endl;
    stream << "    if (findIt == m_asyncCalls.end()) return;" << endl;
    stream << endl;

    stream << "    QDBusMessage call = findIt.data();" << endl;
    stream << "    m_asyncCalls.erase(findIt);" << endl;
    stream << endl;

    stream << "    QDBusMessage reply = QDBusMessage::methodError(call, error);"
           << endl;
    stream << "    handleMethodReply(reply);" << endl;

    stream << "}" << endl;
    stream << endl;
}

void MethodGenerator::writeInterfaceMainMethod(const Class& classData,
        QTextStream& stream)
{
    if (classData.methods.isEmpty()) return;

    stream << "bool " << classData.name
           << "::handleMethodCall(const QDBusMessage& message)" << endl;
    stream << "{" << endl;

    stream << "    if (message.interface() != \"" << classData.dbusName
           << "\") return false;" << endl;
    stream << endl;

    QValueList<Method>::const_iterator it    = classData.methods.begin();
    QValueList<Method>::const_iterator endIt = classData.methods.end();
    for (; it != endIt; ++it)
    {
        stream << "    if (message.member() == \"" << (*it).name << "\")" << endl;
        stream << "    {" << endl;

        if ((*it).async)
        {
            stream << "        call" << (*it).name << "Async(message);" << endl;
            stream << endl;
        }
        else
        {
            stream << "        QDBusMessage reply = call" << (*it).name
                   << "(message);" << endl;
            stream << "        handleMethodReply(reply);" << endl;
            stream << endl;
        }
        stream << "        return true;" << endl;
        stream << "    }" << endl;
        stream << endl;
    }

    stream << "    return false; " << endl;
    stream << "}" << endl;
    stream << endl;
}

void MethodGenerator::writeSignalHandler(const Class& classData,
        QTextStream& stream)
{
    stream << "void " << classData.name
           << "::slotHandleDBusSignal(const QDBusMessage& message)" << endl;
    stream << "{" << endl;

    QValueList<Method>::const_iterator it    = classData.signals.begin();
    QValueList<Method>::const_iterator endIt = classData.signals.end();
    bool first = true;
    for (; it != endIt; ++it)
    {
        stream << "    ";

        if (!first)
            stream << "else ";
        else
            first = false;

        stream << "if (message.member() == \"" << (*it).name << "\")" << endl;
        stream << "    {" << endl;

        writeVariables("        ", *it, stream);
        stream << endl;

        writeSignalEmit(*it, stream);

        stream << "    }" << endl;
    }

    stream << "}" << endl;
    stream << endl;
}

void MethodGenerator::writeProxyBegin(const Class& classData, QTextStream& stream)
{
    stream << classData.name << "::" << classData.name
           << "(const QString& service, const QString& path, QObject* parent, const char* name)" << endl;
    stream << "    : QObject(parent, name)," << endl;
    stream << "      m_baseProxy(new QDBusProxy())" << endl;
    stream << "{" << endl;
    stream << "    m_baseProxy->setInterface(\""
           << classData.dbusName << "\");" << endl;
    stream << "    m_baseProxy->setPath(path);" << endl;
    stream << "    m_baseProxy->setService(service);" << endl;
    stream << endl;

    if (!classData.signals.isEmpty())
    {
        stream << "    QObject::connect(m_baseProxy, "
               << "SIGNAL(dbusSignal(const QDBusMessage&))," << endl;
        stream << "                     this, "
               << "       SLOT(slotHandleDBusSignal(const QDBusMessage&)));"
               << endl;
    }

    if (!classData.asyncReplySignals.isEmpty())
    {
        stream << "    QObject::connect(m_baseProxy, "
               << "SIGNAL(asyncReply(int, const QDBusMessage&))," << endl;
        stream << "                     this, "
               << "       SLOT(slotHandleAsyncReply(int, const QDBusMessage&)));"
               << endl;
    }

    stream << "}" << endl;

    stream << endl;

    stream << classData.name << "::~" << classData.name << "()" << endl;
    stream << "{" << endl;
    stream << "    delete m_baseProxy;" << endl;
    stream << "}" << endl;
    stream << endl;

    stream << "void " << classData.name
           << "::setConnection(const QDBusConnection& connection)" << endl;
    stream << "{" << endl;
    stream << "    m_baseProxy->setConnection(connection);" << endl;
    stream << "}" << endl;
    stream << endl;
}

void MethodGenerator::writeProxyMethod(const QString& className,
        const Method& method, QTextStream& stream)
{
    stream << "bool " << className << "::" << method.name
           << (method.async ? "Async(" : "(");

    QValueList<Argument>::const_iterator it    = method.arguments.begin();
    QValueList<Argument>::const_iterator endIt = method.arguments.end();
    for (; it != endIt; ++it)
    {
        if (!(*it).isPrimitive && (*it).direction == Argument::In)
            stream << "const ";

        stream << (*it).signature;

        if (!(*it).isPrimitive || (*it).direction == Argument::Out) stream << "&";

        stream << " " << (*it).name << ", ";
    }

    stream << "QDBusError& error)" << endl;

    stream << "{" << endl;
    stream << "    QValueList<QDBusData> parameters;" << endl;
    stream << endl;

    uint outCount = 0;

    it = method.arguments.begin();
    for (; it != endIt; ++it)
    {
        if ((*it).direction == Argument::Out)
        {
            ++outCount;
            continue;
        }

        if (!(*it).annotatedType.isEmpty())
        {
            stream << "    QDBusData " << (*it).name << "Data;" << endl;

            // TODO error handling
            stream << "    if (QDBusDataConverter::convertToQDBusData<"
                   << (*it).annotatedType << ">(" << (*it).name << ", "
                   << (*it).name << "Data"
                   << ") != QDBusDataConverter::Success) return false;"
                   << endl;
            stream << "    parameters << " << (*it).name << "Data;" << endl;
        }
        else if (!(*it).accessor.isEmpty())
        {
            stream << "    parameters << QDBusData::from" << (*it).accessor << "(";

            if ((*it).subAccessor.isEmpty())
                stream << (*it).name;
            else
                stream << (*it).containerClass << "(" << (*it).name << ")";

            stream << ");" << endl;
        }
        else
            stream << "    parameters << " << (*it).name << ";" << endl;
    }

    stream << endl;

    if (outCount == 0 && method.noReply)
    {
        stream << "    if (!m_baseProxy->send(\"" << method.name
               << "\", parameters))" << endl;
        stream << "    {" << endl;
        stream << "        error = m_baseProxy->lastError();" << endl;
        stream << "        return false;" << endl;
        stream << "    }" << endl;
        stream << "    return true;" << endl;
        stream << "}" << endl;
        stream << endl;
        return;
    }

    if (method.async)
    {
        stream << "    asyncCallId = m_baseProxy->sendWithAsyncReply(\"";
        stream << method.name << "\", parameters);" << endl;
        stream << endl;

        stream << "    if (asyncCallId != 0) m_asyncCalls[asyncCallId] = \""
               << method.name << "\";" << endl;
        stream << endl;

        stream << "    return (asyncCallId != 0);" << endl;
        stream << "}" << endl;
        stream << endl;
        return;
    }

    stream << "    QDBusMessage reply = m_baseProxy->sendWithReply(\"";
    stream << method.name << "\", parameters, &error);" << endl;
    stream << endl;

    stream << "    if (reply.type() != QDBusMessage::ReplyMessage) return false;"
           << endl;

    if (outCount == 0)
    {
        stream << "    return true;" << endl;
        stream << "}" << endl;
        stream << endl;
        return;
    }

    stream << endl;

    // TODO: create error or use enum for return
    stream << "    if (reply.count() != " << outCount << ") return false;" << endl;
    stream << endl;

    bool firstAccessor    = true;
    bool firstSubAccessor = true;

    it = method.arguments.begin();
    for (; it != endIt; ++it)
    {
        if ((*it).direction == Argument::In) continue;

        --outCount;

        if (!(*it).annotatedType.isEmpty())
        {
            // TODO error handling
            stream << "    if (QDBusDataConverter::convertFromQDBusData<"
                   << (*it).annotatedType << ">(reply.front(), "
                   << (*it).name
                   << ") != QDBusDataConverter::Success) return false;"
                   << endl;
        }
        else if (!(*it).accessor.isEmpty())
        {
            if (firstAccessor)
            {
                stream << "    bool ok = false;" << endl;
                stream << endl;
                firstAccessor = false;
            }

            if ((*it).subAccessor.isEmpty())
            {
                stream << "    " << (*it).name << " = reply.front().to"
                    << (*it).accessor << "(&ok);" << endl;
            }
            else
            {
                if (firstSubAccessor)
                {
                    stream << "    bool subOK = false;" << endl;
                    stream << endl;
                    firstSubAccessor = false;
                }

                stream << "    " << (*it).name << " = reply.front().to"
                    << (*it).accessor << "(&ok).to" << (*it).subAccessor
                    << "(&subOK);" << endl;

                // TODO: create error or use enum for return
                stream << "    if (!subOK) return false;" << endl;
            }

            // TODO: create error or use enum for return
            stream << "    if (!ok) return false;" << endl;
        }
        else
            stream << "    " << (*it).name << " = reply.front();" << endl;
        stream << endl;

        if (outCount > 0)
        {
            stream << "    reply.pop_front();" << endl;
            stream << endl;
        }
    }

    stream << "    return true;" << endl;

    stream << "}" << endl;
    stream << endl;
}

void MethodGenerator::writeProxyGenericProperty(const Class& classData,
        QTextStream& stream)
{
    stream << "void " << classData.name
           << "::setDBusProperty(const QString& name, "
           << "const QDBusVariant& value, QDBusError& error)"
           << endl;
    stream << "{" << endl;

    stream << "    QDBusConnection connection = m_baseProxy->connection();" << endl;
    stream << endl;
    stream << "    QDBusMessage message = QDBusMessage::methodCall("
           << "m_baseProxy->service(), m_baseProxy->path(), "
           << "\"org.freedesktop.DBus.Properties\", \"Set\");" << endl;
    stream << endl;

    stream << "    message << QDBusData::fromString(m_baseProxy->interface());"
           << endl;
    stream << "    message << QDBusData::fromString(name);" << endl;
    stream << "    message << QDBusData::fromVariant(value);" << endl;
    stream << endl;

    stream << "    connection.sendWithReply(message, &error);" << endl;

    stream << "}" << endl;

    stream << endl;

    stream << "QDBusVariant " << classData.name
           << "::getDBusProperty(const QString& name, QDBusError& error) const"
           << endl;
    stream << "{" << endl;

    stream << "    QDBusConnection connection = m_baseProxy->connection();" << endl;
    stream << endl;

    stream << "    QDBusMessage message = QDBusMessage::methodCall("
           << "m_baseProxy->service(), m_baseProxy->path(), "
           << "\"org.freedesktop.DBus.Properties\", \"Get\");" << endl;
    stream << endl;

    stream << "    message << QDBusData::fromString(m_baseProxy->interface());"
           << endl;
    stream << "    message << QDBusData::fromString(name);" << endl;
    stream << endl;

    stream << "    QDBusMessage reply = connection.sendWithReply(message, &error);"
           << endl;
    stream << endl;

    stream << "    if (reply.type() != QDBusMessage::ReplyMessage)"
           << " return QDBusVariant();" << endl;
    stream << "    if (reply.count() != 1) return QDBusVariant();" << endl;

    stream << endl;

    stream << "    bool ok = false;" << endl;
    stream << "    QDBusVariant value = reply.front().toVariant(&ok);" << endl;

    // TODO generate error
    stream << "    if (!ok) return QDBusVariant();" << endl;
    stream << endl;

    stream << "    return value;" << endl;

    stream << "}" << endl;

    stream << endl;
}

void MethodGenerator::writeProxyProperty(const Class& classData,
        const Property& property, QTextStream& stream)
{
    if (property.write)
    {
        stream << "void " << classData.name << "::set" << property.name << "(";

        if (!property.isPrimitive) stream << "const ";

        stream << property.signature;

        if (!property.isPrimitive) stream << "&";

        stream << " value, QDBusError& error)" << endl;
        stream << "{" << endl;
        stream << "    QDBusVariant variant;" << endl;

        if (!property.annotatedType.isEmpty())
        {
            // TODO: error handling
            stream << "    QDBusDataConverter::convertToQDBusData<"
                   << property.annotatedType << ">(value, variant.value);"
                   << endl;
        }
        else if (!property.accessor.isEmpty())
        {
            stream << "    variant.value = QDBusData::from"
                   << property.accessor << "(";

            if (property.subAccessor.isEmpty())
                stream << "value";
            else
                stream << property.containerClass << "(value)";

            stream << ");" << endl;
        }
        else
            stream << "    variant.value = QDBusData(value);" << endl;

        stream << "    variant.signature = \"" << property.dbusSignature << "\";"
               << endl;

        stream << endl;
        stream << "    setDBusProperty(\"" << property.name
               << "\", variant, error);" << endl;

        stream << "}" << endl;
        stream << endl;
    }

    if (property.read)
    {
        stream << property.signature << " " << classData.name
               << "::get" << property.name << "(QDBusError& error) const" << endl;
        stream << "{" << endl;

        stream << "    QDBusVariant variant = getDBusProperty(\""
               << property.name << "\", error);" << endl;
        stream << endl;
        stream << "    if (error.isValid()) return "
               << property.signature << "();" << endl;
        stream << endl;

        if (!property.annotatedType.isEmpty())
        {
            stream << "    " << property.signature << " result;" << endl;

            // TODO error handling
            stream << "    QDBusDataConverter::convertFromQDBusData<"
                   << property.annotatedType << ">(variant.value, result);"
                   << endl;
        }
        else if (!property.accessor.isEmpty())
        {
            stream << "    bool ok = false;" << endl;
            stream << endl;

            if (property.subAccessor.isEmpty())
            {
                stream << "    " << property.signature << " result = ";
                stream << " variant.value.to" << property.accessor
                       << "(&ok);" << endl;
            }
            else
            {
                stream << "    bool subOK = false;" << endl;
                stream << endl;

                stream << "    " << property.signature << " result = ";
                stream << " variant.value.to"
                       << property.accessor << "(&ok).to" << property.subAccessor
                       << "(&subOK);" << endl;

                // TODO: create error
                stream << "    if (!subOK) {}" << endl;
            }

            // TODO: create error
            stream << "    if (!ok) {}" << endl;
        }
        else
            stream << "    " << property.signature << " result = variant.value;";
        stream << endl;

        stream << "    return result;" << endl;
        stream << "}" << endl;
        stream << endl;
    }
}

void MethodGenerator::writeProxyAsyncReplyHandler(const Class& classData,
        QTextStream& stream)
{
    stream << "void " << classData.name
           << "::slotHandleAsyncReply(int asyncCallId, const QDBusMessage& message)" << endl;
    stream << "{" << endl;

    stream << "    QMap<int, QString>::iterator findIt = "
           << "m_asyncCalls.find(asyncCallId);" << endl;
    stream << "    if (findIt == m_asyncCalls.end()) return;" << endl;
    stream << endl;
    stream << "    const QString signalName = findIt.data();" << endl;
    stream << "    m_asyncCalls.erase(findIt);" << endl;
    stream << endl;

    QValueList<Method>::const_iterator it    = classData.asyncReplySignals.begin();
    QValueList<Method>::const_iterator endIt = classData.asyncReplySignals.end();
    bool first = true;
    for (; it != endIt; ++it)
    {
        stream << "    ";

        if (!first)
            stream << "else ";
        else
            first = false;

        stream << "if (signalName == \"" << (*it).name << "\")" << endl;
        stream << "    {" << endl;

        // FIXME tricking writeVariables and writeSignalEmit into writing
        // the reply emit code by manipulating arguments and name
        stream << "        int _asyncCallId = asyncCallId;" << endl;

        Method signal = *it;
        signal.arguments.pop_front();

        writeVariables("        ", signal, stream);
        stream << endl;

        signal = *it;
        signal.name += "AsyncReply";

        writeSignalEmit(signal, stream);

        stream << "    }" << endl;
    }

    stream << "}" << endl;
    stream << endl;
}

void MethodGenerator::writeIntrospectionDataMethod(const Class& classData,
    QTextStream& stream)
{
    stream << "void " << classData.name
           << "::buildIntrospectionData(QDomElement& interfaceElement)" << endl;
    stream << "{" << endl;

    stream << "    interfaceElement.setAttribute(\"name\", \""
           << classData.dbusName << "\");" << endl;
    stream << endl;

    bool firstMethod   = true;
    bool firstArgument = true;

    QValueList<Method>::const_iterator it    = classData.methods.begin();
    QValueList<Method>::const_iterator endIt = classData.methods.end();
    for (; it != endIt; ++it)
    {
        if (firstMethod)
        {
            firstMethod = false;
            stream << "    QDomDocument document = interfaceElement.ownerDocument();" << endl;
            stream << "    QDomElement methodElement = document.createElement("
                   << "\"method\");" << endl;
        }
        else
        {
            stream << endl;
            stream << "    methodElement = document.createElement("
                   << "\"method\");" << endl;
        }

        writeMethodIntrospection(*it, firstArgument, stream);

        stream << "    interfaceElement.appendChild(methodElement);" << endl;
    }

    it    = classData.signals.begin();
    endIt = classData.signals.end();
    for (; it != endIt; ++it)
    {
        if (firstMethod)
        {
            firstMethod = false;
            stream << "    QDomDocument document = interfaceElement.ownerDocument();" << endl;
            stream << endl;
            stream << "    QDomElement methodElement = document.createElement("
                   << "\"signal\");" << endl;
        }
        else
        {
            stream << endl;
            stream << "    methodElement = document.createElement("
                   << "\"signal\");" << endl;
        }

        writeMethodIntrospection(*it, firstArgument, stream);

        stream << "    interfaceElement.appendChild(methodElement);" << endl;
    }

    stream << "}" << endl;
    stream << endl;
}

void MethodGenerator::writeNodePrivate(const Class& classData, QTextStream& stream)
{
    stream << "class " << classData.name
           << "::Private : public org::freedesktop::DBus::Introspectable" << endl;
    stream << "{" << endl;
    stream << "public:" << endl;
    stream << "    virtual ~Private();" << endl;
    stream << endl;

    stream << "public:" << endl;
    stream << "    QMap<QString, QDBusObjectBase*> interfaces;" << endl;
    stream << "    QString introspectionData;" << endl;
    stream << endl;
    stream << "    QDBusConnection connection;" << endl;
    stream << "    QString objectPath;" << endl;
    stream << endl;

    stream << "protected:" << endl;
    stream << "    virtual bool Introspect(QString& data, QDBusError& error);"
           << endl;
    stream << endl;
    stream << "    virtual void handleMethodReply(const QDBusMessage& reply);"
           << endl;

    stream << "private:" << endl;
    stream << "    void cacheIntrospectionData();" << endl;

    stream << "};" << endl;
    stream << endl;
}

void MethodGenerator::writeNodeBegin(const Class& classData, QTextStream& stream)
{
    stream << classData.name << "::" << classData.name
           << "()  : QDBusObjectBase()," << endl;
    stream << "    m_private(new Private())" << endl;
    stream << "{" << endl;
    stream << "}" << endl;
    stream << endl;

    stream << classData.name << "::~" << classData.name << "()" << endl;
    stream << "{" << endl;
    stream << "    unregisterObject();" << endl;
    stream << endl;
    stream << "    delete m_private;" << endl;
    stream << "}" << endl;
    stream << endl;
}

void MethodGenerator::writeNodeMethods(const Class& classData,
            const QValueList<Class>& interfaces, QTextStream& stream)
{
    writeNodeInitialization(classData, interfaces, stream);

    stream << "void " << classData.name << "::unregisterObject()" << endl;
    stream << "{" << endl;
    stream << "    if (m_private->objectPath.isEmpty()) return;" << endl;
    stream << endl;
    stream << "    m_private->connection.unregisterObject(m_private->objectPath);" << endl;
    stream << endl;
    stream << "    m_private->connection = QDBusConnection();" << endl;
    stream << "    m_private->objectPath = QString::null;" << endl;
    stream << "}" << endl;
    stream << endl;

    stream << "bool " << classData.name
           << "::handleMethodCall(const QDBusMessage& message)" << endl;
    stream << "{" << endl;
    stream << "    QMap<QString, QDBusObjectBase*>::iterator findIt = "
           << "m_private->interfaces.find(message.interface());" << endl;
    stream << "    if (findIt == m_private->interfaces.end()) return false;"
           << endl;
    stream << endl;
    stream << "    return delegateMethodCall(message, findIt.data());" << endl;
    stream << "}" << endl;
    stream << endl;

    stream << classData.name << "::Private::~Private()" << endl;
    stream << "{" << endl;
    stream << "    QMap<QString, QDBusObjectBase*>::const_iterator it    = "
           << "interfaces.begin();" << endl;
    stream << "    QMap<QString, QDBusObjectBase*>::const_iterator endIt = "
           << "interfaces.end();" << endl;
    stream << "    for (; it != endIt; ++it)" << endl;
    stream << "    {" << endl;
    stream << "        QDBusObjectBase* interface = it.data();" << endl;
    stream << "        if (interface != this)" << endl;
    stream << "            delete interface;" << endl;
    stream << "    }" << endl;
    stream << "    interfaces.clear();" << endl;
    stream << "}" << endl;
    stream << endl;

    stream << "bool " << classData.name << "::Private"
           << "::Introspect(QString& data, QDBusError& error)" << endl;
    stream << "{" << endl;
    stream << "    Q_UNUSED(error);" << endl;
    stream << "    if (introspectionData.isEmpty()) cacheIntrospectionData();"
           << endl;
    stream << endl;
    stream << "    data = introspectionData;" << endl;
    stream << endl;
    stream << "    return true;" << endl;
    stream << "}" << endl;
    stream << endl;

    stream << "void " << classData.name << "::Private"
           << "::handleMethodReply(const QDBusMessage& reply)" << endl;
    stream << "{" << endl;
    stream << "    connection.send(reply);" << endl;
    stream << "}" << endl;
    stream << endl;

    writeNodeIntrospection(classData, interfaces, stream);
}

// End of File
