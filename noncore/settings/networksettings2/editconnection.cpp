#include <opie2/odebug.h>
#include <qlistview.h>
#include <qwidgetstack.h>
#include <qframe.h>
#include <qcombobox.h>
#include <qtabwidget.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qheader.h>
#include <qpainter.h>
#include <qcheckbox.h>
#include <qlabel.h>

#include "editconnection.h"
#include "resources.h"
#include "netnode.h"

//
//
// THESE TO GIVE BETTER FEEDBACK ABOUT DISABLED ITEMS
//
//

class MyQCheckListItem : public QCheckListItem
{
public:
    MyQCheckListItem( QListView *parent, const QString & S, Type T ) :
      QCheckListItem( parent, S, T ) { }
    MyQCheckListItem( QCheckListItem *parent, const QString & S, Type T ) :
      QCheckListItem( parent, S, T ) { }
    MyQCheckListItem( QListViewItem *parent, const QString & S, Type T ) :
      QCheckListItem( parent, S, T ) { }

    MyQCheckListItem( QListView *parent, const QString & S ) :
      QCheckListItem( parent, S, QCheckListItem::Controller ) { }
    MyQCheckListItem( QCheckListItem *parent, const QString & S ) :
      QCheckListItem( parent, S, QCheckListItem::Controller ) { }
    MyQCheckListItem( QListViewItem *parent, const QString & S ) :
      QCheckListItem( parent, S, QCheckListItem::Controller ) { }

    virtual void paintCell( QPainter *p, const QColorGroup &cg,
			    int column, int width, int alignment );

};

void MyQCheckListItem::paintCell( QPainter *p, const QColorGroup &cg,
				 int column, int width, int alignment )
{
    QColorGroup _cg( cg );
    QColor c = _cg.text();
    if ( ! isSelectable() )
	_cg.setColor( QColorGroup::Text, Qt::lightGray );
    QCheckListItem::paintCell( p, _cg, column, width, alignment );
    _cg.setColor( QColorGroup::Text, c );
}

class MyQListViewItem : public QListViewItem
{
public:
    MyQListViewItem( QListView *parent, const QString & S ) :
      QListViewItem( parent, S ) { }
    MyQListViewItem( QListViewItem *parent, const QString & S ) :
      QListViewItem( parent, S ) { }

    virtual void paintCell( QPainter *p, const QColorGroup &cg,
			    int column, int width, int alignment );

};

void MyQListViewItem::paintCell( QPainter *p, const QColorGroup &cg,
				 int column, int width, int alignment )
{
    QColorGroup _cg( cg );
    QColor c = _cg.text();
    if ( ! isSelectable() )
	_cg.setColor( QColorGroup::Text, Qt::lightGray );
    QListViewItem::paintCell( p, _cg, column, width, alignment );
    _cg.setColor( QColorGroup::Text, c );
}

//
//
// REAL GUI
//
//

bool EditConnection::AutoCollapse = 1;

EditConnection::EditConnection( QWidget* parent ) :
      EditConnectionGUI( parent, 0, TRUE ), TmpCollection() {

      Tab_TB->setTabEnabled( Setup_FRM, FALSE );
      Setup_FRM->setEnabled( FALSE );

      TmpIsValid = 0;
      SelectedNodes = 0;

      AutoCollapse_CB->setChecked( AutoCollapse );

      Mapping = new QPtrDict<ANetNode>;
      Mapping->setAutoDelete( FALSE );
      Nodes_LV->header()->hide();
      // popluate tree with all NetNodes 
      buildFullTree();
}

NodeCollection * EditConnection::getTmpCollection( void ) {

      if( TmpIsValid ) 
        // content is stil OK
        return &(TmpCollection);

      // reset collection -> delete all NEW NetNodes
      for( QListIterator<ANetNodeInstance> it(TmpCollection);
           it.current();
           ++it ) {
        if( it.current()->isNew() ) {
          delete it.current();
        }
      }
      TmpCollection.clear();

      // update content
      QListViewItem * it = Nodes_LV->firstChild();
      ANetNode * NN;

      // start iter (if there is a collection)
      /*

          a node collection is sorted from the toplevel
          node to the deepest node

      */
      ANetNodeInstance * NNI = 
        (SelectedNodes) ? SelectedNodes->first() : 0 ;

      TmpCollection.setModified( 0 );

      // the listview always starts with the toplevel 
      // hierarchy.  This is always a controller item
      while ( it ) {
        NN = (*Mapping)[it];
        if( NN == 0 ) {
          // this item is a controller -> 
          // has radio items as children -> 
          // find selected one 
          it = it->firstChild();
          while( it ) {
            if( ((QCheckListItem *)it)->isOn() ) {
              // this radio is selected -> go deeper
              break;
            }
            it = it->nextSibling();
          }

          if( ! it ) {
            owarn  << "Radio not selected" << oendl;
            TmpIsValid = 0;
            return 0;
          }

          // it now contains selected radio
          NN = (*Mapping)[it];
        }

        // NN here contains the netnode of the
        // current item -> this node needs to
        // be stored in the collection
        if( NNI == 0 ||
            it->text(0) != NNI->nodeClass()->name() ) {
          // new item not in previous collection
          ANetNodeInstance * NNI = NN->createInstance();
          NNI->initialize();
          // this node type not in collection
          TmpCollection.append( NNI );
          // master collection changed because new item in it
          TmpCollection.setModified( 1 );
          // no more valid items in old list
          NNI = 0;
        } else {
          // already in list -> copy pointer
          TmpCollection.append( NNI );
          NNI = SelectedNodes->next();
        }

        // go deeper to next level
        // this level is can be a new controller
        // or an item
        it = it->firstChild();
      }

      TmpIsValid = 1;
      return &(TmpCollection);
}

// pass a connection NodeCollection to be edited
void EditConnection::setConnection( NodeCollection * NC ) {
      ANetNodeInstance * NNI;
      ANetNode * NN;

      SelectedNodes = NC;
      Name_LE->setText( NC->name() );
      NNI = NC->first();

      // show configure tabl
      Tab_TB->setCurrentPage( 1 );

      // valid colledction
      Tab_TB->setTabEnabled( Setup_FRM, FALSE );
      Setup_FRM->setEnabled( FALSE );

      // select items in collection
      QListViewItem * it = Nodes_LV->firstChild();
      bool Found;

      TmpIsValid = 0;

      while ( it ) {
        NN = (*Mapping)[it];
        if( NN == 0 ) {
          // this item is a controller -> 
          // has radio items as children -> 
          // find selected one 
          it = it->firstChild();
          Found = 0;
          while( it ) {
            if( NNI && it->text(0) == NNI->nodeClass()->name() ) {
              // this radio is part of the collection
              ((QCheckListItem *)it)->setOn( 1 );
              updateGUI( it, NNI->nodeClass() );
              // check its children
              Found = 1;
              it = it->firstChild();
              NNI = SelectedNodes->next();
              // do not bother to check other items
              break;
            }
            it = it->nextSibling();
          }

          if( ! Found ) {
            // this means that this level is NOT present in collection
            // probably INCOMPATIBEL collection OR Missing plugin
            QMessageBox::warning(
                0, 
                tr( "Error presentig Connection" ),
                tr( "<p>Old connection or missing plugin \"<i>%1</i>\"</p>" ).
                    arg(NNI->nodeClass()->name()) );
            return;
          }

          // it now contains selected radio
          NN = (*Mapping)[it];
        } else {
          // automatic selection 
          if( NNI == 0 ||  it->text(0) != NNI->nodeClass()->name() ) {
            // should exist and be the same
            if( NNI ) {
              QMessageBox::warning(
                  0, 
                  tr( "Error presentig Connection" ),
                  tr( "<p>Old connection or missing plugin \"<i>%1</i>\"</p>" ).
                      arg(NNI->nodeClass()->name()) );
            } else {
              QMessageBox::warning(
                  0, 
                  tr( "Error presentig Connection" ),
                  tr( "<p>Missing connection\"<i>%1</i>\"</p>" ).
                      arg(it->text(0)) );
            }
            return;
          }
          it = it->firstChild();
        }
      }
}

// get result of editing (either new OR updated collection
NodeCollection * EditConnection::connection( void ) {

      if( SelectedNodes == 0 ) {
        // new collection 
        SelectedNodes = new NodeCollection;
      }

      // clean out old entries
      SelectedNodes->clear();

      // transfer 
      for( QListIterator<ANetNodeInstance> it(TmpCollection);
           it.current();
           ++it ) {
        SelectedNodes->append( it.current() );
      }

      if( TmpCollection.isModified() )
        SelectedNodes->setModified( 1 );

      if( SelectedNodes->name() != Name_LE->text() ) {
        SelectedNodes->setName( Name_LE->text() );
        SelectedNodes->setModified( 1 );
      }

      return SelectedNodes;
}

// Build device tree -> start 
void EditConnection::buildFullTree( void ) {
    ANetNode * NN;

    // toplevel item
    MyQCheckListItem * TheTop = new MyQCheckListItem( 
        Nodes_LV, 
        NSResources->netNode2Name("fullsetup"),
        QCheckListItem::Controller );
    TheTop->setOpen( TRUE );
    Description_LBL->setText( 
          NSResources->netNode2Description( "fullsetup" ) );
    Nodes_LV->setSelected( TheTop, TRUE );

    // find all Nodes that are toplevel nodes -> ie provide
    // TCP/IP Connection
    for( QDictIterator<NetNode_t> Iter(NSResources->netNodes());
         Iter.current();
         ++Iter ) {

      NN = Iter.current()->NetNode;

      if( ! NN->isToplevel() ) {
        continue;
      }

      MyQCheckListItem * it = new MyQCheckListItem( TheTop, 
          NN->name(), 
          QCheckListItem::RadioButton );
      it->setPixmap( 0, 
                     NSResources->getPixmap( NN->pixmapName() )
                   );
      // remember that this node maps to this listitem
      Mapping->insert( it, NN );
      buildSubTree( it, NN );
    }
}

// Build device tree -> help function 
void EditConnection::buildSubTree( QListViewItem * it, ANetNode *NN ) {
    ANetNode::NetNodeList & NNL = NN->alternatives();

    if( NNL.size() > 1 ) {
      // this node has alternatives -> needs radio buttons
      it = new MyQCheckListItem( 
        it, 
        NSResources->netNode2Name(NN->needs()[0]),
        QCheckListItem::Controller );
      it->setSelectable( FALSE );
    }

    for ( unsigned int i=0; i < NNL.size(); i++ ) {
      QListViewItem * CI;
      if( NNL.size() > 1 ) {
        // generate radio buttons
        CI = new MyQCheckListItem( 
                (QCheckListItem *)it, 
                NNL[i]->name(), QCheckListItem::RadioButton );
        // remember that this node maps to this listitem
        CI->setPixmap( 0, NSResources->getPixmap( NNL[i]->pixmapName() ) );
        Mapping->insert( CI, NNL[i] );
        CI->setSelectable( FALSE );
      } else {
        // Single item
        CI = new MyQListViewItem( it, NNL[i]->name() );
        // remember that this node maps to this listitem
        Mapping->insert( CI, NNL[i] );
        CI->setSelectable( FALSE );
        CI->setPixmap( 0, NSResources->getPixmap( NNL[i]->pixmapName() ) );
      }
      buildSubTree( CI, NNL[i] );
    }
}

// Clicked ok OK button
void EditConnection::accept( void ) {
    if( ! haveCompleteConfig( 0 ) || Name_LE->text().isEmpty() ) {
      QMessageBox::warning(
          0, 
          tr( "Closing Connection Setup" ),
          tr( "Definition not complete or no name" ) );
      return;
    }

    // check if all devices have acceptable input
    getTmpCollection();
    { ANetNodeInstance * NNI;
      QString S;

      for( QListIterator<ANetNodeInstance> it(TmpCollection);
           it.current();
           ++it ) {
        NNI = it.current();
        // widget must show its own problems
        S = NNI->acceptable();
        if( ! S.isEmpty() ) {
          QMessageBox::warning(
              0, 
              tr( "Cannot save" ),
              S );
          return;
        }
        NNI->commit();

        if( NNI->isModified() ) {
          TmpCollection.setModified( 1 );
          // commit the data
        }
      }
    }

    QDialog::accept();
}

// triggered by CB
void EditConnection::SLOT_AutoCollapse( bool b ) {
    AutoCollapse = b;
}

// clicked on node in tree -> update GUI
void EditConnection::SLOT_SelectNode( QListViewItem * it ) {
    ANetNode * NN;
    if( it == 0 || it->depth() == 0 ) {
      Description_LBL->setText( 
          NSResources->netNode2Description( "fullsetup" ) );
      // topevel or no selection
      return;
    }

    // store conversion from lvitem to node
    NN = (*Mapping)[ it ];

    if( ! NN ) {
      // intermediate node
      NN = (*Mapping)[ it->parent() ];
      if( NN ) {
        // figure out type of this node -> produce mesage
        Description_LBL->setText( NSResources->netNode2Description(
                                  NN->needs()[0]) );
      } else {
        Description_LBL->setText( "" );
      }
      return;
    }

    Description_LBL->setText( NN->nodeDescription() );

    if( ! it->isSelectable() ) {
      return;
    }

    if( ! ((QCheckListItem *)it)->isOn() ) {
      // clicked on line but NOT on Check or Radio item
      return;
    }

    // item has really changed -> update 
    TmpIsValid = 0;
    updateGUI( it, NN );
}

// cliecked on TAB to go to setup
void EditConnection::SLOT_AlterTab( const QString & S ) {
    if( S == tr( "Setup" ) && Setup_FRM->isEnabled() ) {
      // switched to setup -> update CB and populate ws with
      // forms for devices

      if( ! TmpIsValid ) {
        getTmpCollection();

        // clear CB and Ws
        { QWidget * W;
          int i = 0;

          Devices_CB->clear();
          while( ( W = Setup_WS->widget( i ) ) ) {
            Setup_WS->removeWidget( W );
            i ++;
          }
        }

        // update CB
        // and populate WidgetStack
        { ANetNodeInstance * NNI;
          QListIterator<ANetNodeInstance> it(TmpCollection);
          int i = 0;
          QWidget * W;

          for ( ; it.current(); ++it ) {
            NNI = it.current();
            Devices_CB->insertItem(
                NSResources->getPixmap( NNI->nodeClass()->pixmapName() ),
                NNI->nodeClass()->name()
            );

            // add edit widget
            W = NNI->edit( Setup_WS );
            if( ! W) {
              W = new QLabel( Setup_WS, 
                              tr("No configuration required"));
            }
            Setup_WS->addWidget( W , i );
            i ++;
          }
        }
        Setup_WS->raiseWidget( 0 );
      } // still valid
    }
}

// update visual feedback of selection state
void EditConnection::updateGUI( QListViewItem * it, ANetNode * NN ) {

    bool HCC = haveCompleteConfig( it );
    Tab_TB->setTabEnabled( Setup_FRM, HCC );
    Setup_FRM->setEnabled( HCC );

    // disable children of all siblings at same level
    QListViewItem * Sbl = it->parent()->firstChild();
    while( Sbl ) {
      if ( Sbl != it ) {
        disableTree( Sbl->firstChild(), FALSE );
        Sbl->setSelectable( TRUE );
        if( AutoCollapse )
          Sbl->setOpen( FALSE );
      }
      Sbl = Sbl->nextSibling();
    }

    // enable selected path (as deep as it goes
    it->setOpen( TRUE );
    enablePath( it->firstChild(), 
                (it->depth()==1) ? 
                    1 : // toplevel always alternatives
                    (NN->alternatives().size() > 1) );
}

void EditConnection::disableTree( QListViewItem * it, bool Mode ) {
    while( it ) {
      // disable sbl's chidren
      it->setSelectable( Mode );
      if( AutoCollapse )
        it->setOpen( Mode );
      disableTree( it->firstChild(), Mode );
      it = it->nextSibling();
    }
}

// pah : ParentHasAlternatives
void EditConnection::enablePath( QListViewItem * it, bool pha ) {
    while( it ) {
      ANetNode * NN;
      NN = (*Mapping)[it];
      if( NN ) {
        if( pha ) {
          bool doOn = ((QCheckListItem *)it)->isOn();
          // we are a checklistitem for sure
          it->setSelectable( TRUE );
          if( AutoCollapse && ! doOn )
            it->setOpen( doOn );
          if( doOn ) {
            // selected alternative
            enablePath( it->firstChild(), 
                        NN->alternatives().size() > 1);
          } else {
            // non-selected alternative
            disableTree( it->firstChild(), FALSE);
          }
        } else {
          // we are single subitem
          it->setSelectable( TRUE );
          it->setOpen( TRUE );
          enablePath( it->firstChild(),
                      NN->alternatives().size() > 1);
        }
      } else {
        // controller node
        it->setSelectable( TRUE );
        it->setOpen( TRUE );
        enablePath( it->firstChild(), pha );
      }
      it = it->nextSibling();
    }
}

// do we have a complete configuration (all needs are provided for ?)
bool EditConnection::haveCompleteConfig( QListViewItem * it ) {

    // check if all below this level is selected
    it = ( it ) ?it : Nodes_LV->firstChild();
    ANetNode *NN;
    bool Found;

    while ( it ) {
      NN = (*Mapping)[it];
      if( NN == 0 ) {
        // this item is a controller -> 
        // has radio items as children -> 
        // find selected one 
        it = it->firstChild();
        Found = 0;
        while( it ) {
          if( ((QCheckListItem *)it)->isOn() ) {
            Found = 1;
            // go deeper
            it = it->firstChild();
            break;
          }
          it = it->nextSibling();
        }

        if( ! Found ) {
          return 0; // no not complete -> a radio should have been chkd
        }

        // it now contains selected radio
        NN = (*Mapping)[it];
      } else {
        // automatic selection 
        it = it->firstChild();
      }
    }
    return 1;
}
