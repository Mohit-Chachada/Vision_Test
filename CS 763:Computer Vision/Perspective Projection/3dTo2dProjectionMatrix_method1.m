clear;
clc;
f2D = load('Features2D_dataset1.mat','f2D');
f3D = load('Features3D_dataset1.mat','f3D');
v = f2D.f2D;
V = f3D.f3D;

x = v(1,:)./v(3,:); y = v(2,:)./v(3,:);
X = V(1,:); Y = V(2,:); Z = V(3,:);

[nx ny] = size(v);
N = ny;

A = zeros(2*N,12);

for i=1:2:N
    A(i,:) = [X(i) Y(i) Z(i) 1 0 0 0 0 -x(i)*X(i) -x(i)*Y(i) -x(i)*Z(i) -x(i)];
    A(i+1,:) = [0 0 0 0 X(i) Y(i) Z(i) 1 -y(i)*X(i) -y(i)*Y(i) -y(i)*Z(i) -y(i)];
end

a = A'*A;
[t,b] = eigs(a,1,'sm');

%disp('min magnitude eigen value is');
%disp(b);
%disp('corresponding eigen vector is');
%disp(t); 

M = [t(1) t(2) t(3) t(4); t(5) t(6) t(7) t(8); t(9) t(10) t(11) t(12)];
% scaling factor
gamma = 0;
for i=1:N
    gamma = gamma + 1/(M(3,1)*X(i)+M(3,2)*Y(i)+M(3,3)*Z(i)+M(3,4));
end
gamma = gamma/N;
M = gamma*M;
disp (M); %final ans

% Validation
v_chk = M*V;
error = v-v_chk;
%disp(error);

