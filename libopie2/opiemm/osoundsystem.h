/*
                             This file is part of the Opie Project

                             (C) 2003 Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef OSOUNDSYSTEM_H
#define OSOUNDSYSTEM_H

#include <qobject.h>
#include <qdict.h>
#include <qmap.h>

class OAudioInterface;
class OMixerInterface;
class OSoundCard;

/*======================================================================================
 * OSoundSystem
 *======================================================================================*/

/**
 * @brief A container class for all audio interfaces
 *
 * This class provides access to all available audio/midi/sequencer interfaces of your computer.
 *
 * @author Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
 */
class OSoundSystem : public QObject
{
  Q_OBJECT

  public:
    typedef QDict<OSoundCard> CardMap;
    typedef QDictIterator<OSoundCard> CardIterator;

  public:
    /**
     * @returns the number of available interfaces
     */
    int count() const;
    /**
     * @returns a pointer to the (one and only) @ref ONetwork instance.
     */
    static OSoundSystem* instance();
    /**
     * @returns an iterator usable for iterating through all network interfaces.
     */
    CardIterator iterator() const;
    /**
     * @returns a pointer to the @ref OAudioInterface object for the specified @a interface or 0, if not found
     * @see OAudioInterface
     */
    OSoundCard* card( const QString& interface ) const;
    /**
     * @internal Rebuild the internal interface database
     * @note Sometimes it might be useful to call this from client code,
     * e.g. after issuing a cardctl insert
     */
    void synchronize();

  protected:
    OSoundSystem();

  private:
    static OSoundSystem* _instance;
    CardMap _interfaces;
};


/*======================================================================================
 * OSoundCard
 *======================================================================================*/

class OSoundCard : public QObject
{
  Q_OBJECT

  public:
    /**
     * Constructor. Normally you don't create @ref OSoundCard objects yourself,
     * but access them via @ref OSoundSystem::card().
     */
    OSoundCard( QObject* parent, const char* name );
    /**
     * Destructor.
     */
    virtual ~OSoundCard();

    bool hasMixer() const { return _audio; };
    bool hasAudio() const { return _mixer; };

    OAudioInterface* audio() const { return _audio; };
    OMixerInterface* mixer() const { return _mixer; };

  protected:
    OAudioInterface* _audio;
    OMixerInterface* _mixer;

  private:
    void init();
};

/*======================================================================================
 * OAudioInterface
 *======================================================================================*/

class OAudioInterface : public QObject
{
  Q_OBJECT

  public:
    /**
     * Constructor. Normally you don't create @ref OAudioInterface objects yourself,
     * but access them via the @ref OSoundCard interface.
     */
    OAudioInterface( QObject* parent, const char* name );
    /**
     * Destructor.
     */
    virtual ~OAudioInterface();

  protected:
    const int _sfd;

  private:
    void init();
};


/*======================================================================================
 * OMixerInterface
 *======================================================================================*/

class OMixerInterface : public QObject
{
  Q_OBJECT

  public:

    typedef QMap<QString,int>::ConstIterator ChannelIterator;

    /**
     * Constructor. Normally you don't create @ref OMixerInterface objects yourself,
     * but access them via the @ref OSoundCard interface.
     */
    OMixerInterface( QObject* parent, const char* name );
    /**
     * Destructor.
     */
    virtual ~OMixerInterface();

    /**
     * @returns all available channels.
     */
    QStringList allChannels() const;
    /**
     * @returns recordable channels.
     */
    QStringList recChannels() const;
    /**
     * @returns playable channels.
     */
    QStringList playChannels() const;

    /**
     * @returns true, if @a channel exists.
     */
    bool hasChannel( const QString& channel );

    /**
     * Set the @a left and @a right volumes for @a channel.
     * If no value for right is given, the value for left is taken for that.
     */
    void setVolume( const QString& channel, int left, int right = -1 );
    /**
     * @returns the volume of @a channel or -1, if the channel doesn't exist.
     * @note You might want to use @ref hasChannel() to check if a channel exists.
     */
    int volume( const QString& channel ) const;

  protected:
    int _fd;
    QMap<QString, int> _channels;

  private:
    void init();
};

#endif // OSOUNDSYSTEM_H

