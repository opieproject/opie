#include <stdlib.h>
#include <stdio.h>
#include "arith.h"
#include "ppm.h"

/****************************************************************************
 * Gestion des noeuds
 ****************************************************************************/

/* 
 * Désallocation du noeud p
 */

void ppm_worker::Node_Free(UINT p) {
  node_heap[node_free_last].free_next=p;
  node_heap[p].free_next=NIL;
  node_free_last=p;
  node_free_nb++;
}

/*
 * Allocation d'un noeud
 * s'il ne reste plus de place, on désalloue le contexte le moins utilisé.
 */

UINT ppm_worker::Node_Alloc(void)  {
  UINT p;
  if (node_free_nb<=2) Context_DeleteLast();
  p=node_free_first;
  node_free_first=node_heap[node_free_first].free_next;
  node_free_nb--;
#ifdef DEBUG
  printf("Node_Alloc: p=%d\n",p);
#endif
  return p;
}

/****************************************************************************
 * Gestion des contextes 
 ****************************************************************************/


/* 
 * Mise au début de la liste des contextes du contexte c 
 */
void ppm_worker::Context_MoveFirst(UINT c) {
  NODE *ctx;
   
  if (c!=ctx_first)  {
    ctx=&node_heap[c];
    /* suppression du contexte dans la liste */
    if (c==ctx_last)  {
      ctx_last=ctx->hdr.ctx_prev;
    } else  {
      node_heap[ctx->hdr.ctx_prev].hdr.ctx_next=ctx->hdr.ctx_next;
      node_heap[ctx->hdr.ctx_next].hdr.ctx_prev=ctx->hdr.ctx_prev;
    }
    /* insertion au début de la liste */
    node_heap[ctx_first].hdr.ctx_prev=c;
    ctx->hdr.ctx_next=ctx_first;
    ctx_first=c;
  }
}

/*
 * Destruction du contexte le moins utilisé (ctx_last)
 */
void ppm_worker::Context_DeleteLast(void) {
  NODE *n;
  UINT h,h_next,node,node_next;
  USHORT *p;
   
  n=&node_heap[ctx_last];
   
  /* libération dans la table de hachage. Comme on ne dispose pas de
   * pointeur hash_prev dans les contextes, il faut parcourir toute
   * la liste. Heureusement, celle-ci est de longueur faible en moyenne
   */
  h_next=n->hdr.hash_next;
  h=h_next;
  while (h<HASH_ADDRESS) h=node_heap[h].hdr.hash_next;
  p=&hash_table[h-HASH_ADDRESS];
  while (*p!=ctx_last) p=&node_heap[*p].hdr.hash_next;
  *p=h_next;
   
  /* libération des noeuds & modification de ctx_last */
   
  if (n->hdr.sf_max>=2)  {
    node=n->hdr.sf.l.sf_next;
    while (1) {
      node_next=node_heap[node].sf.sf_next;
      Node_Free(node);
      if (node_next==NIL) break;
      node=node_next;
    }
  } 

  node=ctx_last;
  ctx_last=n->hdr.ctx_prev;
  Node_Free(node);
  ctx_nb--;
}

/* 
 * Création d'un nouveau contexte avec un seul symbole sym de fréquence 1
 * Utilisation implicite de sym_context et sym_hash.
 * Libération de mémoire si nécessaire, et mise en premier dans la liste
 */
void ppm_worker::Context_New(int sym,int order) {
  NODE *ctx;
  UINT i,c;

#ifdef DEBUG   
  printf("Context_New: sym=%d o=%d\n",sym,order);
#endif

  c=Node_Alloc();
  ctx=&node_heap[c];
   
  /* mise du contexte en tête de la liste */
  ctx->hdr.ctx_next=ctx_first;
  node_heap[ctx_first].hdr.ctx_prev=c;
  ctx_first=c;
  ctx_nb++;
   
  /* insertion dans la table de hachage */
  ctx->hdr.hash_next=hash_table[sym_hash[order]];
  hash_table[sym_hash[order]]=ctx_first;

  /* initialisation du contexte */
  ctx->hdr.order=order;
  for(i=0;i<order;i++) ctx->hdr.sym[i]=sym_context[i+1];

  ctx->hdr.sf_max=0;
  ctx->hdr.sf.sf[0].sym=sym;
  ctx->hdr.sf.sf[0].freq=1;
#ifdef DEBUG
  Context_Print(ctx_first);
#endif
}

/*
 * Ajout d'un nouveau symbole au contexte c
 */

void ppm_worker::Context_NewSym(int sym,UINT c) {
  NODE *n,*m;
  UINT p,sf_max;

#ifdef DEBUG   
  printf("Context_NewSym: sym=%d c=%d\n",sym,c);
  Context_Print(c);
#endif

  n=&node_heap[c];
  sf_max=n->hdr.sf_max;
  n->hdr.sf_max++;
  if (sf_max==0)  {
    n->hdr.sf.sf[1].sym=sym;
    n->hdr.sf.sf[1].freq=1;
  } else if (sf_max==1)  {
    p=Node_Alloc();
    m=&node_heap[p];
    m->sf.sf[0]=n->hdr.sf.sf[0];
    m->sf.sf[1]=n->hdr.sf.sf[1];
    m->sf.sf[2].sym=sym;
    m->sf.sf[2].freq=1;
    m->sf.sf_next=NIL;
    n->hdr.sf.l.sf_next=p;
    n->hdr.sf.l.freq_tot=((UINT)m->sf.sf[0].freq+(UINT)m->sf.sf[1].freq+1);
  } else  {
    n->hdr.sf.l.freq_tot++;
    m=&node_heap[n->hdr.sf.l.sf_next];
    while (sf_max>=NODE_SFNB)  {
      sf_max-=NODE_SFNB;
      m=&node_heap[m->sf.sf_next];
    }
    sf_max++;
    if (sf_max==NODE_SFNB)  {
      p=Node_Alloc();
      m->sf.sf_next=p;
      m=&node_heap[p];
      m->sf.sf_next=NIL;
      sf_max=0;
    }
    m->sf.sf[sf_max].sym=sym;
    m->sf.sf[sf_max].freq=1;
  }
#ifdef DEBUG
  Context_Print(c);
#endif
}


#ifdef STAT
int hash_nb=1;
int hash_cnt=0;
#endif 

/*
 * Recherche d'un contexte, utilisation de façon implicite de sym_context
 * et de sym_hash.
 * C'est une procédure très critique qui doit être particulièrement optimisée
 */

UINT ppm_worker::Context_Search(int order)  {
  UCHAR *sym;
  UINT i,p;
  NODE *n;
#ifdef DEBUG   
  printf("Context_Search: o=%d\n",order);
#endif
   
  p=hash_table[sym_hash[order]];
  sym=&sym_context[1];
#ifdef STAT
  hash_nb++;
#endif
  while (p<HASH_ADDRESS)  {
#ifdef STAT
    hash_cnt++;
#endif
    n=&node_heap[p];
    if (n->hdr.order==order)  {
      if (order==0) return p;
      i=0;
      while (sym[i]==n->hdr.sym[i]) {
	i++;
	if (i==order) return p;
      }
    }
    p=n->hdr.hash_next;
  }
  return HASH_ADDRESS;
}

/*
 * Cette macro est HORRIBLE mais permet de simplifier beaucoup le parcours
 * des listes de couples symbole,fréquence tout en ayant un code rapide.
 * Une alternative élégante mais lente aurait été de passer une fonction
 * en paramètre contenant le code à exécuter
 */

#define SF_Read(n,p,code_to_execute) \
{\
	 UINT nb,i;\
	 nb=(UINT)n->hdr.sf_max+1;\
   if (nb<=HDR_SFNB)  {\
      p=&n->hdr.sf.sf[0];\
   } else {\
      p=&node_heap[n->hdr.sf.l.sf_next].sf.sf[0];\
      while (nb>NODE_SFNB) {\
				 for(i=0;i<NODE_SFNB;i++)  {\
						code_to_execute;\
						p++;\
				 }\
				 p=&node_heap[ *((USHORT *)p) ].sf.sf[0];\
				 nb-=NODE_SFNB;\
      }\
   }\
   for(i=0;i<nb;i++)  {\
			code_to_execute;\
      p++;\
   }\
}


/* 
 * Renormalisation d'un contexte, ie, division de toutes les fréquences 
 * par 2 et élimination des symboles de fréquence nulle  
 * Note: le contexte obtenu n'est jamais vide. 
 * Une amélioration prévue mais non implémentée serait de trier le contexte
 * dans l'ordre des fréquences décroissantes pour accélérer la recherche.
 * Les gains en vitesse seraient de toute façon assez faibles car les
 * contextes sont de toute façon à peu près triés vu leur méthode de 
 * construction: les caractères sont ajoutés à la fin de la liste 
 */
void ppm_worker::Context_Renorm(UINT ctx) {
  NODE *n,*m;
  UINT a,b,c,i,freq_tot,sf_nb;
  SYMFREQ s,*p,tab_sf[SYM_NB];
	 
#ifdef DEBUG   
  printf("Context_Renorm: c=%d\n",ctx);
  Context_Print(ctx);
#endif 
   
  n=&node_heap[ctx];
  freq_tot=0;
  sf_nb=0;
	 
  SF_Read(n,p, {
    s=*p;
    s.freq=s.freq/2;
    if (s.freq!=0) {
      freq_tot+=s.freq;
      tab_sf[sf_nb]=s;
      sf_nb++;
    }
  } );
	 
	 
  /* libération des noeuds utilisés pour stocker les symboles */
  if (n->hdr.sf_max>=HDR_SFNB) {
    a=n->hdr.sf.l.sf_next;
    do {
      b=node_heap[a].sf.sf_next;
      Node_Free(a);
      a=b;
    } while (a!=NIL);
  }
				 
  /* reconstruction de la liste des "sf_nb" symboles d'apres le tableau
   * "tab_sf"
   */ 

  n->hdr.sf_max=sf_nb-1;
  if (sf_nb<=HDR_SFNB)  {
    for(i=0;i<sf_nb;i++) n->hdr.sf.sf[i]=tab_sf[i];
  } else  {
    a=Node_Alloc();
    n->hdr.sf.l.sf_next=a;
    n->hdr.sf.l.freq_tot=freq_tot;
    m=&node_heap[a];
    i=0;
    c=0;
    while (1) {
      m->sf.sf[c]=tab_sf[i];
      i++;
      if (i==sf_nb) break;
      c++;
      if (c==NODE_SFNB)  {
	c=0;
	a=Node_Alloc();
	m->sf.sf_next=a;
	m=&node_heap[a];
      }
    }
    m->sf.sf_next=NIL;
  }

#ifdef DEBUG
  Context_Print(ctx);
#endif
}


/*
 * Mise à jour des index dans la table de hachage et des caractères du
 * contexte courant.
 * La fonction de hachage a été choisie de façon empirique en controlant
 * qu'elle donne en moyenne de bons résultats.
 */
void ppm_worker::Hash_Update(int sym) {
  UINT i,k;
   
  for(i=ORDER_MAX;i>=2;i--) 
    sym_context[i]=sym_context[i-1];
  sym_context[1]=sym;
   
  for(i=ORDER_MAX;i>=2;i--) {  
    k=sym_hash[i-1];
    sym_hash[i]=( (k<<6)-k+sym ) & (HASH_SIZE-1);
  }
  sym_hash[1]=sym+1;
}


/****************************************************************************
 * Système d'exclusion des symboles 
 ****************************************************************************/


/*
 * Remise à zéro du tableau d'exclusion des symboles 
 */
void ppm_worker::Sym_ExcludeReset(void) {
  UINT i;
   
  sym_excl_code++;
  if (sym_excl_code==0)  {
    for(i=0;i<SYM_NB;i++) sym_excl[i]=0;
    sym_excl_code=1;
  }
}


/****************************************************************************
 * Initialisation et Libération mémoire 
 ****************************************************************************/

/*
 * Initialisation des structures de données du compresseur/décompresseur
 * retourne 0 si tout va bien
 */
int ppm_worker::PPM_Init(unsigned short NODE_NBMAX) {
  UINT i;
  node_heap= new NODE[NODE_NBMAX];
  hash_table= new USHORT[HASH_SIZE];
  if (node_heap==NULL || hash_table==NULL)  {
    if (node_heap!=NULL) delete [] node_heap;
    if (hash_table!=NULL) delete [] hash_table;
    return 1;
  }
  /* noeuds: tous vides */
  for(i=0;i<=(NODE_NBMAX-2);i++) node_heap[i].free_next=i+1;
  node_heap[NODE_NBMAX-1].free_next=NIL;
  node_free_first=0;
  node_free_last=NODE_NBMAX-1;
  node_free_nb=NODE_NBMAX;
   
  /* contextes */
  for(i=0;i<HASH_SIZE;i++) hash_table[i]=HASH_ADDRESS+i;
   
  /* cette initialisation n'est pas sûre mais simplifie beaucoup le code:
   * on suppose que le premier contexte sera alloué dans le noeud 0
   */
  ctx_first=0;
  ctx_last=0;
  ctx_nb=0;
   
  /* contexte courant */
  for(i=0;i<=ORDER_MAX;i++) sym_context[i]=0;
  for(i=0;i<=ORDER_MAX;i++) sym_hash[i]=0;

  /* système d'exclusion des caractères */
  sym_excl_code=0xFF;
	 
  return 0;
}

/*
 * Fin de la compression/décompression: on libère la mémoire
 */
void ppm_worker::PPM_End(void) {
  free(hash_table);
  free(node_heap);
}

/****************************************************************************
 * Décodage et décompression
 ****************************************************************************/

/*
 * Décodage: cf Encode_NewSym
 */
int ppm_worker::Decode_NewSym(void) {
  UINT i,freq_tot,freq_cum,f;
  UCHAR code;
	 
  code=sym_excl_code;
  freq_tot=0;
  for(i=0;i<SYM_NB;i++) if (sym_excl[i]!=code) freq_tot++;
  f=arith.Arith_DecodeVal(freq_tot+SYM_SPECIAL_NB);
  if (f>=freq_tot) {
    /* cas d'un symbole spécial */ 
    arith.Arith_Decode(f,f+1,freq_tot+SYM_SPECIAL_NB);
    return SYM_NB+f-freq_tot;
  } else  {
    i=0;
    freq_cum=0;
    while (1)  {
      if (sym_excl[i]!=code)  {
	freq_cum++;
	if (freq_cum>f) break;
      }
      i++;
    }
    arith.Arith_Decode(freq_cum-1,freq_cum,freq_tot+SYM_SPECIAL_NB);
    return i;
  }
}

/*
 * Décodage: cf Decode_NoExclude
 */
int ppm_worker::Decode_NoExclude(UINT ctx) {
  NODE *n;
  UCHAR code;
  UINT i,f,freq_tot,freq_cum,freq_sym,sf_nb;
  SYMFREQ *p,s;
	 
	 
  n=&node_heap[ctx];
  code=sym_excl_code;
	 
  /* Calcul de la somme des fréquences des caractères */
  if (n->hdr.sf_max<HDR_SFNB)  {
    freq_tot=0;
    for(i=0;i<=n->hdr.sf_max;i++) freq_tot+=n->hdr.sf.sf[i].freq;
  } else  {
    freq_tot=n->hdr.sf.l.freq_tot;
  }
	 
  /* décodage */
  sf_nb=(UINT) n->hdr.sf_max+1;
  f=arith.Arith_DecodeVal(freq_tot+sf_nb);
  if (f>=freq_tot)  {
    /* gestion du code ESCAPE */
			
    /* marquage des caractères utilisés */
    SF_Read(n,p, { sym_excl[p->sym]=code; });
				 
    /* décodage ESCAPE */
    arith.Arith_Decode(freq_tot,freq_tot+sf_nb,freq_tot+sf_nb);
    return SYM_ESCAPE;
  }
	 
  /* recherche du caractère en calculant la fréquence */
  freq_cum=0;
  SF_Read(n,p, {
    s=*p;
    freq_cum+=s.freq;
    if (freq_cum>f) goto decode_sym;
  } );
	 
 decode_sym:

  freq_sym=s.freq;
  p->freq=freq_sym+1;
  if (n->hdr.sf_max>=HDR_SFNB) n->hdr.sf.l.freq_tot=freq_tot+1;

  arith.Arith_Decode(freq_cum-freq_sym,freq_cum,freq_tot+sf_nb);
   
  /* test de la renormalisation */
  if (freq_sym==(RENORM_FREQSYM-1) || freq_tot>=RENORM_FREQTOT) { 
    Context_Renorm(ctx);
  }
  return s.sym;
}


/*
 * Décodage: cf Encode_Exclude
 */

int ppm_worker::Decode_Exclude(UINT ctx) {
  UINT sf_nb,freq_sym,freq_cum,freq_tot,f;
  NODE *n;
  SYMFREQ s,*p;
  UCHAR code;
	 
  n=&node_heap[ctx];
  code=sym_excl_code;

  freq_tot=0;
  sf_nb=0;

  SF_Read( n,p, {
    s=*p;
    if (sym_excl[s.sym]!=code) 
      {
	freq_tot+=s.freq;
	sf_nb++;
      }
  } );
	 
	 
  f=arith.Arith_DecodeVal(freq_tot+sf_nb);
	 
  if (f>=freq_tot) {
			
    /* ESCAPE */
			
    SF_Read(n,p, { sym_excl[p->sym]=code; } );
			
    arith.Arith_Decode(freq_tot,freq_tot+sf_nb,freq_tot+sf_nb);
			
    return SYM_ESCAPE;
  } else  {
			
    /* recherche du caractère */
			
    freq_cum=0;
    SF_Read(n,p, {
      s=*p;
      if (sym_excl[s.sym]!=code)  {
	freq_cum+=s.freq;
	if (freq_cum>f) goto decode_sym;
      }
    } );
			
  decode_sym:
			
    /* incrémentation de la fréquence */
			
    freq_sym=p->freq;
    p->freq=freq_sym+1;
    if (n->hdr.sf_max>=HDR_SFNB) n->hdr.sf.l.freq_tot++;
			
    /* décodage du caractère */
			
    arith.Arith_Decode(freq_cum-freq_sym,freq_cum,freq_tot+sf_nb);
   
    if (freq_sym==(RENORM_FREQSYM-1) || freq_tot>=RENORM_FREQTOT) { 
      Context_Renorm(ctx);
    }
			
    return s.sym;
  }
}


/*
 * Décodage d'un symbole
 */
int ppm_worker::PPM_Decode(void) {
  int i,order,sym;
  UINT ctx,ctx_tab[ORDER_MAX+1],ctx_last;
   
   
  /* recherche de l'ordre maximum */

  Sym_ExcludeReset();
  order=ORDER_MAX;
  ctx_last=NIL;
  while (1)  {
    ctx=Context_Search(order);
    ctx_tab[order]=ctx;
    if (ctx<HASH_ADDRESS)  {
      Context_MoveFirst(ctx); 
      if (ctx_last==NIL)
	sym=Decode_NoExclude(ctx);
      else
	sym=Decode_Exclude(ctx);
      if (sym!=SYM_ESCAPE) break;
      ctx_last=ctx;
    } 
    order--;
    if (order==-1)  {
      sym=Decode_NewSym();
      if (sym>=SYM_NB) return sym;
      break;
    }
  }

  for(i=order+1;i<=ORDER_MAX;i++) { 
    if (ctx_tab[i]>=HASH_ADDRESS) 
      Context_New(sym,i);
    else 
      Context_NewSym(sym,ctx_tab[i]);
  }
   
  Hash_Update(sym);

  return sym;
}

/*
 * Décompression: idem
 */

#ifdef STAT

/****************************************************************************
 * Statistiques 
 ****************************************************************************/


void ppm_worker::PrintStat(void)  {
  fprintf(stderr,"free=%d ctx_nb=%d hash_moy=%0.2f\n",
	  node_free_nb,ctx_nb,
	  (float)hash_cnt/(float)hash_nb);
	 
}

/*
 * Impression d'un caractère
 */
void ppm_worker::Sym_Print(int c) {
  if (c>=32 && c<=126) printf("%c",c); else printf("\\%2X",c);
}

/*
 * Impression couple SYMFREQ
 */

void ppm_worker::SF_Print(SYMFREQ s) {
  Sym_Print(s.sym);
  printf(":%d ",s.freq);
}

/*
 * Impression du contenu d'un contexte
 * utilisé pour les tests 
 */

void ppm_worker::Context_Print(UINT c) {
  NODE *n;
  int i,sf_max,sf_nb,sf_freq;
   
  n=&node_heap[c];
   
  sf_max=n->hdr.sf_max;
  sf_nb=sf_max+1;
  if (sf_max>=2) sf_freq=n->hdr.sf.l.freq_tot;
  else  {
    sf_freq=0;
    for(i=0;i<=sf_max;i++) sf_freq+=n->hdr.sf.sf[i].freq;
  }

  printf("Ctx=%d: hash_n=%d ctx_p=%d ctx_n=%d o=%d sf_nb=%d sf_freq=%d\n",
	 c,n->hdr.hash_next,n->hdr.ctx_prev,n->hdr.ctx_next,
	 n->hdr.order,sf_nb,sf_freq);
  for(i=0;i<n->hdr.order;i++) Sym_Print(n->hdr.sym[i]);
  printf(": ");
  if (sf_max<=1) {
    for(i=0;i<=sf_max;i++) SF_Print(n->hdr.sf.sf[i]);
  } else  {
    n=&node_heap[n->hdr.sf.l.sf_next];
    i=0;
    while (1) {
      SF_Print(n->sf.sf[i]);
      if (sf_max==0) break;
      i++;
      sf_max--;
      if (i==NODE_SFNB)  {
	i=0;
	n=&node_heap[n->sf.sf_next];
      }
    }
  }
  printf("\n");
}


/*
 * Nombre total de contextes et nombre de contextes de longueur données.
 * Utilisé pour les statistiques
 */

void ppm_worker::Context_Statistic(void) {
  UINT i,p;
  int tab[SYM_NB+1],tot,cnt;
   
  for(i=0;i<=SYM_NB;i++) tab[i]=0;
  tot=0;
   
  p=ctx_first;
  do {
    cnt=node_heap[p].hdr.sf_max+1;
    tab[cnt]++;
    tot++;
    p=node_heap[p].hdr.ctx_next;
  } while (p!=ctx_last);
   
   
  printf("Context_Statistic: ");
  for(i=1;i<=SYM_NB;i++)  {
    printf("%d:%d (%0.2f%%),",i,tab[i],(float)tab[i]/(float)tot*100.0);
  }
  printf("\n");
}

#endif

