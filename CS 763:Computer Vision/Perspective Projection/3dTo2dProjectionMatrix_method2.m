clear;
clc;
f2D = load('Features2D_dataset1.mat','f2D');
f3D = load('Features3D_dataset1.mat','f3D');
v = f2D.f2D;
V = f3D.f3D;

x = v(1,:); y = v(2,:);
X = V(1,:); Y = V(2,:); Z = V(3,:);

[nx ny] = size(v);
N = ny;

A = zeros(2*N,11);
b = zeros(2*N,1);

for i=1:2:N
    A(i,:) = [X(i) Y(i) Z(i) 1 0 0 0 0 -x(i)*X(i) -x(i)*Y(i) -x(i)*Z(i)];
    A(i+1,:) = [0 0 0 0 X(i) Y(i) Z(i) 1 -y(i)*X(i) -y(i)*Y(i) -y(i)*Z(i)];
    b(i) = x(i);
    b(i+1) = y(i);
end

[U,S,T] = svd(A);

P = zeros(11,2*N);

for k=1:11
    if S(k,k) ~= 0;
    P(k,k) = 1/S(k,k);
    else 
        P(k,k) = 0;
    end    
end
t = T*P*U'*b;

M = [t(1) t(2) t(3) t(4); t(5) t(6) t(7) t(8); t(9) t(10) t(11) 1];
%scaling factor
gamma = 0;
for i=1:N
    gamma = gamma + 1/(M(3,1)*X(i)+M(3,2)*Y(i)+M(3,3)*Z(i)+1);
end
gamma = gamma/N;
M = gamma*M;
disp (M); %final ans

% Validation
v_chk = M*V;
error = (v-v_chk);
%disp(error);

