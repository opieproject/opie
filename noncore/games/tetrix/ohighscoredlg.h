/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/

class QWidget;
class QVBoxLayout;
class QListView;

#include <qdialog.h>
#include <list>

using namespace std;

struct t_playerData
{
	QString sName;
	int points;
	int level;
};

class OHighscore : public QWidget
{
	Q_OBJECT

	public:
		/*
		 *An OHighscore-Object contains all Points, level and playernames sorted in a stl::vector
		 */
		OHighscore( int , int );
	
		~OHighscore();

		/*
		 * is true if the player did a new highscore
		 */
		bool isNewhighscore;

		/*
		 * this inserts the new entry at the correct position
		 */
		void insertData( QString , int , int );
		
		list<t_playerData*> playerData;
		
		/*
		 * As Qt/e does not support QInputDialog I did that code myself
		 */
		QString getName();
	private:

		list<t_playerData*>::iterator iPlayerData;
		
		/*
		 * the lowest score in the highscorelist
		 */
		int lowest;

		/*
		 * the level of the highscore
		 */
		int pLevel;
		
		/*
		 * get all scores in a vector and give "lowest" a value
		 */
		void getList();
		
		/*
		 * sets the bool if the current score is in the top10
		 */
		void checkIfItIsANewhighscore( int );
};

class OHighscoreDialog : public QDialog
{
	Q_OBJECT
	
    public:
		OHighscoreDialog(OHighscore *highscore, QWidget *parent, const char *name = 0, bool modal = true );

	private:
		OHighscore *hs_;
	
		QVBoxLayout *vbox_layout;
		
		/*
		 * this method creates the QListView with all data
		 */
		void createHighscoreListView();

		QListView *list;
};
