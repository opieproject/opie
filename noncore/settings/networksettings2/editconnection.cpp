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
      { ANetNodeInstance * NNI;
        for( QListIterator<ANetNodeInstance> it(TmpCollection);
             it.current();
             ++it ) {
          if( it.current()->isNew() ) {
            delete it.current();
          }
        }
        TmpCollection.clear();
      }

      // update content
      QListViewItem * it = Nodes_LV->firstChild();
      ANetNode * NN;
      // start iter
      ANetNodeInstance * NNI = 
        (SelectedNodes) ? SelectedNodes->first() : 0 ;

      TmpCollection.setModified( 0 );

      while ( it ) {
        NN = (*Mapping)[it];
        if( NN == 0 ) {
          // child is controller -> has sub radio
          // check if one radio is selected
          it = it->firstChild();
          while( it ) {
            if( ((QCheckListItem *)it)->isOn() ) {
              // this radio is selected -> go deeper
              if( SelectedNodes == 0 || 
                  NNI == 0 ||
                  NNI->netNode()->nodeName() != it->text(0) ) {
                // new item not in previous collection
                ANetNodeInstance * NNI = (*Mapping)[it]->createInstance();
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
              it = it->firstChild();
              // do not bother to check other items
              break;
            }
            it = it->nextSibling();
          }
        } else {
          // check children
          it = it->firstChild();
        }
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
        // listitem corresponds to netnode 
        NN = (*Mapping)[it];
        if( NN == 0 ) {
          // child is controller -> has sub radio
          QString Ctr = it->text(0);
          // check if one radio is selected
          it = it->firstChild();
          Found = 0;
          while( it ) {
            if( NNI && NNI->netNode()->nodeName() == it->text(0) ) {
              // this radio is part of the collection
              ((QCheckListItem *)it)->setOn( 1 );
              updateGUI( it, NNI->netNode() );
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
                    arg(Ctr) );
            return;
          }
        } else {
          // automatic item -> check children
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
      { ANetNodeInstance * NNI;

        for( QListIterator<ANetNodeInstance> it(TmpCollection);
             it.current();
             ++it ) {
          SelectedNodes->append( it.current() );
        }
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

    // find all Nodes that care toplevel nodes -> ie provide
    // TCP/IP Connection
    for( QDictIterator<NetNode_t> Iter(NSResources->netNodes());
         Iter.current();
         ++Iter ) {

      NN = Iter.current()->NetNode;

      if( ! NN->isToplevel() ) {
        continue;
      }

      MyQCheckListItem * it = new MyQCheckListItem( TheTop, 
          NN->nodeName(), 
          QCheckListItem::RadioButton );
      it->setPixmap( 0, NSResources->getPixmap( "Devices/commprofile" ) );
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
        NSResources->netNode2Name(NNL[0]->provides()),
        QCheckListItem::Controller );
      it->setSelectable( FALSE );
    }

    for ( unsigned int i=0; i < NNL.size(); i++ ) {
      QListViewItem * CI;
      if( NNL.size() > 1 ) {
        // generate radio buttons
        CI = new MyQCheckListItem( 
                (QCheckListItem *)it, 
                NNL[i]->nodeName(), QCheckListItem::RadioButton );
        // remember that this node maps to this listitem
        CI->setPixmap( 0, NSResources->getPixmap( NNL[i]->pixmapName() ) );
        Mapping->insert( CI, NNL[i] );
        CI->setSelectable( FALSE );
      } else {
        // Single item
        CI = new MyQListViewItem( it, NNL[i]->nodeName() );
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
      NN = (*Mapping)[ it->firstChild() ];
      if( NN ) {
        // figure out type of this node -> produce mesage
        Description_LBL->setText( NSResources->netNode2Description(NN->provides()) );
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
                NSResources->getPixmap( NNI->netNode()->pixmapName() ),
                NNI->netNode()->nodeName()
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
    if( it == 0 || ((QCheckListItem *)it)->isOn() ) {
      // check children
      it = (it) ? it->firstChild() : Nodes_LV->firstChild() ;
      while ( it ) {
        if( ((QCheckListItem *)it)->type() == 
            QCheckListItem::Controller ) {
          // child is controller -> has sub radio
          // check if one radio is selected
          it = it->firstChild();
          while( it ) {
            if( ((QCheckListItem *)it)->isOn() ) {
              // this radio is selected -> go deeper
              it = it->firstChild();
              if( ! it ) {
                // was deepest level
                return 1;
              }
              // do not bother to check other items
              break;
            }
            it = it->nextSibling();
          }
          if( ! it ) {
            // no radio selected
            return 0;
          }
        } else {
          // check children
          it = it->firstChild();
        }
      }
      // deepest level -> all is still OK
      return 1;
    } // was not ON
    return 0;
}
