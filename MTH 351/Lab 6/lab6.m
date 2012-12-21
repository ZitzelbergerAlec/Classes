%% Lab 6
% David Merrick


% 1. 
%   a.
lsq(1)
lsq(2)
lsq(3)

% Degree 1: Max error = 0.1548
% Degree 2: Max error = 0.0150
% Degree 3: Max error = 0.0011

%   b.
lsq(4)
lsq(5)
lsq(6)

% Degree 4: Max error = 5.8444e-05
% Degree 5: Max error = 1.9226e-04
% Degree 6: Max error = 0.0015

% Max error stops decreasing at degree 5.

%   c.
lsq(7)
lsq(8)
lsq(9)

% At degree 9, there are visible errors in the approximation.

%   d.
lsq(10)
lsq(11)
lsq(12)

% maxerr on degree 10: 2.3753.
% maxerr on degree 11: 21.8856.
% maxerr on degree 12: 49.4773.
% The maximum error appears to be increasing exponentially.

%   e.
% Polynomial approximations of e^x of degree 1-5 are more accurate than
% higher-degree approximations of this function. This does not necessarily mean that,
% in general, low-degree polynomials are better at approximating functions
% than high-degree polynomials. We have only tested this function. 
        
%   f.

% The 4x4 Hilbert matrix is not diagonally dominant, a prerequisite for Jacobian convergence,
% so this should not converge using Jacobian. 

m=4
A=hilb(m+1);
N=diag(diag(A));
P=N-A;
norm(inv(N)*P)

m=10
A=hilb(m+1);
N=diag(diag(A));
P=N-A;
norm(inv(N)*P)

m=30
A=hilb(m+1);
N=diag(diag(A));
P=N-A;
norm(inv(N)*P)

m=40
A=hilb(m+1);
N=diag(diag(A));
P=N-A;
norm(inv(N)*P)

m=100
A=hilb(m+1);
N=diag(diag(A));
P=N-A;
norm(inv(N)*P)

m=200
A=hilb(m+1);
N=diag(diag(A));
P=N-A;
norm(inv(N)*P)

% For m=4, result is 4.6738. For m=10, result is 11.2735. For m=30, result
% is 33.2966. For m=40, result is 44.3076. For m=70, result is 77.395. For
% m=100, result is 110.3709. For m=200, result is 270.2244. As predicted, this does not
% converge using Jacobian iteration, so this method could not be applied to this linear system.

%   g.
% Gauss-Seidel can be applied to any matrix with non-zero elements on the diagonals, 
% but convergence is only guaranteed if the matrix is either diagonally dominant, 
% or symmetric and positive definite. The Hilbert matrix is not
% diagonally dominant, but is symmetric, so it would be reasonable to
% predict that this will converge for the Hilbert matrix.

m=4
A=hilb(m+1);
N=tril(A);
P=N-A;
norm(inv(N)*P)

m=10
A=hilb(m+1);
N=tril(A);
P=N-A;
norm(inv(N)*P)

m=30
A=hilb(m+1);
N=tril(A);
P=N-A;
norm(inv(N)*P)

m=40
A=hilb(m+1);
N=tril(A);
P=N-A;
norm(inv(N)*P)

m=70
A=hilb(m+1);
N=tril(A);
P=N-A;
norm(inv(N)*P)

m=100
A=hilb(m+1);
N=tril(A);
P=N-A;
norm(inv(N)*P)

m=200
A=hilb(m+1);
N=tril(A);
P=N-A;
norm(inv(N)*P)

% For m=4, result is 1.0003. For m=10, result is 1.0015. For m=30, result
% is 1.0032. For m=40, result is 1.0035. For m=70, result is 1.0039. For
% m=100, result is 1.0041. For m=200, result is 1.0043. Clearly, as predicted, this does 
% converge using Gauss-Seidel Iteration, so this method could be applied to this linear system.

%   2.
n=2
gaussweights(n)

n=4
gaussweights(n)

n=8
gaussweights(n)

% For n = 2,4,8, the calculated values match the values in Table 5.7 in the
% book.

n=32
gaussweights(n)

n=64
gaussweights(n)

% For n=32, the condition number is 1.2827e+12. For n=64, the condition
% number is 1.2338e+20. The condition number relates the reative accuracy
% of the input to the relative accuracy of the output. With a high
% condition number, small changes in the input can produce huge errors, and this is
% clearly demonstrated on the plot.

%   c. 
% Convergence for Jacobian iteration requires that the matrix is
% diagonally-dominant. With the Vandermonde matrix, this is not the case.
% So Jacobion iteration will not converge for this linear system.

%   d. 
% Convergence for Gauss-Seidel requires  that the matrix is either
% diagonally-dominant, symmetrical, or positive definite. The Vandermonde
% matrix meets none of these criteria, so that method will not converge for
% this linear system.

%%   3. 
cubspline(8)
cubspline(16)

%   a. GEPivot
tot641=0;
tot1281=0;
tot2561=0;
for i=1:3
    [res64, time64, itnum64] = cubspline(64);
    [res128, time128, itnum128] = cubspline(128);
    [res256, time256, itnum256] = cubspline(256);
    tot641 = tot64 + time64;
    tot1281 = tot128 + time128;
    tot2561 = tot256 + time256;
end 

%   b. Jacobian
tot642=0;
tot1282=0;
tot2562=0;
for i=1:3
    [res64, time64, itnum64] = cubspline2(64);
    [res128, time128, itnum128] = cubspline2(128);
    [res256, time256, itnum256] = cubspline2(256);
    tot642 = tot64 + time64;
    tot1282 = tot128 + time128;
    tot2562 = tot256 + time256;
end 

%   c. Gauss-Seidel: ignore timings
tot643=0;
tot1283=0;
tot2563=0;
for i=1:3
    [res64, time64, itnum64] = cubspline3(64);
    [res128, time128, itnum128] = cubspline3(128);
    [res256, time256, itnum256] = cubspline3(256);
    tot643 = tot64 + time64;
    tot1283 = tot128 + time128;
    tot2563 = tot256 + time256;
end 

%   d. Conjugate-Gradient
tot644=0;
tot1284=0;
tot2564=0;
for i=1:3
    [res64, time64, itnum64] = cubspline4(64);
    [res128, time128, itnum128] = cubspline4(128);
    [res256, time256, itnum256] = cubspline4(256);
    tot644 = tot64 + time64;
    tot1284 = tot128 + time128;
    tot2564 = tot256 + time256;
end 

%   e. Tridiag
tot645=0;
tot1285=0;
tot2565=0;
for i=1:3
    [res64, time64, itnum64] = cubspline5(64);
    [res128, time128, itnum128] = cubspline5(128);
    [res256, time256, itnum256] = cubspline5(256);
    tot645 = tot64 + time64;
    tot1285 = tot128 + time128;
    tot2565 = tot256 + time256;
end 


disp(sprintf('Runtimes for solving tridiagonal systems using \n'));
disp(sprintf('n\t GEPivot\t Jacobi\t Conjugate-Gradient\t Tridiagonal\n'));
disp(sprintf('64\t %d\t %d\t %d\t %d\n', tot641/3, tot642/3, tot644/3, tot645/3));
disp(sprintf('128\t %d\t %d\t %d\t %d\n', tot1281/3, tot1282/3, tot1284/3, tot1285/3));
disp(sprintf('256\t %d\t %d\t %d\t %d\n', tot2561/3, tot2562/3, tot2564/3, tot2565/3));