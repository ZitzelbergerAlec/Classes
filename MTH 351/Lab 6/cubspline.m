function [res,time,itnum]=cubspline(n)
% n = number of data points (xj,yj)

  itnum=1; % count number of iterations

  % for simplicity we assume data is equally spaced
  h=1/(n-1);  
  xj=[0:h:1]';
  
  %example: yj = Pm(xj) (Legendre polynomial of degree m)
  m=15;
  pm=legendre(m,xj);
  yj=pm(1,:)';

  % setup right hand side
  % from Equation 4.64
  b=(yj(3:n)-2*yj(2:n-1)+yj(1:n-2))/h^2;

  % with uniformly spaced data, linear system becomes
  % from Equation 4.64
  ld=ones(n-2,1)/6;
  di=2*ones(n-2,1)/3;
  ud=ones(n-2,1)/6;
  A=diag(ld(1:n-3),-1)+diag(di)+diag(ud(1:n-3),1);
  
  tic % for timing
  
  % solve system using Gaussian elimination and backsubstitution
  [M,lu,piv] = GEpivot(A,b);
  
  % alternatively, solve system using an iterative method
  % (be sure to comment out GEpivot above when uncommenting one of these) 
  %%[M, iflag, itnum] = Jacobi(A,b,b,eps,100000);
  %%[M, iflag, itnum] = GS(A,b,b,eps,100000);
  %%[M, iflag, itnum] = CG(A,b,b,eps,100000);
  
  % alternatively, use tridiag solver
  % (be sure to comment out lines above when uncommenting this line) 
  %%[M, alpha, beta, ier] = tridiag(ld,di,ud,b,n-2,0);
  
  time=toc; % for timing
  
  % output norm of residual
  res = norm(b-A*M);
  
  % add first and last M=0 for natural spline
  Mv=[0;M;0];

  % use M values to build splines (or at least evaluate them at t):
  % from Equation 4.63 
  nt=4*m;
  t=[0:h/nt:1];
  st=0*t;
  for j=2:n,
    tj=[xj(j-1):h/nt:xj(j)-h/nt];
    st(nt*(j-2)+1:nt*(j-1))=( (xj(j)-tj).^3*Mv(j-1) ...
			      + (tj-xj(j-1)).^3*Mv(j) )/(6*h) ...
	      +( (xj(j)-tj)*yj(j-1) + (tj-xj(j-1))*yj(j) )/h ...
	      -h* ( (xj(j)-tj)*Mv(j-1) + (tj-xj(j-1))*Mv(j) )/6;
  end
  st(nt*(n-1)+1)=( (1-xj(n-1)).^3*Mv(n) )/(6*h) ...
	      +( (1-xj(n-1))*yj(n) )/h ...
	      -h* ( (1-xj(n-1))*Mv(n) )/6;
  
  % actual Legendre polynomial on t:
  pm=legendre(m,t);
  y=pm(1,:)';
  
  
  % plot data and cubic spline approximation
  figure
  plot(xj,yj,'o')
  hold on
  plot(t,y)
  plot(t,st,'--')
  title(sprintf(['Natural cubic spline approximation of %d degree', ...
		 'Legendre polynomial with %d nodes'],m,n));

