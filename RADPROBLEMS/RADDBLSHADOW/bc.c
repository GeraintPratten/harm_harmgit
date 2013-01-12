//returns problem specific BC
//int calc_bc(int ix,int iy,int iz,ldouble t,ldouble *uu,ldouble *pp) {

ldouble gdet_src,gdet_bc;
int iix,iiy,iiz,iv;  	  

  gdet_bc=get_g(g,3,4,ix,iy,iz);  
  ldouble gg[4][5],ggsrc[4][5],eup[4][4],elo[4][4];
  pick_g(ix,iy,iz,gg);
  pick_T(emuup,ix,iy,iz,eup);
  pick_T(emulo,ix,iy,iz,elo);
  ldouble xx=get_x(ix,0);
ldouble yy=get_x(iy,1);


ldouble angle=.4;
//printf("aa\n");

//symmetry with respect to the lower axis
if(iy<0)
    {
      iix=ix;
      iiy=-iy-1;
      iiz=iz;
      for(iv=0;iv<NV;iv++)
	{
	  pp[iv]=get_u(p,iv,iix,iiy,iiz);      
	}
      //change sign of y components
      pp[4]*=-1;
      pp[8]*=-1;
      p2u(pp,uu,gg,eup,elo);     
      return 0.;
    }
//source of light
 else if(iy>NY-1 || (ix<0 && yy>.3))
    {
      ldouble Fx,Fy,Fz,rho,E,uint,vx;
      iix=ix;
      iiy=iy;
      iiz=0;
      rho=RHOAMB;

 
      E=calc_LTE_EfromT(TLEFT);
      
      uint=calc_PEQ_ufromTrho(TAMB,rho);

      


      Fx=NLEFT*E/sqrt(1+angle*angle);
      Fz=0.;
      Fy=-NLEFT*E*angle/sqrt(1+angle*angle);
      

      vx=0.;
      
      pp[0]=rho;
      pp[1]=uint;
      pp[2]=vx;
      pp[3]=0.;
      pp[4]=0.;
      pp[5]=calc_Sfromu(rho,uint);
      pp[6]=E;
      pp[7]=Fx;
      pp[8]=Fy;
      pp[9]=Fz;

      prad_zamo2ff(pp,pp,gg,eup);

      p2u(pp,uu,gg,eup,elo);
      return 0.;
    }
  else if(ix<0.)
    {
      ldouble Fx,Fy,Fz,rho,E,uint,vx;
      ldouble pamb=calc_PEQ_ufromTrho(TAMB,RHOAMB);
      rho=RHOAMB;      
      ldouble T=TAMB;

      uint=calc_PEQ_ufromTrho(T,rho);
      E=calc_LTE_EfromT(T);

      iix=ix;
      iiy=iy;
      iiz=0;
      rho=RHOAMB;

      
      Fx=0.;
      Fz=Fy=0.;

      vx=0.;
      
      pp[0]=rho;
      pp[1]=uint;
      pp[2]=vx;
      pp[3]=0.;
      pp[4]=0.;
      pp[5]=calc_Sfromu(rho,uint);
      pp[6]=E;
      pp[7]=Fx;
      pp[8]=Fy;
      pp[9]=Fz;

      prad_zamo2ff(pp,pp,gg,eup);

      p2u(pp,uu,gg,eup,elo);
      return 0.;
    }
 

  iix=ix;
  iiz=iz;
  iiy=iy;
  //copy
  while(iix>=NX)    iix=NX-1;//iix-=NX; 
  //periodic
  while(iiy<0)    iiy=0 ;
while(iiy>=NY)    iiy=NY-1;
  //periodic
  while(iiz<0)    iiz+=NZ;
  while(iiz>=NY)    iiz-=NZ; 
 
  for(iv=0;iv<NV;iv++)
    {
      
      pp[iv]=get_u(p,iv,iix,iiy,iiz);      
    }
  p2u(pp,uu,gg,eup,elo);
return 0;
  