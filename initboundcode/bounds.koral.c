
#include "decs.h"

int bound_x1dn_radbeamflatinflow(
		       int boundstage, int finalstep, SFTYPE boundtime, int whichdir, int boundvartype, int *dirprim, int ispstag, FTYPE (*prim)[NSTORE2][NSTORE3][NPR],
		       int *inboundloop,
		       int *outboundloop,
		       int *innormalloop,
		       int *outnormalloop,
		       int (*inoutlohi)[NUMUPDOWN][NDIM],
		       int riin, int riout, int rjin, int rjout, int rkin, int rkout,
		       int *dosetbc,
		       int enerregion,
		       int *localenerpos
				 );

/* bound array containing entire set of primitive variables */



static int bound_prim_user_general(int boundstage, int finalstep, SFTYPE boundtime, int whichdir, int boundvartype, int ispstag, int* dirprim, FTYPE (*prim)[NSTORE2][NSTORE3][NPR]);



int bound_prim_user_dir(int boundstage, int finalstep, SFTYPE boundtime, int whichdir, int boundvartype, FTYPE (*prim)[NSTORE2][NSTORE3][NPR])
{
  int dirprim[NPR];
  int pl,pliter;


  // specify location of primitives
  PALLLOOP(pl) dirprim[pl]=CENT;
  //  dualfprintf(fail_file,"start bound_prim\n"); // CHANGINGMARK
  bound_prim_user_general(boundstage, finalstep, boundtime, whichdir, boundvartype, BOUNDPRIMLOC, dirprim, prim);
  //  dualfprintf(fail_file,"end bound_prim\n"); // CHANGINGMARK

  return(0);
}


// assume single user function takes care of primitive locations
int bound_pstag_user_dir(int boundstage, int finalstep, SFTYPE boundtime, int whichdir, int boundvartype, FTYPE (*prim)[NSTORE2][NSTORE3][NPR])
{

  int dirprim[NPR];
  int pl,pliter;




  if(FLUXB!=FLUXCTSTAG) return(0); // nothing to do


  // specify location of primitives
  PALLLOOP(pl) dirprim[pl]=CENT;
  dirprim[B1]=FACE1;
  dirprim[B2]=FACE2;
  dirprim[B3]=FACE3;

#if(0)
  // Assume JCM setup bound_prim_user_general() correctly, so turned this off
  // GODMARK: assume non-field velocity not set and have to have something reasonable
  // use global values of non-field parts at present time
  // note that bound_pstag() setup loops to be over only B1..B3, but user may violate this and just stick in something so no failures even if not using data
  FULLLOOP PLOOPNOB1(pl) MACP0A1(prim,i,j,k,pl)=MACP0A1(p,i,j,k,pl);
  FULLLOOP PLOOPNOB2(pl) MACP0A1(prim,i,j,k,pl)=MACP0A1(p,i,j,k,pl);
#endif


  // assume before calling this that bound_pstag() setup PLOOPINTERP so only doing B1,B2,B3 (even though user may not respect this in bound_prim_user_general() -- which is ok since non-field quantities in pstag aren't needed -- may be problem if user_general() assumes primitive is reasonable)
  //  dualfprintf(fail_file,"start bound_pstag\n"); // CHANGINGMARK
  bound_prim_user_general(boundstage, finalstep, boundtime, whichdir, boundvartype, BOUNDPSTAGLOC, dirprim, prim);
  //  dualfprintf(fail_file,"end bound_pstag\n"); // CHANGINGMARK



  return (0);
}



// user boundary routine
int bound_prim_user_general(int boundstage, int finalstep, SFTYPE boundtime, int whichdir, int boundvartype, int ispstag, int* dirprim, FTYPE (*prim)[NSTORE2][NSTORE3][NPR])
{
  int inboundloop[NDIM];
  int outboundloop[NDIM];
  int innormalloop[NDIM];
  int outnormalloop[NDIM];
  int inoutlohi[NUMUPDOWN][NUMUPDOWN][NDIM];
  int riin,riout,rjin,rjout,rkin,rkout;
  int dosetbc[COMPDIM*2];
  int enerregion;
  int *localenerpos;
  int dir;
  int donebc[COMPDIM*2];



  DIRLOOP(dir) donebc[dir]=0; // assume BC not done yet


  ////////////////////////
  //
  // set bound loop
  //
  ///////////////////////
  set_boundloop(boundvartype, inboundloop,outboundloop,innormalloop,outnormalloop,inoutlohi, &riin, &riout, &rjin, &rjout, &rkin, &rkout, dosetbc);


  // for CZLOOP:
  // avoid looping over region outside active+ghost grid
  // good because somewhat general and avoid bad inversions, etc.
  //  enerregion=TRUEGLOBALENERREGION;
  enerregion=ACTIVEREGION; // now replaces TRUEGLOBALENERREGION
  localenerpos=enerposreg[enerregion];
  // if(WITHINENERREGION(localenerpos,i,j,k)){
  // note that localenerpos[X1DN] sets first evolved cell




  


  if(whichdir==1){

    if((dosetbc[X1DN] || dosetbc[X1UP]) && (donebc[X1DN]==0 && donebc[X1UP]==0)){
      if( (BCtype[X1DN]==PERIODIC)&&(BCtype[X1UP]==PERIODIC) ){
        bound_x1_periodic(boundstage,finalstep,boundtime,whichdir,boundvartype,dirprim,ispstag,prim,inboundloop,outboundloop,innormalloop,outnormalloop,inoutlohi,riin,riout,rjin,rjout,rkin,rkout,dosetbc,enerregion,localenerpos);
        donebc[X1DN]=donebc[X1UP]=1;
      }
    }

    dir=X1DN;
    if(dosetbc[dir] && donebc[dir]==0){
      if((BCtype[dir]==OUTFLOW)||(BCtype[dir]==FIXEDOUTFLOW)||(BCtype[dir]==FREEOUTFLOW)){
        bound_x1dn_outflow_simple(boundstage,finalstep,boundtime,whichdir,boundvartype,dirprim,ispstag,prim,inboundloop,outboundloop,innormalloop,outnormalloop,inoutlohi,riin,riout,rjin,rjout,rkin,rkout,dosetbc,enerregion,localenerpos);
        donebc[dir]=1;
      }
      else if((BCtype[dir]==R0SING)||(BCtype[dir]==SYMM)||(BCtype[dir]==ASYMM) ){
        bound_x1dn_sym(boundstage,finalstep,boundtime,whichdir,boundvartype,dirprim,ispstag,prim,inboundloop,outboundloop,innormalloop,outnormalloop,inoutlohi,riin,riout,rjin,rjout,rkin,rkout,dosetbc,enerregion,localenerpos);
        donebc[dir]=1;
      }
      else if(BCtype[dir]==FIXEDUSEPANALYTIC){
        bound_x1dn_analytic(boundstage,finalstep,boundtime,whichdir,boundvartype,dirprim,ispstag,prim);
        donebc[dir]=1;
      }
      else if(BCtype[dir]==RADBEAMFLATINFLOW){
        bound_x1dn_radbeamflatinflow(boundstage,finalstep,boundtime,whichdir,boundvartype,dirprim,ispstag,prim,inboundloop,outboundloop,innormalloop,outnormalloop,inoutlohi,riin,riout,rjin,rjout,rkin,rkout,dosetbc,enerregion,localenerpos);	
        donebc[dir]=1;
      }

      else{
        dualfprintf(fail_file,"No x1dn boundary condition specified: %d\n",BCtype[dir]);
        myexit(7598730);
      }
    }


    dir=X1UP;
    if(dosetbc[dir] && donebc[dir]==0){
      if((BCtype[dir]==OUTFLOW)||(BCtype[dir]==FIXEDOUTFLOW)||(BCtype[dir]==FREEOUTFLOW)){
        bound_x1up_outflow_simple(boundstage,finalstep,boundtime,whichdir,boundvartype,dirprim,ispstag,prim,inboundloop,outboundloop,innormalloop,outnormalloop,inoutlohi,riin,riout,rjin,rjout,rkin,rkout,dosetbc,enerregion,localenerpos);
        donebc[dir]=1;
      }
      else if(BCtype[dir]==FIXEDUSEPANALYTIC){
        bound_x1up_analytic(boundstage,finalstep,boundtime,whichdir,boundvartype,dirprim,ispstag,prim);
        donebc[dir]=1;
      }
      else{
        dualfprintf(fail_file,"No x1up boundary condition specified: %d\n",BCtype[dir]);
        myexit(7598731);
      }
    }


  }
  else if(whichdir==2){


    if((dosetbc[X2DN] || dosetbc[X2UP]) && (donebc[X2DN]==0 && donebc[X2UP]==0)){
      if( (BCtype[X2DN]==PERIODIC)&&(BCtype[X2UP]==PERIODIC) ){
        bound_x2_periodic(boundstage,finalstep,boundtime,whichdir,boundvartype,dirprim,ispstag,prim,inboundloop,outboundloop,innormalloop,outnormalloop,inoutlohi,riin,riout,rjin,rjout,rkin,rkout,dosetbc,enerregion,localenerpos);
        donebc[X2DN]=donebc[X2UP]=1;
      }
    }


    dir=X2DN;
    if(dosetbc[dir] && donebc[dir]==0){
      if((BCtype[dir]==OUTFLOW)||(BCtype[dir]==FIXEDOUTFLOW)||(BCtype[dir]==FREEOUTFLOW)){
        bound_x2dn_outflow_simple(boundstage,finalstep,boundtime,whichdir,boundvartype,dirprim,ispstag,prim,inboundloop,outboundloop,innormalloop,outnormalloop,inoutlohi,riin,riout,rjin,rjout,rkin,rkout,dosetbc,enerregion,localenerpos);
        donebc[dir]=1;
      }
      else if(BCtype[dir]==POLARAXIS && special3dspc){
        int whichcall=1;
        bound_x2dn_polaraxis_full3d(whichcall,boundstage,finalstep,boundtime,whichdir,boundvartype,dirprim,ispstag,prim,inboundloop,outboundloop,innormalloop,outnormalloop,inoutlohi,riin,riout,rjin,rjout,rkin,rkout,dosetbc,enerregion,localenerpos);
        donebc[dir]=1;
      }
      else if((BCtype[dir]==POLARAXIS)||(BCtype[dir]==SYMM)||(BCtype[dir]==ASYMM) ){
        bound_x2dn_polaraxis(boundstage,finalstep,boundtime,whichdir,boundvartype,dirprim,ispstag,prim,inboundloop,outboundloop,innormalloop,outnormalloop,inoutlohi,riin,riout,rjin,rjout,rkin,rkout,dosetbc,enerregion,localenerpos);
        donebc[dir]=1;
      }
      else{
        dualfprintf(fail_file,"No x2dn boundary condition specified: %d\n",BCtype[dir]);
        myexit(7598732);
      }
    }


    dir=X2UP;
    if(dosetbc[dir] && donebc[dir]==0){
      if((BCtype[dir]==OUTFLOW)||(BCtype[dir]==FIXEDOUTFLOW)||(BCtype[dir]==FREEOUTFLOW)){
        bound_x2up_outflow_simple(boundstage,finalstep,boundtime,whichdir,boundvartype,dirprim,ispstag,prim,inboundloop,outboundloop,innormalloop,outnormalloop,inoutlohi,riin,riout,rjin,rjout,rkin,rkout,dosetbc,enerregion,localenerpos);
        donebc[dir]=1;
      }
      else if(BCtype[dir]==POLARAXIS && special3dspc){
        int whichcall=1;
        bound_x2up_polaraxis_full3d(whichcall,boundstage,finalstep,boundtime,whichdir,boundvartype,dirprim,ispstag,prim,inboundloop,outboundloop,innormalloop,outnormalloop,inoutlohi,riin,riout,rjin,rjout,rkin,rkout,dosetbc,enerregion,localenerpos);
        donebc[dir]=1;
      }
      else if((BCtype[dir]==POLARAXIS)||(BCtype[dir]==SYMM)||(BCtype[dir]==ASYMM) ){
        bound_x2up_polaraxis(boundstage,finalstep,boundtime,whichdir,boundvartype,dirprim,ispstag,prim,inboundloop,outboundloop,innormalloop,outnormalloop,inoutlohi,riin,riout,rjin,rjout,rkin,rkout,dosetbc,enerregion,localenerpos);
        donebc[dir]=1;
      }
      else{
        dualfprintf(fail_file,"No x2dn boundary condition specified: %d\n",BCtype[dir]);
        myexit(7598733);
      }
    }




  }
  else if(whichdir==3){


    
    if((dosetbc[X3DN] || dosetbc[X3UP]) && (donebc[X3DN]==0 && donebc[X3UP]==0)){
      if( (BCtype[X3DN]==PERIODIC)&&(BCtype[X3UP]==PERIODIC) ){
        bound_x3_periodic(boundstage,finalstep,boundtime,whichdir,boundvartype,dirprim,ispstag,prim,inboundloop,outboundloop,innormalloop,outnormalloop,inoutlohi,riin,riout,rjin,rjout,rkin,rkout,dosetbc,enerregion,localenerpos);
        donebc[X3DN]=donebc[X3UP]=1;
      }
    }


    dir=X3DN;
    if(dosetbc[dir] && donebc[dir]==0){
      if((BCtype[dir]==OUTFLOW)||(BCtype[dir]==FIXEDOUTFLOW)||(BCtype[dir]==FREEOUTFLOW)){
	bound_x3dn_outflow_simple(boundstage,finalstep,boundtime,whichdir,boundvartype,dirprim,ispstag,prim,inboundloop,outboundloop,innormalloop,outnormalloop,inoutlohi,riin,riout,rjin,rjout,rkin,rkout,dosetbc,enerregion,localenerpos);
	donebc[dir]=1;
      }
      else{
        dualfprintf(fail_file,"No x3dn boundary condition specified: %d\n",BCtype[dir]);
        myexit(34672546);
      }
    }


    dir=X3UP;
    if(dosetbc[dir] && donebc[dir]==0){
      if((BCtype[dir]==OUTFLOW)||(BCtype[dir]==FIXEDOUTFLOW)||(BCtype[dir]==FREEOUTFLOW)){
	bound_x3up_outflow_simple(boundstage,finalstep,boundtime,whichdir,boundvartype,dirprim,ispstag,prim,inboundloop,outboundloop,innormalloop,outnormalloop,inoutlohi,riin,riout,rjin,rjout,rkin,rkout,dosetbc,enerregion,localenerpos);
	donebc[dir]=1;
      }
      else{
        dualfprintf(fail_file,"No x3up boundary condition specified: %d\n",BCtype[dir]);
        myexit(34672547);
      }
    }





  }
  else{
    dualfprintf(fail_file,"No such whichdir=%d\n",whichdir);
    myexit(2436262);
  }
  






  return(0);
}





// see interpline.c
int apply_bc_line(int nprlocalstart, int nprlocalend, int*nprlocallist, int doinverse, int iterdir, int recontype, int bs, int be, FTYPE (*yin)[2][NBIGM], FTYPE (*yout)[2][NBIGM], FTYPE (*youtpolycoef)[MAXSPACEORDER][NBIGM])
{
  int flip_y(int nprlocalstart, int nprlocalend, int*nprlocallist, int iterdir, int recontype, int bs, int be, FTYPE (*y)[2][NBIGM]);

  if(doinverse==0){
    flip_y(nprlocalstart,nprlocalend,nprlocallist,iterdir, recontype, bs, be, yin);
  }
  else{
    flip_y(nprlocalstart,nprlocalend,nprlocallist,iterdir, recontype, bs, be, yin);
    flip_y(nprlocalstart,nprlocalend,nprlocallist,iterdir, recontype, bs, be, yout);
  }

  return(0);

}


#include "reconstructeno.h"

int flip_y(int nprlocalstart, int nprlocalend, int*nprlocallist, int iterdir, int recontype, int bs, int be, FTYPE (*y)[2][NBIGM])
{
  int pllocal,pl,myi;


#if( WENO_DIR_FLIP_CONS_SIGN_DN )  //flip the sign of the consrved quantities at the cylindrical axis so that they do not have a kink due to multiplication by gdet = |R|
  if( iterdir == WENO_DIR_FLIP_CONS_SIGN_DN && (recontype == CVT_C2A || recontype == CVT_A2C) && mycpupos[iterdir] == 0 ) { 
    NUMPRIMLOOP(pllocal,pl) 
      for( myi = bs; myi < 0; myi++ ) {
        y[pl][0][myi] = - y[pl][0][myi];
      }
  }
#endif
	
#if( WENO_DIR_FLIP_CONS_SIGN_UP )  //flip the sign of the consrved quantities at the cylindrical axis so that they do not have a kink due to multiplication by gdet = |R|
  if( iterdir == WENO_DIR_FLIP_CONS_SIGN_UP && (recontype == CVT_C2A || recontype == CVT_A2C)  && mycpupos[iterdir] == numbercpu[iterdir] - 1 ) { 
    NUMPRIMLOOP(pllocal,pl) 
      for( myi = N1*(iterdir==1) + N2*(iterdir==2) + N3*(iterdir==3); myi <= be; myi++ ) {
        y[pl][0][myi] = - y[pl][0][myi];
      }
  }
#endif


  return(0);

}

void remapdq( int dir, int idel, int jdel, int kdel, int i, int j, int k, FTYPE (*p2interp)[NSTORE2][NSTORE3][NPR2INTERP], 
              FTYPE (*dq)[NSTORE2][NSTORE3][NPR2INTERP], FTYPE (*pleft)[NSTORE2][NSTORE3][NPR2INTERP], FTYPE (*pright)[NSTORE2][NSTORE3][NPR2INTERP], 
              FTYPE *p2interp_l, FTYPE *p2interp_r )
{
}

void remapplpr( int dir, int idel, int jdel, int kdel, int i, int j, int k, 
                FTYPE (*p2interp)[NSTORE2][NSTORE3][NPR2INTERP], FTYPE (*dq)[NSTORE2][NSTORE3][NPR2INTERP], 
                FTYPE (*pleft)[NSTORE2][NSTORE3][NPR2INTERP], FTYPE (*pright)[NSTORE2][NSTORE3][NPR2INTERP], 
                FTYPE *p2interp_l, FTYPE *p2interp_r )
{
}


///Called after the MPI boundary routines
// many things here are copied from above
int bound_prim_user_after_mpi_dir(int boundstage, int finalstep, SFTYPE boundtime, int whichdir, int boundvartype, int ispstag, FTYPE (*prim)[NSTORE2][NSTORE3][NPR])
{
  int dirprim[NPR];
  int pliter,pl;

  int inboundloop[NDIM];
  int outboundloop[NDIM];
  int innormalloop[NDIM];
  int outnormalloop[NDIM];
  int inoutlohi[NUMUPDOWN][NUMUPDOWN][NDIM];
  int riin,riout,rjin,rjout,rkin,rkout;
  int dosetbc[COMPDIM*2];
  int enerregion;
  int *localenerpos;
  int dir;




  ////////////////////////
  //
  // set bound loop
  //
  ///////////////////////
  set_boundloop(boundvartype, inboundloop,outboundloop,innormalloop,outnormalloop,inoutlohi, &riin, &riout, &rjin, &rjout, &rkin, &rkout, dosetbc);


  // for CZLOOP:
  // avoid looping over region outside active+ghost grid
  // good because somewhat general and avoid bad inversions, etc.
  //  enerregion=TRUEGLOBALENERREGION;
  enerregion=ACTIVEREGION; // now replaces TRUEGLOBALENERREGION
  localenerpos=enerposreg[enerregion];
  // if(WITHINENERREGION(localenerpos,i,j,k)){
  // note that localenerpos[X1DN] sets first evolved cell


  if(ispstag==BOUNDPRIMLOC){
    // specify location of primitives
    PALLLOOP(pl) dirprim[pl]=CENT;
  }
  
  if(ispstag==BOUNDPSTAGLOC){
    // specify location of primitives
    PALLLOOP(pl) dirprim[pl]=CENT;
    dirprim[B1]=FACE1;
    dirprim[B2]=FACE2;
    dirprim[B3]=FACE3;
  }


  // use post-MPI values to operate poledeath() and/or polesmooth() since otherwise boundary values poledeath uses are not set yet
  if(whichdir==2){

    dir=X2DN;
    if(dosetbc[dir]){
      if(BCtype[dir]==POLARAXIS && special3dspc){
        bound_x2dn_polaraxis_full3d(2,boundstage,finalstep,boundtime,whichdir,boundvartype,dirprim,ispstag,prim,inboundloop,outboundloop,innormalloop,outnormalloop,inoutlohi,riin,riout,rjin,rjout,rkin,rkout,dosetbc,enerregion,localenerpos);
      }
    }


    dir=X2UP;
    if(dosetbc[dir]){
      if(BCtype[dir]==POLARAXIS && special3dspc){
        bound_x2up_polaraxis_full3d(2,boundstage,finalstep,boundtime,whichdir,boundvartype,dirprim,ispstag,prim,inboundloop,outboundloop,innormalloop,outnormalloop,inoutlohi,riin,riout,rjin,rjout,rkin,rkout,dosetbc,enerregion,localenerpos);
      }
    }

  }


  // can only really check boundaries after MPI is done
  // e.g. periodicx3 and ncpux3==2 and then MPi required to set k<0 and k>=ncpux2*N3 cells
  if(whichdir==1 && N2==1 && N3==1 || N3==1 && whichdir==2 || N3>1 && whichdir==3){ // not completely general conditional
    bound_checks1(boundstage,finalstep,boundtime,whichdir,boundvartype,dirprim,ispstag,prim,inboundloop,outboundloop,innormalloop,outnormalloop,inoutlohi,riin,riout,rjin,rjout,rkin,rkout,dosetbc,enerregion,localenerpos);
  }


  return(0);
}




// DUMMY GLOBAL VALUES for cases when *not* choosing that problem (so set to something when that problem for dummy values)
#if(WHICHPROBLEM!=RADBEAMFLAT)

#define RADBEAMFLAT_FRATIO 0.995
#define RADBEAMFLAT_ERAD 1.
#define RADBEAMFLAT_RHO 1.
#define RADBEAMFLAT_UU 0.1

#endif

// X1 lower for radiation beam injection
int bound_x1dn_radbeamflatinflow(
		       int boundstage, int finalstep, SFTYPE boundtime, int whichdir, int boundvartype, int *dirprim, int ispstag, FTYPE (*prim)[NSTORE2][NSTORE3][NPR],
		       int *inboundloop,
		       int *outboundloop,
		       int *innormalloop,
		       int *outnormalloop,
		       int (*inoutlohi)[NUMUPDOWN][NDIM],
		       int riin, int riout, int rjin, int rjout, int rkin, int rkout,
		       int *dosetbc,
		       int enerregion,
		       int *localenerpos
		       )

{


#pragma omp parallel  // assume don't require EOS
  {

    int i,j,k,pl,pliter;
    FTYPE vcon[NDIM],X[NDIM],V[NDIM]; 
#if(WHICHVEL==VEL3)
    int failreturn;
#endif
    int ri, rj, rk; // reference i,j,k
    FTYPE prescale[NPR];
    int jj,kk;
    struct of_geom geomdontuse[NPR];
    struct of_geom *ptrgeom[NPR];
    struct of_geom rgeomdontuse[NPR];
    struct of_geom *ptrrgeom[NPR];

    // assign memory
    PALLLOOP(pl){
      ptrgeom[pl]=&(geomdontuse[pl]);
      ptrrgeom[pl]=&(rgeomdontuse[pl]);
    }


  
    if((BCtype[X1DN]==RADBEAMFLATINFLOW)){

      // innner x BC:
      if ( (totalsize[1]>1) && (mycpupos[1] == 0) ) {



	OPENMPBCLOOPVARSDEFINELOOPX1DIR; OPENMPBCLOOPSETUPLOOPX1DIR;
	////////	LOOPX1dir{
#pragma omp for schedule(OPENMPSCHEDULE(),OPENMPCHUNKSIZE(blocksize))
	OPENMPBCLOOPBLOCK{
	  OPENMPBCLOOPBLOCK2IJKLOOPX1DIR(j,k);



	  ri=riin;
	  rj=j;
	  rk=k;


	  // ptrrgeom : i.e. ref geom
	  PALLLOOP(pl) get_geometry(ri, rj, rk, dirprim[pl], ptrrgeom[pl]);

	  
	  LOOPBOUND1IN{
    
	    //initially copying everything
	    PBOUNDLOOP(pliter,pl) MACP0A1(prim,i,j,k,pl) = MACP0A1(prim,ri,rj,rk,pl);
	  
	    //coordinates of the ghost cell
	    bl_coord_ijk_2(i,j,k,CENT,X, V);

	    FTYPE ERAD;
	    ERAD=1000.0*RADBEAMFLAT_ERAD; // making like problem24 in koral code
	    FTYPE FxRAD;
	    FxRAD=RADBEAMFLAT_FRATIO*ERAD;

	    if(V[2]>.4 && V[2]<.6){//beam to be imposed


	      //E, F^i
	      MACP0A1(prim,i,j,k,URAD0) = ERAD;
	      //	      MACP0A1(prim,i,j,k,URAD1) = 0.;
	      MACP0A1(prim,i,j,k,URAD1) = FxRAD;
	      //	      MACP0A1(prim,i,j,k,URAD2) = RADBEAMFLAT_FRATIO*MACP0A1(prim,i,j,k,URAD0);
	      MACP0A1(prim,i,j,k,URAD2) = 0.;
	      MACP0A1(prim,i,j,k,URAD3) = 0.;
	    }
	    else{ //no beam
	      MACP0A1(prim,i,j,k,URAD0) = ERAD;
	      //MACP0A1(prim,i,j,k,URAD0) = 1E-1*ERAD;
	      MACP0A1(prim,i,j,k,URAD1) = 0.;
	      MACP0A1(prim,i,j,k,URAD2) = 0.;
	      MACP0A1(prim,i,j,k,URAD3) = 0.;
	    }
	  }// end loop over inner i's
	}
      }
    }
  }// end parallel region

  return(0);
} 

