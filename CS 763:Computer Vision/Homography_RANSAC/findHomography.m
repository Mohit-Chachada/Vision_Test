function H = findHomography(pts1,pts2)
% I2=H*I1;

X1 = pts1(1,:); Y1 = pts1(2,:);
X2 = pts2(1,:); Y2 = pts2(2,:);

N = size(X1,2);
A = zeros(2*N,8);
b = zeros(2*N,1);

kk=1;
for i=1:N
    A(kk,:) = [X1(i) Y1(i) 1 0 0 0 -X2(i)*X1(i) -X2(i)*Y1(i)];
    A(kk+1,:) = [0 0 0 X1(i) Y1(i) 1 -Y2(i)*X1(i) -Y2(i)*Y1(i)];
    b(kk) = X2(i);
    b(kk+1) = Y2(i);
    kk=kk+2;
end

[U,S,V] = svd(A);

P = zeros(8,2*N);

for k=1:8
    if S(k,k) ~= 0;
        P(k,k) = 1/S(k,k);
    else
        P(k,k) = 0;
    end
end
t = V*P*U'*b;
%t=(A'*A)\(A'*b); %same result
H12 = [t(1) t(2) t(3); t(4) t(5) t(6); t(7) t(8) 1];
%scaling factor
gamma = 0;
for i=1:N
    gamma = gamma + 1/(H12(3,1)*X1(i)+H12(3,2)*Y1(i)+1);
end
gamma = gamma/N;
H12 = gamma*H12;
H = H12;
end