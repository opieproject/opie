/*
 * GPLv2 zecke@handhelds.org
 */

#include "dir_lister.h"

#include <lib/slavemaster.h>

/* OPIE */
#include <opie2/odebug.h>
#include <qpe/config.h>
#include <qpe/qpeapplication.h>
using namespace Opie::Core;

/* QT */
#include <qdir.h>
#include <qfileinfo.h>

Dir_DirLister::Dir_DirLister( bool list,bool rec,int recdepth )
    : PDirLister( "dir_dir_lister" )
{
    m_allFiles = list;
    m_recursive = rec;
    m_recDepth = recdepth;
    if (m_recDepth<1) m_recDepth = 1;
    if (m_recDepth>10) m_recDepth = 10;
    owarn << "All Files " << m_allFiles << "" << oendl;
    SlaveHelper::slaveConnectSignals( this );
    m_Filter = (m_allFiles?"*":"*.jpg;*.jpeg;*.JPG;*.PNG;*.GIF;*.BMP;*.png;*.bmp;*.gif");
}

QString Dir_DirLister::defaultPath()const {
    return QPEApplication::documentDir();
}

QString Dir_DirLister::setStartPath( const QString& path ) {
    m_currentDir.cd( path );
    if (!m_currentDir.exists() )
        m_currentDir.cd(defaultPath());


    return m_currentDir.absPath();
}

QString Dir_DirLister::currentPath()const {
    return m_currentDir.absPath();
}


QStringList Dir_DirLister::folders()const {
    QStringList dirs = m_currentDir.entryList( QDir::Dirs );
    dirs.remove(".");
    
    return dirs;
}

QStringList Dir_DirLister::recFiles(const QString&aPath,int currentDepth)const
{
    QStringList all;
    if (currentDepth>m_recDepth) return all;

    QString subPath;
    subPath = aPath;
    if (subPath.length()==0) {
        subPath=".";
    }
    QDir checkDir(currentPath()+"/"+aPath);

    QStringList p = checkDir.entryList( QDir::Dirs );
    all+=checkDir.entryList(m_Filter,QDir::Files|QDir::Readable);
    QStringList tmp;
    for (unsigned i = 0; i < p.count();++i) {
        if (p[i]=="." || p[i]=="..") continue;
        tmp =recFiles(subPath+"/"+p[i],currentDepth+1);
        for (unsigned j = 0; j < tmp.count();++j) {
            all.append(p[i]+"/"+tmp[j]);
        }
    }
    return all;
}

QStringList Dir_DirLister::files()const
{
    if (m_recursive) {
        odebug << "Startpfad: "<<m_currentDir.absPath()<<oendl;
        return recFiles("",0);
    }
    return m_currentDir.entryList(m_Filter,QDir::Files|QDir::Readable);
}

void Dir_DirLister::deleteImage( const QString& fl) {
    QFile::remove( fl );
}

void Dir_DirLister::thumbNail( const QString& str, int w, int h) {
    SlaveMaster::self()->thumbNail( str, w, h );
}

QImage Dir_DirLister::image( const QString& str, Factor f, int m) {
    return SlaveMaster::self()->image( str, f, m );
}

void Dir_DirLister::imageInfo( const QString& str) {
    SlaveMaster::self()->thumbInfo( str );
}

void Dir_DirLister::fullImageInfo( const QString& str) {
    SlaveMaster::self()->imageInfo( str );
}

QString Dir_DirLister::nameToFname(const QString&name)const
{
    return name;
}
