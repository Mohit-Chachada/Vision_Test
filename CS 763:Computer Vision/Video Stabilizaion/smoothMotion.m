function tr_s = smoothMotion(tr,win_size)
%win_size = 3; % shud be odd number
tr_s = tr;
NF = size(tr,2);
% keeping the starting and the end motion as it is
for i=2:NF-1
    sum = zeros(size(tr(:,1)));
    k = (win_size-1)/2;
    if(NF>=win_size)
        for j=1:k
            if(i-j>=1 && i+j<=NF)
                sum = sum + tr(:,i-j)+tr(:,i+j);
            elseif(i-j<1 && i+j<=NF)
                sum = sum + tr(:,1)+tr(:,i+j);
            elseif(i-j>=1 && i+j>NF)
                sum = sum + tr(:,i-j)+tr(:,NF);
            elseif(i-j<1 && i+j>NF)
                sum = sum + tr(:,1)+tr(:,NF);
            end
        end
        sum = sum + tr(:,i);
        tr_s(:,i) = sum/win_size;
    end
end
end