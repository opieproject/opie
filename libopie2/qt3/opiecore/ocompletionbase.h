/*
                             This file is part of the Opie Project

                             Copyright (C) 2003 Michael Lauer <mickey@tm.informatik.uni-frankfurt.de>
                             Inspired by the KDE completion classes which are
                             Copyright (C) 2000 Dawit Alemayehu <adawit@kde.org>
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

#ifndef OCOMPLETIONBASE_H
#define OCOMPLETIONBASE_H

/**
 * An abstract base class for adding a completion feature
 * into widgets.
 *
 * This is a convenience class that provides the basic functions
 * needed to add text completion support into widgets.  All that
 * is required is an implementation for the pure virtual function
 * @ref setCompletedText.  Refer to @ref OLineEdit or @ref OComboBox
 * to see how easily such support can be added using this as a base
 * class.
 *
 * @short An abstract class for adding text completion support to widgets.
 * @author Dawit Alemayehu <adawit@kde.org>
 */

class OCompletionBase
{

  public:

    /**
     * Constants that represent the items whose short-cut
     * key-binding is programmable.  The default key-bindings
     * for these items are defined in @ref OStdAccel.
     */
    enum KeyBindingType {
        /**
         * Text completion (by default Ctrl-E).
         */
        TextCompletion,
        /**
         * Switch to previous completion (by default Ctrl-Up).
         */
        PrevCompletionMatch,
        /**
         * Switch to next completion (by default Ctrl-Down).
         */
        NextCompletionMatch,
        /**
         * Substring completion (by default Ctrl-T).
         */
        SubstringCompletion
    };


    // Map for the key binding types mentioned above.
    typedef QMap<KeyBindingType, OShortcut> KeyBindingMap;

    /**
     * Default constructor.
     */
    OCompletionBase();

    /**
     * Destructor.
     */
    virtual ~OCompletionBase();

    /**
     * Returns a pointer to the current completion object.
     *
     * If the object does not exist, it is automatically
     * created.  Note that the completion object created
     * here is used by default to handle the signals
     * internally.  It is also deleted when this object's
     * destructor is invoked.  If you do not want these
     * default settings, use @ref setAutoDeleteCompletionObject
     * and @ref setHandleSignals to change the behavior.
     * Alternatively, you can set the boolean parameter to
     * false to disable the automatic handling of the signals
     * by this object.  Note that the boolean argument will be
     * ignored if there already exists a completion object since
     * no new object needs to be created. You need to use either
     * @ref setHandleSignals or @ref setCompletionObject for
     * such cases depending on your requirement.
     *
     * @param hsig if true, handles signals internally.
     * @return a pointer the completion object.
     */
    OCompletion* completionObject( bool hsig = true );

    /**
     * Sets up the completion object to be used.
     *
     * This method assigns the completion object and sets it
     * up to automatically handle the completion and rotation
     * signals internally.  You should use this function if
     * you want to share one completion object among you widgets
     * or need to use a customized completion object.
     *
     * The object assigned through this method is not deleted
     * when this object's destructor is invoked unless you
     * explicitly call @ref setAutoDeleteCompletionObject after
     * calling this method.  Also if you do not want the signals
     * to be handled by an internal implementation, be sure to
     * set the bool argument to false.
     *
     * This method is also called when a completion-object is created
     * automatically, when completionObject() is called the first time.
     *
     * @param compObj a @ref OCompletion() or a derived child object.
     * @param hsig if true, handles signals internally.
     */
    virtual void setCompletionObject( OCompletion* /*compObj*/, bool hsig = true );

    /**
     * Enables this object to handle completion and rotation
     * events internally.
     *
     * This function simply assigns a boolean value that
     * indicates whether it should handle rotation and
     * completion events or not.  Note that this does not
     * stop the object from emitting signals when these
     * events occur.
     *
     * @param handle if true, handle completion & rotation internally.
     */
    virtual void setHandleSignals( bool /*handle*/ );

    /**
     * Returns true if the completion object is deleted
     * upon this widget's destruction.
     *
     * See @ref setCompletionObject() and @ref enableCompletion()
     * for details.
     *
     * @return true if the completion object
     */
    bool isCompletionObjectAutoDeleted() const {
        return m_delegate ? m_delegate->isCompletionObjectAutoDeleted() : m_bAutoDelCompObj;
    }

    /**
     * Sets the completion object when this widget's destructor
     * is called.
     *
     * If the argument is set to true, the completion object
     * is deleted when this widget's destructor is called.
     *
     * @param autoDelete if true, delete completion object on destruction.
    */
    void setAutoDeleteCompletionObject( bool autoDelete ) {
        if ( m_delegate )
            m_delegate->setAutoDeleteCompletionObject( autoDelete );
        else
            m_bAutoDelCompObj = autoDelete;
    }

    /**
     * Sets the widget's ability to emit text completion and
     * rotation signals.
     *
     * Invoking this function with @p enable set to @p false will
     * cause the completion & rotation signals not to be emitted.
     * However, unlike setting the completion object to @p NULL
     * using @ref setCompletionObject, disabling the emition of
     * the signals through this method does not affect the current
     * completion object.
     *
     * There is no need to invoke this function by default.  When a
     * completion object is created through @ref completionObject or
     * @ref setCompletionObject, these signals are set to emit
     * automatically.  Also note that disabling this signals will not
     * necessarily interfere with the objects ability to handle these
     * events internally.  See @ref setHandleSignals.
     *
     * @param enable if false, disables the emition of completion & rotation signals.
     */
    void setEnableSignals( bool enable ) {
        if ( m_delegate )
            m_delegate->setEnableSignals( enable );
        else
            m_bEmitSignals = enable;
    }

    /**
     * Returns true if the object handles the signals
     *
     * @return true if this signals are handled internally.
     */
    bool handleSignals() const { return m_delegate ? m_delegate->handleSignals() : m_bHandleSignals; }

    /**
     * Returns true if the object emits the signals
     *
     * @return true if signals are emitted
     */
    bool emitSignals() const { return m_delegate ? m_delegate->emitSignals() : m_bEmitSignals; }

    /**
     * Sets the type of completion to be used.
     *
     * The completion modes supported are those defined in
     * @ref OGlobalSettings().  See below.
     *
     * @param mode Completion type:
     *   @li CompletionNone:  Disables completion feature.
     *   @li CompletionAuto:  Attempts to find a match &
     *                        fills-in the remaining text.
     *   @li CompletionMan:   Acts the same as the above
     *                        except the action has to be
     *                        manually triggered through
     *                        pre-defined completion key.
     *   @li CompletionShell: Mimics the completion feature
     *                        found in typical *nix shell
     *                        environments.
     *   @li CompletionPopup: Shows all available completions at once,
     *                        in a listbox popping up.
     */
    virtual void setCompletionMode( OGlobalSettings::Completion mode );

    /**
     * Returns the current completion mode.
     *
     * The return values are of type @ref OGlobalSettings::Completion.
     * See @ref setCompletionMode() for details.
     *
     * @return the completion mode.
     */
    OGlobalSettings::Completion completionMode() const {
        return m_delegate ? m_delegate->completionMode() : m_iCompletionMode;
    }

    /**
     * Sets the key-binding to be used for manual text
     * completion, text rotation in a history list as
     * well as a completion list.
     *
     *
     * When the keys set by this function are pressed, a
     * signal defined by the inheriting widget will be activated.
     * If the default value or 0 is specified by the second
     * parameter, then the key-binding as defined in the global
     * setting should be used.  This method returns false value
     * for @p key is negative or the supplied key-binding conflicts
     * with the ones set for one of the other features.
     *
     * NOTE: To use a modifier key (Shift, Ctrl, Alt) as part of
     * the key-binding simply simply @p sum up the values of the
     * modifier and the actual key.  For example, to use CTRL+E as
     * a key binding for one of the items, you would simply supply
     * @p "Qt::CtrlButton + Qt::Key_E" as the second argument to this
     * function.
     *
     * @param item the feature whose key-binding needs to be set:
     *
     * @li TextCompletion	the manual completion key-binding.
     * @li PrevCompletionMatch	the previous match key for multiple completion.
     * @li NextCompletionMatch	the next match key for for multiple completion.
     * @li SubstringCompletion  the key for substring completion
     *
     * @param key key-binding used to rotate down in a list.
     *
     * @return  true if key-binding can successfully be set.
     * @see #getKeyBinding
     */
    bool setKeyBinding( KeyBindingType /*item*/ , const OShortcut& cut );

    /**
     * Returns the key-binding used for the specified item.
     *
     * This methods returns the key-binding used to activate
     * the feature feature given by @p item.  If the binding
     * contains modifier key(s), the SUM of the modifier key
     * and the actual key code are returned.
     *
     * @return the key-binding used for the feature given by @p item.
     * @see #setKeyBinding
     */
    const OShortcut& getKeyBinding( KeyBindingType item ) const {
        return m_delegate ? m_delegate->getKeyBinding( item ) : m_keyMap[ item ];
    }

    /**
     * Sets this object to use global values for key-bindings.
     *
     * This method changes the values of the key bindings for
     * rotation and completion features to the default values
     * provided in OGlobalSettings.
     *
     * NOTE: By default inheriting widgets should uses the
     * global key-bindings so that there will be no need to
     * call this method.
     */
    void useGlobalKeyBindings();

    /**
     * A pure virtual function that must be implemented by
     * all inheriting classes.
     *
     * This function is intended to allow external completion
     * implementations to set completed text appropriately.  It
     * is mostly relevant when the completion mode is set to
     * CompletionAuto and CompletionManual modes. See
     * @ref OCompletionBase::setCompletedText.
     * Does nothing in CompletionPopup mode, as all available
     * matches will be shown in the popup.
     *
     * @param text the completed text to be set in the widget.
     */
    virtual void setCompletedText( const QString& text ) = 0;

    /**
     * A pure virtual function that must be implemented by
     * all inheriting classes.
     *
     */
    virtual void setCompletedItems( const QStringList& items ) = 0;

    /**
     * Returns a pointer to the completion object.
     *
     * This method is only different from @ref completionObject()
     * in that it does not create a new OCompletion object even if
     * the internal pointer is @p NULL. Use this method to get the
     * pointer to a completion object when inheriting so that you
     * won't inadvertently create it!!
     *	
     * @returns the completion object or NULL if one does not exist.
     */
    OCompletion* compObj() const { return m_delegate ? m_delegate->compObj() : (OCompletion*) m_pCompObj; }

protected:
    /**
     * Returns a key-binding map
     *
     * This method is the same as @ref getKeyBinding() except it
     * returns the whole keymap containing the key-bindings.
     *
     * @return the key-binding used for the feature given by @p item.
     */
    KeyBindingMap getKeyBindings() const { return m_delegate ? m_delegate->getKeyBindings() : m_keyMap; }

    void setDelegate( OCompletionBase *delegate );
    OCompletionBase *delegate() const { return m_delegate; }

private:
    // This method simply sets the autodelete boolean for
    // the completion object, the emit signals and handle
    // signals internally flags to the provided values.
    void setup( bool, bool, bool );

    // Flag that determined whether the completion object
    // should be deleted when this object is destroyed.
    bool m_bAutoDelCompObj;
    // Determines whether this widget handles completion signals
    // internally or not
    bool m_bHandleSignals;
    // Determines whether this widget fires rotation signals
    bool m_bEmitSignals;
    // Stores the completion mode locally.
    OGlobalSettings::Completion m_iCompletionMode;
    // Pointer to Completion object.
    QGuardedPtr<OCompletion> m_pCompObj;
    // Keybindings
    KeyBindingMap m_keyMap;
    // we may act as a proxy to another OCompletionBase object
    OCompletionBase *m_delegate;

  // FIXME: Revise this for Opie?
  //protected:
  //  virtual void virtual_hook( int id, void* data );
  private:
    OCompletionBasePrivate *d;
};

#endif // OCOMPLETIONBASE_H

