#include "moveengine.h"

#include <qtimer.h>
#include <stdlib.h>

MoveEngine::MoveEngine()
        : QObject()
{
    int offset=7;
    int a=0; //counter variable
    int xfill[]={210,185,170,155,140,125,110,85,70,55,40,25,10,10,25,40,55,70,85,110,125,140,155,170,185,210};
    for(a=0;a<26;a++)
    {
        x_coord[a]=xfill[a];
    }

    int yfill[]={10,25,40,55,70,10+offset,25+offset,40+offset,55+offset,25,40,55, 25+offset,40+offset,40};
    int zfill[]={1,1,1,1,1,2,2,2,2,3,3,3,4,4,5};
    for(a=0;a<15;a++)
    {
        yup_coord[a]=yfill[a];
        ylow_coord[a]=185-(yfill[a]);
        z_coord[a]=zfill[a];
    }
    for(a=0;a<5;a++)
    {
        if(a<3)
        {
            x_fin1[a]=65+a*15;
            x_fin2[a]=155-a*15;
        }
        y_fin[a]=225-a*5;
    }
    z_fin=1;

    reset();
}

MoveEngine::~MoveEngine()
{}

void MoveEngine::position(Pieces& pieces,bool non_qte)
{
    int player1_counter=0;
    int player2_counter=0;

    //non qte styles are smaller !!
    int offset=(non_qte) ? 5 : 0;

    for(int a=0;a<28;a++)
    {
        for(int b=0;b<abs(population[a].total);b++)
        {
            if(population[a].total>0) //player 1 pieces
            {
                pieces.player1[player1_counter].x=x_coord[a]-offset;
                if(a>=0 && a<13)
                {
                    pieces.player1[player1_counter].y=yup_coord[b]-offset;
                    pieces.player1[player1_counter].z=z_coord[b];
                    pieces.player1[player1_counter].side=false;
                    player1_counter++;
                }
                else if(a>12 && a<26)
                {
                    pieces.player1[player1_counter].y=ylow_coord[b]-offset;
                    pieces.player1[player1_counter].z=z_coord[b];
                    pieces.player1[player1_counter].side=false;
                    player1_counter++;
                }
                else if(a==26)
                {
                    if(b<5)
                    {
                        pieces.player1[player1_counter].x=x_fin1[0]-offset;
                        pieces.player1[player1_counter].y=y_fin[b]-offset;
                        pieces.player1[player1_counter].z=z_fin;
                    }
                    else if(b>=5 && b<10)
                    {
                        pieces.player1[player1_counter].x=x_fin1[1]-offset;
                        pieces.player1[player1_counter].y=y_fin[b-5]-offset;
                        pieces.player1[player1_counter].z=z_fin;
                    }
                    else
                    {
                        pieces.player1[player1_counter].x=x_fin1[2]-offset;
                        pieces.player1[player1_counter].y=y_fin[b-10]-offset;
                        pieces.player1[player1_counter].z=z_fin;
                    }
                    pieces.player1[player1_counter].side=true;
                    player1_counter++;

                }
            }

            else if(population[a].total<0) //player 2 pieces
            {
                pieces.player2[player2_counter].x=x_coord[a]-offset;
                if(a>=0 && a<13)
                {
                    pieces.player2[player2_counter].y=yup_coord[b]-offset;
                    pieces.player2[player2_counter].z=z_coord[b];
                    pieces.player2[player2_counter].side=false;
                    player2_counter++;
                }
                else if(a>12 && a<26)
                {
                    pieces.player2[player2_counter].y=ylow_coord[b]-offset;
                    pieces.player2[player2_counter].z=z_coord[b];
                    pieces.player2[player2_counter].side=false;
                    player2_counter++;
                }
                else if(a==27)
                {
                    if(b<5)
                    {
                        pieces.player2[player2_counter].x=x_fin2[0]-offset;
                        pieces.player2[player2_counter].y=y_fin[b]-offset;
                        pieces.player2[player2_counter].z=z_fin;
                    }
                    else if(b>=5 && b<10)
                    {
                        pieces.player2[player2_counter].x=x_fin2[1]-offset;
                        pieces.player2[player2_counter].y=y_fin[b-5]-offset;
                        pieces.player2[player2_counter].z=z_fin;
                    }
                    else
                    {
                        pieces.player2[player2_counter].x=x_fin2[2]-offset;
                        pieces.player2[player2_counter].y=y_fin[b-10]-offset;
                        pieces.player2[player2_counter].z=z_fin;
                    }
                    pieces.player2[player2_counter].side=true;
                    player2_counter++;

                }
            }
        }
    }
}

void MoveEngine::diceroll(const int& newplayer,const int& face1,const int& face2,const int& face3,const int& face4,bool computer)
{
    checkstate();
    player=newplayer;
    otherplayer=(player==1) ? 2 : 1;
    dice[0]=face1;
    dice[1]=face2;
    dice[2]=face3;
    dice[3]=face4;
    marker_current=-1;
    if(getPossibleMoves()==0)
    {
        emit nomove();
        return; // player will be changed
    }
    if(!computer)
        return; //human intervention required

    QTimer::singleShot(2000,this,SLOT(automove()));
}


void MoveEngine::automove()
{
    //the maximimum possibility
    int maxpos=0;
    //the position in the moves array
    int from=-1;
    int to=-1;
    //dice1 or dice 2 ??
    int index_dice=0;
    for(int counter=0;counter<26;counter++)
    {
        int a=(player==1) ? counter : 25-counter;
        for(int b=0;b<4;b++)
        {
            if(moves[a].weight[b]>maxpos)
            {
                maxpos=moves[a].weight[b];
                from=a;
                to=moves[a].to[b];
                index_dice=b+1;
            }
        }
    }
    move(from,to,index_dice);
}


void MoveEngine::boardpressed(const int& x,const int& y,Marker& marker,bool non_qte)
{
    //get the position of the mouse click
    bool upper=true;
    bool found=false;

    int offset=(non_qte) ? 5 : 0;

    if(y<=85) // board slots 0 to 12
        marker.y_current=0;
    else if(y>=105) //board slots 13 to 25
    {
        marker.y_current=195-2*offset;
        upper=false;
    }

    int index=13; // the clicked board slot

    while(index<25 && !found)
    {
        if(x>=x_coord[index] && x<x_coord[index+1])
        {
            marker.x_current=x_coord[index];
            found=true;
            ;
        }
        else
        {
            index++;
        }
    }
    if(!found)
    {
        marker.x_current=x_coord[25];
        index=25;
    }
    if(upper)
    {
        index=25-index;
    }

    int a=0;
    int usedice=-1;
    int dice_value=7;
    for(a=0;a<4;a++)
    {
        if(index==marker_next[a] && marker_next[a]!=-1 && dice_value>dice[a])
        {
          usedice=a;
          dice_value=dice[0];
         }        
    }  
    if(usedice!=-1)
    {
        move(marker_current,marker_next[usedice],usedice+1);
        nomarker(marker);
        return;

    }


    if(dice[0]==7 && dice[1]==7 && dice[2]==7 && dice[3]==7) //no dice rolled
    {
        nomarker(marker);
        return;
    }
    else if(fieldColor(index)==player)
    {
        marker.visible_current=true;
        marker_current=index;
    }
    else
    {
        nomarker(marker);
        return;
    }

    for(a=0;a<4;a++)
    {
        if(moves[index].weight[a]>0)
        {
            int nextfield=moves[index].to[a];
            marker.x_next[a]=x_coord[nextfield];
            marker_next[a]=nextfield;
            if(nextfield<13) //upper half
                marker.y_next[a]=0;
            else //lower half
                marker.y_next[a]=195-2*offset;
            marker.visible_next[a]=true;
        }
        else
        {
            marker.x_next[a]=0;
            marker.y_next[a]=0;
            marker_next[a]=-1;
            marker.visible_next[a]=false;
        }
    }
    return;
}

void MoveEngine::reset()
{
    int a=0;
    for(a=0;a<28;a++)
    {
        population[a].total=0;
    }

    int p1_index[]={1,1,12,12,12,12,12,17,17,17,19,19,19,19,19};
    int p2_index[]={24,24,13,13,13,13,13,8,8,8,6,6,6,6,6};
    //int p1_index[]={19,20,21,22,22,23,23,18,18,23,24,24,24,24,24};
    //int p2_index[]={6,5,4,3,3,2,2,2,2,2,1,7,7,1,1};
    for(a=0;a<15;a++)
    {
        population[p1_index[a]].total++;
        population[p2_index[a]].total--;
    }

    player=0;
    dice[0]=7;
    dice[1]=7;
    dice[2]=7;
    dice[3]=7;

    marker_current=-1;
    marker_next[0]=-1;
    marker_next[1]=-1;
    marker_next[2]=-1;
    marker_next[3]=-1;
    //allclear[0]==false;
    allclear[1]=false;
    allclear[2]=false;
    last_piece[1]=0;
    last_piece[2]=25;
}

void MoveEngine::loadGame(const LoadSave& load)
{
    for(int a=0;a<28;a++)
    {
        population[a].total=load.pop[a].total;
    }
    checkstate();
}

LoadSave MoveEngine::saveGame()
{
    LoadSave save;
    for(int a=0;a<28;a++)
    {
        save.pop[a].total=population[a].total;
    }
    return save;
}

AISettings MoveEngine::getAISettings()
{
    return ai;
}

void MoveEngine::setAISettings(const AISettings& new_ai)
{
    ai=new_ai;
}

void MoveEngine::setRules(Rules rules)
{
  move_with_pieces_out=rules.move_with_pieces_out;
  nice_dice=rules.generous_dice;
}


int MoveEngine::getPossibleMoves()
{
    int homezone[]={0,25,0};
    int lastToHomeZone=abs(last_piece[player]-homezone[player]);
    for(int field=0;field<26;field++)
    {
       
        for(int b=0;b<4;b++)
        {            
            int dice_tmp=dice[b];
            if(dice[b]!=7 && dice[b]> lastToHomeZone)
              dice_tmp=lastToHomeZone;

            int nextfield=(player==1) ? field+dice_tmp : field-dice_tmp;
           
            if(nice_dice)
            {
              if(player==1 && nextfield>homezone[1])
                nextfield=homezone[1];
              else if(player==2 && nextfield<homezone[2])
                nextfield=homezone[2];
            }

            moves[field].weight[b]=0;
            moves[field].to[b]=nextfield;

            int out_of_board[]={-1,0,25};
            if(!move_with_pieces_out && field!=out_of_board[player] && pieces_out[player])
            {
              continue;
            }
           

        
            if(dice[b]!=7 && fieldColor(field)==player )  //player can only move his own pieces
            {
                if((player==1 && nextfield > homezone[1]) || (player==2 && nextfield < homezone[2]))
                {
                  moves[field].weight[b]=0; //movement would be far out of board
                }
                else if(nextfield==homezone[player] && !allclear[player])
                {
                  moves[field].weight[b]=0; //can not rescue pieces until all are in the endzone
                }
                else if(nextfield==homezone[player] && allclear[player])
                {
                   moves[field].weight[b]=ai.rescue; //rescue your pieces : nuff said ;-)
                }
                else if(fieldColor(nextfield)==otherplayer)
                {
                    if(abs(population[nextfield].total)>1) //can not move to this field
                        moves[field].weight[b]=0;
                    else if(abs(population[nextfield].total)==1) //eliminate opponent : very nice
                        moves[field].weight[b]=ai.eliminate;
                }
                else if(fieldColor(nextfield)==player) //nextfield already occupied by player
                {
                    if(abs(population[field].total)==2) //expose own piece : not diserable
                        moves[field].weight[b]=ai.expose;
                    else if(abs(population[nextfield].total)>1) //own pices already there : safe
                        moves[field].weight[b]=ai.safe;
                    else if(abs(population[nextfield].total)==1) //protect own piece : most importatnt
                        moves[field].weight[b]=ai.protect;
                }
                else if(population[nextfield].total==0) //nextfield empty
                {
                    if(abs(population[field].total)==2) //expose own piece : not diserable
                        moves[field].weight[b]=ai.expose;
                    else
                        moves[field].weight[b]=ai.empty;
                }
                else
                    moves[field].weight[b]=0; //default.
            }



            else
                moves[field].weight[b]=0; //dice already used or field not used by player
        }



    }

    int total=0;
    for(int field=0;field<26;field++)
    {
        total+=moves[field].weight[0]+moves[field].weight[1]+moves[field].weight[2]+moves[field].weight[3];
    }
    return total;
}

void MoveEngine::move(const int& from, int to, const int& dice)
{
    //qDebug("%d moves from %d to %d (%d) with dice %d",player,from,to,to-from,dice);

    if(player==1 && to==25)
        to=26;
    if(player==2 && to==0)
        to=27;

    //if space is occupied by enemy move pieces to startzone
    if(fieldColor(to)==otherplayer)
    {
        population[to].total=0;
        if(otherplayer==1)
            population[0].total++;
        else
            population[25].total--;
    }

    if(player==1)
    {
        population[from].total--;
        population[to].total++;
    }
    else  //player=2
    {
        population[from].total++;
        population[to].total--;
    }

    if(dice==1)
        emit done_dice1();
    else if(dice==2)
        emit done_dice2();
    else if(dice==3)
        emit done_dice3();
    else
        emit done_dice4();

    if(abs(population[26].total)==15)
        emit player_finished(1);
    if(abs(population[27].total)==15)
        emit player_finished(2);
}

void MoveEngine::checkstate()
{
    //check if pieces are out
    pieces_out[1]=(population[0].total>0) ? true : false;
    pieces_out[2]=(population[25].total<0) ? true : false;
    
    //check if all pieces are in the endzones
    allclear[1]=true;
    allclear[2]=true;

    last_piece[1]=25;
    bool found_last_piece1=false;
    last_piece[2]=0;
    
    for(int a=0;a<26;a++)
    {
        if(a<19 && population[a].total>0)
            allclear[1]=false;
        if(a>6 && population[a].total<0)
            allclear[2]=false;
            
        if(population[a].total>0 && !found_last_piece1)
        {
          last_piece[1]=a;
          found_last_piece1=true;
        }
        if(population[a].total<0)
          last_piece[2]=a;
    }
}

void MoveEngine::nomarker(Marker& marker)
{
    marker.visible_current=false;
    marker_current=-1;
    for(int a=0;a<4;a++)
    {
        marker.x_next[a]=0;
        marker.y_next[a]=0;
        marker_next[a]=-1;
        marker.visible_next[a]=false;
    }
}

int MoveEngine::fieldColor(const int& index) const
{
    if(population[index].total>0)
        return 1;
    else if(population[index].total<0)
        return 2;
    else
        return 0;
}


