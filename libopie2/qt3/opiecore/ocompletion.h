/*
                             This file is part of the Opie Project
                             Originally part of the KDE Project
                             Copyright (C) 1999,2000 Carsten Pfeiffer <pfeiffer@kde.org>
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

#ifndef OCOMPLETION_H
#define OCOMPLETION_H

/* QT */

#include <qmap.h>
#include <qlist.h>
#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qguardedptr.h>

/* OPIE */

#include <opie2/oglobalsettings.h>
#include <opie2/osortablevaluelist.h>

/* FORWARDS */

class OCompTreeNode;
class OCompletionPrivate;
class OCompletionBasePrivate;
class OCompletionMatchesWrapper;
class OCompletionMatches;
class QPopupMenu;

// FIXME: Do we need special ShortCut handling in Opie? If so, revise this.
class OShortcut
{
public:
  bool isNull() const { return true; };
  bool operator == ( const OShortcut& bla ) const { return false; };
};


/**
 * This class offers easy use of "auto-completion", "manual-completion" or
 * "shell completion" on QString objects. A common use is completing filenames
 * or URLs (see @ref OURLCompletion()).
 * But it is not limited to URL-completion -- everything should be completable!
 * The user should be able to complete email-addresses, telephone-numbers,
 * commands, SQL queries, ...
 * Every time your program knows what the user can type into an edit-field, you
 * should offer completion. With OCompletion, this is very easy, and if you are
 * using a line edit widget (@ref OLineEdit), it is even more easy.
 * Basically, you tell a OCompletion object what strings should be completable
 * and whenever completion should be invoked, you call @ref makeCompletion().
 * OLineEdit and (an editable) OComboBox even do this automatically for you.
 *
 * OCompletion offers the completed string via the signal @ref match() and
 * all matching strings (when the result is ambiguous) via the method
 * @ref allMatches().
 *
 * Notice: auto-completion, shell completion and manual completion work
 *         slightly differently:
 *
 * @li auto-completion always returns a complete item as match.
 *     When more than one matching items are available, it will deliver just
 *     the first (depending on sorting order) item. Iterating over all matches
 *     is possible via @ref nextMatch() and @ref previousMatch().
 *
 * @li popup-completion works in the same way, the only difference being that
 *     the completed items are not put into the edit-widget, but into a
 *     separate popup-box.
 *
 * @li manual completion works the same way as auto-completion, the
 *     subtle difference is, that it isn't invoked automatically while the user
 *     is typing, but only when the user presses a special key. The difference
 *     of manual and auto-completion is therefore only visible in UI classes,
 *     OCompletion needs to know whether to deliver partial matches
 *     (shell completion) or whole matches (auto/manual completion), therefore
 *     @ref OGlobalSettings::CompletionMan and
 *     @ref OGlobalSettings::CompletionAuto have the exact same effect in
 *     OCompletion.
 *
 * @li shell completion works like how shells complete filenames:
 *     when multiple matches are available, the longest possible string of all
 *     matches is returned (i.e. only a partial item).
 *     Iterating over all matching items (complete, not partial) is possible
 *     via @ref nextMatch() and @ref previousMatch().
 *
 * You don't have to worry much about that though, OCompletion handles
 * that for you, according to the setting @ref setCompletionMode().
 * The default setting is globally configured by the user and read
 * from @ref OGlobalSettings::completionMode().
 *
 * A short example:
 * <pre>
 * OCompletion completion;
 * completion.setOrder( OCompletion::Sorted );
 * completion.addItem( "pfeiffer@kde.org" );
 * completion.addItem( "coolo@kde.org" );
 * completion.addItem( "carpdjih@sp.zrz.tu-berlin.de" );
 * completion.addItem( "carp@cs.tu-berlin.de" );
 *
 * cout << completion.makeCompletion( "ca" ).latin1() << endl;
 * </pre>
 * In shell-completion-mode, this will be "carp"; in auto-completion-
 * mode it will be "carp@cs.tu-berlin.de", as that is alphabetically
 * smaller.
 * If setOrder was set to Insertion, "carpdjih@sp.zrz.tu-berlin.de"
 * would be completed in auto-completion-mode, as that was inserted before
 * "carp@cs.tu-berlin.de".
 *
 * You can dynamically update the completable items by removing and adding them
 * whenever you want.
 * For advanced usage, you could even use multiple OCompletion objects. E.g.
 * imagine an editor like kwrite with multiple open files. You could store
 * items of each file in a different OCompletion object, so that you know (and
 * tell the user) where a completion comes from.
 *
 * Note: OCompletion does not work with strings that contain 0x0 characters
 *       (unicode nul), as this is used internally as a delimiter.
 *
 * You may inherit from OCompletion and override @ref makeCompletion() in
 * special cases (like reading directories/urls and then supplying the
 * contents to OCompletion, as OURLCompletion does), but generally, this is
 * not necessary.
 *
 *
 * @short A generic class for completing QStrings
 * @author Carsten Pfeiffer <pfeiffer@kde.org>
 */

class OCompletion : public QObject
{
    Q_ENUMS( CompOrder )
    Q_PROPERTY( CompOrder order READ order WRITE setOrder )
    Q_PROPERTY( bool ignoreCase READ ignoreCase WRITE setIgnoreCase )
    Q_PROPERTY( QStringList items READ items WRITE setItems )
    Q_OBJECT

  public:
    /**
     * Constants that represent the order in which OCompletion performs
     * completion-lookups.
     */
    enum CompOrder { Sorted, Insertion, Weighted };

    /**
     * Constructor, nothing special here :)
     */
    OCompletion();

    // FIXME: copy constructor, assignment constructor...

    /**
     * Destructor, nothing special here, either.
     */
    virtual ~OCompletion();

    /**
     * Attempts to find an item in the list of available completions,
     * that begins with @p string. Will either return the first matching item
     * (if there is more than one match) or QString::null, if no match was
     * found.
     *
     * In the latter case, a sound will be issued, depending on
     * @ref isSoundsEnabled().
     * If a match was found, it will also be emitted via the signal
     * @ref match().
     *
     * If this is called twice or more often with the same string while no
     * items were added or removed in the meantime, all available completions
     * will be emitted via the signal @ref matches().
     * This happens only in shell-completion-mode.
     *
     * @returns the matching item, or QString::null if there is no matching
     * item.
     * @see #slotMakeCompletion
     * @see #substringCompletion
     */
    virtual QString makeCompletion( const QString& string );

    /**
     * @returns a list of items which all contain @p text as a substring,
     * i.e. not necessarily at the beginning.
     *
     * @see #makeCompletion
     */
    QStringList substringCompletion( const QString& string ) const;

    /**
     * @returns the next item from the matching-items-list.
     * When reaching the beginning, the list is rotated so it will return the
     * last match and a sound is issued (depending on @ref isSoundsEnabled()).
     * When there is no match, QString::null is returned and
     * a sound is be issued.
     * @see #slotPreviousMatch
     */
    QString previousMatch();

    /**
     * @returns the previous item from the matching-items-list
     * When reaching the last item, the list is rotated, so it will return
     * the first match and a sound is issued (depending on
     * @ref isSoundsEnabled()). When there is no match, QString::null is
     * returned and a sound is issued.
     * @see #slotNextMatch
     */
    QString nextMatch();

    /**
     * @returns the last match. Might be useful if you need to check whether
     * a completion is different from the last one.
     * QString::null is returned when there is no last match.
     */
    virtual const QString& lastMatch() const { return myLastMatch; }

    /**
     * Returns a list of all items inserted into OCompletion. This is useful
     * if you need to save the state of a OCompletion object and restore it
     * later.
     *
     * Important note: when @ref order() == Weighted, then every item in the
     * stringlist has its weight appended, delimited by a colon. E.g. an item
     * "www.kde.org" might look like "www.kde.org:4", where 4 is the weight.
     *
     * This is necessary so that you can save the items along with its
     * weighting on disk and load them back with @ref setItems(), restoring its
     * weight as well. If you really don't want the appended weightings, call
     * @ref setOrder( OCompletion::Insertion )
     * before calling items().
     *
     * @returns a list of all items
     * @see #setItems
     */
    QStringList items() const;

    /**
     * Sets the completion mode to Auto/Manual, Shell or None.
     * If you don't set the mode explicitly, the global default value
     * OGlobalSettings::completionMode() is used.
     * @ref OGlobalSettings::CompletionNone disables completion.
     * @see #completionMode
     * @see #OGlobalSettings::completionMode
     */
    virtual void setCompletionMode( OGlobalSettings::Completion mode );

    /**
     * @returns the current completion mode.
     * May be different from @ref OGlobalSettings::completionMode(), if you
     * explicitly called @ref setCompletionMode().
     * @see #setCompletionMode
     */
    OGlobalSettings::Completion completionMode() const { return myCompletionMode; };

    /**
     * OCompletion offers three different ways in which it offers its items:
     * @li in the order of insertion
     * @li sorted alphabetically
     * @li weighted
     *
     * Choosing weighted makes OCompletion perform an implicit weighting based
     * on how often an item is inserted. Imagine a web browser with a location
     * bar, where the user enters URLs. The more often a URL is entered, the
     * higher priority it gets.
     *
     * Note: Setting the order to sorted only affects new inserted items,
     * already existing items will stay in the current order. So you probably
     * want to call setOrder( Sorted ) before inserting items, when you want
     * everything sorted.
     *
     * Default is insertion order
     * @see #order
     */
    virtual void setOrder( CompOrder order );

    /**
     * @returns the current completion order.
     * @see #setOrder
     */
    CompOrder order() const { return myOrder; }

    /**
     * Setting this to true makes OCompletion behave case insensitively.
     * E.g. makeCompletion( "CA" ); might return "carp@cs.tu-berlin.de".
     * Default is false (case sensitive).
     * @see #ignoreCase
     */
    virtual void setIgnoreCase( bool ignoreCase );

    /**
     * @returns whether OCompletion acts case insensitively or not.
     * Default is false (case sensitive).
     * @see #setIgnoreCase
     */
    bool ignoreCase() const { return myIgnoreCase; };

    /**
     * @returns a list of all items matching the last completed string.
     * Might take some time, when you have LOTS of items.
     *
     * @see #substringCompletion
     */
    QStringList allMatches();

    /**
     * @returns a list of all items matching @p string.
     */
    QStringList allMatches( const QString& string );

    /**
     * @returns a list of all items matching the last completed string.
     * Might take some time, when you have LOTS of items.
     * The matches are returned as OCompletionMatches, which also
     * keeps the weight of the matches, allowing
     * you to modify some matches or merge them with matches
     * from another call to allWeightedMatches(), and sort the matches
     * after that in order to have the matches ordered correctly
     *
     * @see #substringCompletion
     */
    OCompletionMatches allWeightedMatches();

    /**
     * @returns a list of all items matching @p string.
     */
    OCompletionMatches allWeightedMatches( const QString& string );

    /**
     * Enables/disables playing a sound when
     * @li @ref makeCompletion() can't find a match
     * @li there is a partial completion (= multiple matches in
     *     Shell-completion mode)
     * @li @ref nextMatch() or @ref previousMatch() hit the last possible
     *     match -> rotation
     *
     * For playing the sounds, @ref ONotifyClient() is used. // FIXME: Revise this for Opie
     *
     * @see #isSoundsEnabled
     */
    virtual void setEnableSounds( bool enable ) { myBeep = enable; }

    /**
     * Tells you whether OCompletion will play sounds on certain occasions.
     * Default is enabled
     * @see #enableSounds
     * @see #disableSounds
     */
    bool isSoundsEnabled() const { return myBeep; };

    /**
     * @returns true when more than one match is found
     * @see #multipleMatches
     */
    bool hasMultipleMatches() const { return myHasMultipleMatches; };

  public slots:
    /**
     * Attempts to complete "string" and emits the completion via @ref match().
     * Same as @ref makeCompletion() (just as a slot).
     * @see #makeCompletion
     */
    void slotMakeCompletion( const QString& string ) { (void) makeCompletion( string ); };

    /**
     * Searches the previous matching item and emits it via @ref match()
     * Same as @ref previousMatch() (just as a slot).
     * @see #previousMatch
     */
    void slotPreviousMatch() { (void) previousMatch(); };

    /**
     * Searches the next matching item and emits it via @ref match()
     * Same as @ref nextMatch() (just as a slot).
     * @see #nextMatch
     */
    void slotNextMatch() { (void) nextMatch(); };

    /**
     * Inserts @p items into the list of possible completions.
     * Does the same as @ref setItems(), but does not call @ref clear() before.
     */
    void insertItems( const QStringList& items );

    /**
     * Sets the list of items available for completion. Removes all previous
     * items.
     *
     * Notice: when order() == Weighted, then the weighting is looked up for
     * every item in the stringlist. Every item should have ":number" appended,
     * where number is an unsigned integer, specifying the weighting.
     *
     * If you don't like this, call
     * setOrder( OCompletion::Insertion )
     * before calling setItems().
     *
     * @see #items
     */
    virtual void setItems( const QStringList& );

    /**
     * Adds an item to the list of available completions.
     * Resets the current item-state (@ref previousMatch() and @ref nextMatch()
     * won't work anymore).
     */
    void addItem( const QString& );

    /**
     * Adds an item to the list of available completions.
     * Resets the current item-state (@ref previousMatch() and @ref nextMatch()
     * won't work anymore).
     *
     * Sets the weighting of the item to @p weight or adds it to the current
     * weighting if the item is already available. The weight has to be greater
     * than 1 to take effect (default weight is 1).
     */
    void addItem( const QString&, uint weight );

    /**
     * Removes an item from the list of available completions.
     * Resets the current item-state (@ref previousMatch() and @ref nextMatch()
     * won't work anymore).
     */
    void removeItem( const QString& );

    /**
     * Removes all inserted items.
     */
    virtual void clear();

  signals:
    /**
     * The matching item. Will be emitted by @ref makeCompletion(),
     * @ref previousMatch() or @ref nextMatch(). May be QString::null if there
     * is no matching item.
     */
    void match( const QString& );

    /**
     * All matching items. Will be emitted by @ref makeCompletion() in shell-
     * completion-mode, when the same string is passed to makeCompletion twice
     * or more often.
     */
    void matches( const QStringList& );

    /**
     * This signal is emitted, when calling @ref makeCompletion() and more than
     * one matching item is found.
     * @see #hasMultipleMatches
     */
    void multipleMatches();

  protected:
    /**
     * This method is called after a completion is found and before the
     * matching string is emitted. You can override this method to modify the
     * string that will be emitted.
     * This is necessary e.g. in @ref OURLCompletion(), where files with spaces
     * in their names are shown escaped ("filename\ with\ spaces"), but stored
     * unescaped inside OCompletion.
     * Never delete that pointer!
     *
     * Default implementation does nothing.
     * @see #postProcessMatches
     */
    virtual void postProcessMatch( QString * /*match*/ ) const {}

    /**
     * This method is called before a list of all available completions is
     * emitted via @ref matches. You can override this method to modify the
     * found items before @ref match() or @ref matches() are emitted.
     * Never delete that pointer!
     *
     * Default implementation does nothing.
     * @see #postProcessMatch
     */
    virtual void postProcessMatches( QStringList * /*matches*/ ) const {}

    /**
     * This method is called before a list of all available completions is
     * emitted via @ref matches. You can override this method to modify the
     * found items before @ref match() or @ref matches() are emitted.
     * Never delete that pointer!
     *
     * Default implementation does nothing.
     * @see #postProcessMatch
     */
    virtual void postProcessMatches( OCompletionMatches * /*matches*/ ) const {}

private:
    void addWeightedItem( const QString& );
    QString findCompletion( const QString& string );
    void findAllCompletions( const QString&, OCompletionMatchesWrapper *matches, bool& hasMultipleMatches ) const;

    void extractStringsFromNode( const OCompTreeNode *,
                                 const QString& beginning,
                                 OCompletionMatchesWrapper *matches,
                                 bool addWeight = false ) const;
    void extractStringsFromNodeCI( const OCompTreeNode *,
                                   const QString& beginning,
                                   const QString& restString,
                                   OCompletionMatchesWrapper *matches) const;

    enum BeepMode { NoMatch, PartialMatch, Rotation };
    void doBeep( BeepMode ) const;

    OGlobalSettings::Completion myCompletionMode;

    CompOrder myOrder;
    QString myLastString;
    QString myLastMatch;
    QString myCurrentMatch;
    OCompTreeNode * myTreeRoot;
    QStringList myRotations;
    bool myBeep;
    bool myIgnoreCase;
    bool myHasMultipleMatches;
    uint myRotationIndex;

  private:
    OCompletionPrivate *d;
};

// some more helper stuff
typedef OSortableValueList<QString> OCompletionMatchesList;
class OCompletionMatchesPrivate;

/**
 * This structure is returned by @ref OCompletion::allWeightedMatches .
 * It also keeps the weight of the matches, allowing
 * you to modify some matches or merge them with matches
 * from another call to allWeightedMatches(), and sort the matches
 * after that in order to have the matches ordered correctly
 *
 * Example (a simplified example of what Oonqueror's completion does):
 * <pre>
 * OCompletionMatches matches = completion->allWeightedMatches( location );
 * if( !location.startsWith( "www." ))
       matches += completion->allWeightedmatches( "www." + location" );
 * matches.removeDuplicates();
 * QStringList list = matches.list();
 * </pre>
 *
 * @short List for keeping matches returned from OCompletion
 */

class OCompletionMatches
    : public OCompletionMatchesList
{
  public:
    OCompletionMatches( bool sort );
    /**
     * @internal
     */
    OCompletionMatches( const OCompletionMatchesWrapper& matches );
    ~OCompletionMatches();
    /**
     * Removes duplicate matches. Needed only when you merged several matches
     * results and there's a possibility of duplicates.
     */
    void removeDuplicates();
    /**
     * Returns the matches as a QStringList.
     * @param sort if false, the matches won't be sorted before the conversion,
     *             use only if you're sure the sorting is not needed
     */
    QStringList list( bool sort = true ) const;
    /**
     * If sorting() returns false, the matches aren't sorted by their weight,
     * even if true is passed to list().
     */
    bool sorting() const {
        return _sorting;
    }
private:
    bool _sorting;
    OCompletionMatchesPrivate* d;
};

#endif // OCOMPLETION_H
